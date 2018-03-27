#include <sugar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <matrix_lua.h>

#define Mij(M,i,j) (M->data[((j)-1)*M->ld + ((i)-1)])
#define Mrow0(M,k) ( ((k)-1) % (M->m) )
#define Mcol0(M,k) ( ((k)-1) / (M->m) )
#define Mk(M,k)    ( M->data[Mcol0(M,k) * M->ld + Mrow0(M,k)] )

static int lua_matrix_getmethod(lua_State* L);

static int lua_matrix_tag;

static int lua_matrix_call(lua_State* L)
{
    lua_matrix_t A = lua_touserdata(L,1);
    int n = lua_gettop(L);
    int i, j;

    if (n < 2 || n > 3)
        lua_error(L, "Wrong number of arguments");

    if (!lua_isnumber(L,2) || (n == 3 && !lua_isnumber(L,3)))
        lua_error(L, "Index must be a number");

    i = (int) lua_tonumber(L,2);
    j = (n == 3) ? (int) lua_tonumber(L,3) : 1;

    if (i <= 0 || i > A->m || j <= 0 || j > A->n)
        lua_error(L, "Index out of range");

    lua_settop(L,0);
    lua_pushnumber(L, Mij(A,i,j));
    return 1;
}

static int lua_matrix_gettable(lua_State* L)
{
    lua_matrix_t A = lua_touserdata(L,1);
    int k;

    if (!lua_isnumber(L,2)) {
        if (lua_isstring(L,2))
            return lua_matrix_getmethod(L);
        else
            lua_error(L, "Index must be a number");
    }
    k = (int) lua_tonumber(L,2);

    if (k <= 0 || k > A->m * A->n)
        lua_error(L, "Index out of range");

    lua_settop(L,0);
    lua_pushnumber(L, Mk(A,k));
    return 1;
}

static int lua_matrix_settable(lua_State* L)
{
    lua_matrix_t A = lua_touserdata(L,1);
    int k;

    if (!lua_isnumber(L,2))
        lua_error(L, "Index must be a number");

    if (!lua_isnumber(L,3))
        lua_error(L, "Value must be a number");

    k = (int) lua_tonumber(L,2);
    if (k <= 0 || k > A->m*A->n)
        lua_error(L, "Index out of range");

    Mk(A,k) = lua_tonumber(L,3);

    lua_settop(L,0);
    return 0;
}

static int lua_matrix_add(lua_State* L)
{
    int i, j;
    lua_matrix_t A, B, C;
    
    if (lua_tag(L,1) != lua_matrix_tag || lua_tag(L,2) != lua_matrix_tag)
        lua_error(L, "Invalid operands");
    
    A = lua_touserdata(L,1);
    B = lua_touserdata(L,2);

    if (A->m != B->m || A->n != B->n)
        lua_error(L, "Noncomformal matrices");
    
    C = lua_matrix_create(A->m, A->n);

    for (i = 1; i <= A->m; ++i)
        for (j = 1; j <= A->n; ++j)
            Mij(C,i,j) = Mij(A,i,j) + Mij(B,i,j);

    lua_settop(L,0);
    lua_pushusertag(L, C, lua_matrix_tag);
    return 1;
}

static int lua_matrix_sub(lua_State* L)
{
    int i, j;
    lua_matrix_t A, B, C;
    
    if (lua_tag(L,1) != lua_matrix_tag || lua_tag(L,2) != lua_matrix_tag)
        lua_error(L, "Invalid operands");
    
    A = lua_touserdata(L,1);
    B = lua_touserdata(L,2);

    if (A->m != B->m || A->n != B->n)
        lua_error(L, "Noncomformal matrices");
    
    C = lua_matrix_create(A->m, A->n);

    for (i = 1; i <= A->m; ++i)
        for (j = 1; j <= A->n; ++j)
            Mij(C,i,j) = Mij(A,i,j) - Mij(B,i,j);

    lua_settop(L,0);
    lua_pushusertag(L, C, lua_matrix_tag);
    return 1;
}

static int lua_matrix_unm(lua_State* L)
{
    int i, j;
    lua_matrix_t A = lua_touserdata(L,1);
    lua_matrix_t C;

    C = lua_matrix_create(A->m, A->n);

    for (i = 1; i <= A->m; ++i)
        for (j = 1; j <= A->n; ++j)
            Mij(C,i,j) = -Mij(A,i,j);

    lua_settop(L,0);
    lua_pushusertag(L, C, lua_matrix_tag);
    return 1;
}

static int lua_matrix_mul(lua_State* L)
{
    extern int dgemm_(char* transA, char* transB, int* m, int* n, int* k,
                      double* alpha, double* A, int* ldA,
                      double* B, int* ldB,
                      double* beta, double* C, int* ldC);
#ifdef HAVE_LAPACK
    double one  = 1;
    double zero = 0;
#endif

    lua_matrix_t A, B, C;
    
    if (lua_tag(L,1) != lua_matrix_tag || lua_tag(L,2) != lua_matrix_tag)
        lua_error(L, "Invalid operands");
    
    A = lua_touserdata(L,1);
    B = lua_touserdata(L,2);

    if (A->n != B->m)
        lua_error(L, "Nonconformal matrices");

    C = lua_matrix_create(A->m, B->n);

#ifdef HAVE_LAPACK

    dgemm_("N", "N", &(C->m), &(C->n), &(A->m),
           &one, A->data, &(A->ld),
           B->data, &(B->ld),
           &zero, C->data, &(C->ld));

#else
    lua_error(L, "Dense linear algebra routines not linked\n");
#endif /* HAVE_LAPACK */

    lua_settop(L,0);
    lua_pushusertag(L, C, lua_matrix_tag);
    return 1;
}

static int lua_matrix(lua_State* L)
{
    int nargs = lua_gettop(L);
    lua_matrix_t A;
    int i,j;
    int m = 0, n = 0;
    int init_table = 0;

    for (i = 1; i <= nargs; ++i) {
        if (lua_isnumber(L,i)) {
            int val = (int) lua_tonumber(L,i);
            if (val <= 0)
                lua_error(L, "Size out of bounds");
    
            if      (m == 0)  m = val;
            else if (n == 0)  n = val;
            else              lua_error(L, "Too many size parameters");
    
        } else if (lua_istable(L,i)) {
            if (init_table == 0)
                init_table = i;
            else
                lua_error(L, "Too many initializers");
        }
    }
    
    if (m == 0) {
        if (init_table) {
            m = lua_getn(L,init_table);
            if (m == 0)
                lua_error(L, "Insufficient initializer entries");
        }
    }
    
    if (n == 0)
        n = 1;
    
    if (init_table && lua_getn(L,init_table) > m*n)
        lua_error(L, "Initializer too large");
    

    A = lua_matrix_create(m,n);

    if (init_table) {
        int k = 1;
        for (i = 1; i <= m; ++i) {
            for (j = 1; j <= n; ++j) {
                lua_rawgeti(L,init_table, k++);
                if (lua_isnumber(L,-1))
                    Mij(A,i,j) = lua_tonumber(L,-1);
                lua_pop(L,1);
            }
        }
    }

    lua_settop(L,0);
    lua_pushusertag(L, A, lua_matrix_tag);
    return 1;
}

static void print_matrix(lua_State* L, lua_matrix_t A)
{
    int i, j, c;
    int m = A->m;
    int n = A->n;
    char buf[256];

    lua_getglobal(L, "print");
    for (c = 1; c <= n; c += 6) {

        if (n > 6) {
            sprintf(buf, "  Columns %d through %d\n", 
                    c, (c+5 > n) ? n : c+5);
            lua_pushvalue(L, -1);
            lua_pushstring(L, buf);
            lua_rawcall(L, 1, 0);
        }

        for (i = 1; i <= m; ++i) {
            char num[64];

            *buf = '\0';
            for (j = c; j <= n && j < c+6; ++j) {
                if (Mij(A,i,j))
                    sprintf(num, "  % .4e", Mij(A,i,j));
                else
                    sprintf(num, "            0");
                strcat(buf, num);
            }
            lua_pushvalue(L, -1);
            lua_pushstring(L, buf);
            lua_rawcall(L, 1, 0);

        }
        buf[0] = ' ';
        buf[1] = '\0';
        lua_pushvalue(L, -1);
        lua_pushstring(L, buf);
        lua_rawcall(L, 1, 0);
    }
    lua_pop(L,1);
}

static int lua_matrix_print(lua_State* L)
{
    lua_matrix_t A = lua_touserdata(L,-1);

    if (lua_gettop(L) != 1)
        lua_error(L, "Wrong number of arguments");

    print_matrix(L, A);
    lua_settop(L, 0);
    return 0;
}

static int lua_matrix_factor(lua_State* L)
{
    extern int dgetrf_(int* m, int* n, double* A, int* ldA,
                       int* ipiv, int* info);

    lua_matrix_t A = lua_touserdata(L,-1);

    if (A->m != A->n)
        lua_error(L, "Matrix must be square");

#ifdef HAVE_LAPACK
    if (A->ipiv == NULL) {
        int info;
        A->ipiv = calloc(A->m, sizeof(int));
        dgetrf_(&(A->m), &(A->n), A->data, &(A->ld), A->ipiv, &info);    
        if (info != 0) {
            printf("dgetrf failed with error code %d\n", info);
            lua_error(L, "Error during factorization");
        }
    }
#else
    lua_error(L, "Dense linear algebra not linked\n");
#endif

    lua_settop(L,0);
    lua_pushusertag(L, A, lua_matrix_tag);
    return 1;
}

static int lua_matrix_solve(lua_State* L)
{
    extern int dgetrs_(char* trans, int* n, int* nrhs, double* A, int* ldA,
                       int* ipiv, double* B, int* ldB, int* info);

    lua_matrix_t A = lua_touserdata(L,-1);
    lua_matrix_t B;

#ifdef HAVE_LAPACK
    int info;
#endif

    if (lua_gettop(L) != 2)
        lua_error(L, "Wrong number of arguments");

    if (lua_tag(L,1) != lua_matrix_tag)
        lua_error(L, "Argument must be a matrix");
    B = lua_touserdata(L,1);

    if (A->ipiv == NULL) {
        lua_matrix_factor(L);
    }

    if (A->n != B->m)
        lua_error(L, "Dimension mismatch");

#ifdef HAVE_LAPACK
    dgetrs_("N", &(A->n), &(B->n), A->data, &(A->ld), A->ipiv, 
            B->data, &(B->ld), &info);    
    if (info != 0)
        printf("Solve failed with error code %d\n", info);

#else
    lua_error(L, "Dense linear algebra libraries not linked\n");
#endif

    lua_settop(L,0);
    lua_pushusertag(L, B, lua_matrix_tag);
    return 1;
}

static int lua_matrix_clone(lua_State* L)
{
    lua_matrix_t A = lua_touserdata(L,-1);
    lua_matrix_t B;
    int j;

    if (lua_gettop(L) != 1)
        lua_error(L, "Too many arguments");

    B = lua_matrix_create(A->m, A->n);
    for (j = 1; j <= A->n; ++j)
        memcpy(&Mij(B,1,j), &Mij(A,1,j), A->m * sizeof(double));

    if (A->ipiv) {
        B->ipiv = calloc(A->m, sizeof(int));
        memcpy(B->ipiv, A->ipiv, A->m * sizeof(int));
    }

    lua_settop(L,0);
    lua_pushusertag(L, B, lua_matrix_tag);
    return 1;
}

static int lua_matrix_slice_method(lua_State* L)
{
    lua_matrix_t A = lua_touserdata(L,-1);
    lua_matrix_t B;
    int n = lua_gettop(L);
    int i1, i2, j1 = 1, j2 = 1;

    if (n != 3 && n != 5)
        lua_error(L, "Wrong number of arguments");
    
    if (!lua_isnumber(L,1) || !lua_isnumber(L,2) ||
        (n == 5 && (!lua_isnumber(L,3) || !lua_isnumber(L,4))))
        lua_error(L, "Subscripts must be numeric");
    
    i1 = (int) lua_tonumber(L,1);
    i2 = (int) lua_tonumber(L,2);
    if (n == 5) {
        j1 = (int) lua_tonumber(L,3);
        j2 = (int) lua_tonumber(L,4);
    }
    
    if (i1 <= 0 || i2 < i1 || i2 > A->m ||
        j1 <= 0 || j2 < j1 || j2 > A->n)
        lua_error(L, "Bad subscripts");
    

    B = lua_matrix_slice(A,i1,i2,j1,j2);

    lua_settop(L,0);
    lua_pushusertag(L, B, lua_matrix_tag);
    return 1;
}

static int lua_matrix_free(lua_State* L)
{
    lua_matrix_t A = lua_touserdata(L,-1);

    if (lua_gettop(L) != 1)
        lua_error(L, "Too many arguments");

    lua_matrix_destroy(A);

    lua_settop(L,0);
    return 0;
}

static int lua_matrix_m(lua_State* L)
{
    lua_matrix_t A = lua_touserdata(L,-1);
    lua_settop(L,0);
    lua_pushnumber(L, A->m);
    return 1;
}

static int lua_matrix_n(lua_State* L)
{
    lua_matrix_t A = lua_touserdata(L,-1);
    lua_settop(L,0);
    lua_pushnumber(L, A->n);
    return 1;
}

static int lua_matrix_getmethod(lua_State* L)
{
    const char* name = lua_tostring(L,2);

    lua_pop(L,1);
    if (strcmp(name, "print") == 0)
        lua_pushcclosure(L, lua_matrix_print, 1);
    else if (strcmp(name, "free") == 0)
        lua_pushcclosure(L, lua_matrix_free, 1);
    else if (strcmp(name, "factor") == 0)
        lua_pushcclosure(L, lua_matrix_factor, 1);
    else if (strcmp(name, "solve") == 0)
        lua_pushcclosure(L, lua_matrix_solve, 1);
    else if (strcmp(name, "clone") == 0)
        lua_pushcclosure(L, lua_matrix_clone, 1);
    else if (strcmp(name, "slice") == 0)
        lua_pushcclosure(L, lua_matrix_slice_method, 1);
    else if (strcmp(name, "m") == 0)
        lua_matrix_m(L);
    else if (strcmp(name, "n") == 0)
        lua_matrix_n(L);
    else
        lua_error(L, "Invalid method name");

    return 1;
}

lua_matrix_t lua_matrix_create(int m, int n)
{
    lua_matrix_t C;

    C = calloc(1, sizeof(*C));
    C->owns_data = 1;
    C->ld = m;
    C->m  = m;
    C->n  = n;
    C->data = calloc(m*n, sizeof(double));
    return C;
}

void lua_matrix_destroy(lua_matrix_t self)
{
    if (self->ipiv)
        free(self->ipiv);

    if (self->owns_data)
        free(self->data);

    free(self);
}

lua_matrix_t lua_matrix_slice(lua_matrix_t A, int i1, int i2, int j1, int j2)
{
    lua_matrix_t C;

    C = calloc(1, sizeof(*C));
    C->owns_data = 0;
    C->ld   = A->ld;
    C->m    = (i2-i1)+1;
    C->n    = (j2-j1)+1;
    C->data = &Mij(A,i1,j1);
    return C;
}

void lua_matrix_push(lua_State* L, lua_matrix_t matrix)
{
    lua_pushusertag(L, matrix, lua_matrix_tag);
}

lua_matrix_t lua_matrix_get(lua_State* L, int index)
{
    if (index > lua_gettop(L))
        lua_error(L, "Index out of range");

    if (lua_tag(L,index) != lua_matrix_tag)
        lua_error(L, "Variable is not a matrix");

    return lua_touserdata(L,index);
}

void lua_matrix_register(lua_State* L)
{
    lua_matrix_tag = lua_newtag(L);
    lua_pushcclosure(L, lua_matrix_call, 0);
    lua_settagmethod(L, lua_matrix_tag, "function");
    lua_pushcclosure(L, lua_matrix_gettable, 0);
    lua_settagmethod(L, lua_matrix_tag, "gettable");
    lua_pushcclosure(L, lua_matrix_settable, 0);
    lua_settagmethod(L, lua_matrix_tag, "settable");
    lua_pushcclosure(L, lua_matrix_add, 0);
    lua_settagmethod(L, lua_matrix_tag, "add");
    lua_pushcclosure(L, lua_matrix_sub, 0);
    lua_settagmethod(L, lua_matrix_tag, "sub");
    lua_pushcclosure(L, lua_matrix_unm, 0);
    lua_settagmethod(L, lua_matrix_tag, "unm");
    lua_pushcclosure(L, lua_matrix_mul, 0);
    lua_settagmethod(L, lua_matrix_tag, "mul");
    lua_register(L, "matrix", lua_matrix);
}

