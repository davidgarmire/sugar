#include <sugar.h>

#include <stdlib.h>
#include <assert.h>

#include <dynarray.h>
#include <assemblem_sparse.h>

typedef struct coord_t {
    int    i;
    int    j;
    double Aij;
} coord_t;

typedef struct csc_build_t {
    int         is_csc;
    int         n;
    int         nactive;
    int         nnz;
    dynarray_t  Acoord;
    int*        colptr;
    int*        rowind;
    double*     nzval;
} csc_build_t;

static void sparse_destroy(void* pself)
{
    csc_build_t* self = pself;
    if (self->is_csc) {
        free(self->nzval);
        free(self->rowind);
        free(self->colptr);
    } else {
        dynarray_destroy(self->Acoord);
    }
    free(self);
}

static void sparse_put(void* pself, int* vars, int n, double* local)
{
    csc_build_t* self = pself;
    int i, j;

    if (self->is_csc) {
        for (j = 0; j < n; ++j) {
            int j_var = vars[j];
            for (i = 0; i < n; ++i) {
                int i_var = vars[i];
                if (i_var < self->nactive && j_var < self->nactive) {
                    int k = self->colptr[j_var];
                    while (self->rowind[k] < i_var && k < self->colptr[j_var])
                         ++k;
                    assert(self->rowind[k] == i_var && k != self->colptr[j_var]);
                    self->nzval[k] += *local++;
                } else {
                    local++;
                }
            }
        }
    } else {
        for (j = 0; j < n; ++j) {
            for (i = 0; i < n; ++i) {
                coord_t coord;
                coord.i   = vars[i];
                coord.j   = vars[j];
                coord.Aij = *local++;
                if (coord.i < self->nactive && coord.j < self->nactive)
                    dynarray_append(self->Acoord, &coord);
            }
        }
    }
}

static int coord_compare(const void* pA, const void* pB)
{
    const coord_t* A = pA;
    const coord_t* B = pB;

    if      (A->j < B->j) return -1;
    else if (A->j > B->j) return  1;
    else if (A->i < B->i) return -1;
    else if (A->i > B->i) return  1;
    else                  return 0;
}

assemble_matrix_t* assembler_sparse_matrix(int n, int nactive)
{
    assemble_matrix_t* assembler = calloc(1, sizeof(*assembler));
    csc_build_t*       self      = calloc(1, sizeof(*self));
    assembler->data = self;

    self->n       = n;
    self->nactive = nactive;
    self->Acoord  = dynarray_create(sizeof(coord_t), 128);

    assembler->assemble = sparse_put;
    assembler->destroy  = sparse_destroy;

    return assembler;
}

int assembler_sparse_nnz(assemble_matrix_t* assembler)
{
    csc_build_t* self = assembler->data;
    return self->nnz;
}

double* assembler_sparse_nzval (assemble_matrix_t* assembler)
{
    csc_build_t* self = assembler->data;
    return self->nzval;
}

int* assembler_sparse_colptr(assemble_matrix_t* assembler)
{
    csc_build_t* self = assembler->data;
    return self->colptr;
}

int* assembler_sparse_rowind(assemble_matrix_t* assembler)
{
    csc_build_t* self = assembler->data;
    return self->rowind;
}

void assembler_sparse_csc(assemble_matrix_t* assembler)
{
    csc_build_t* self       = assembler->data;
    coord_t*     coord      = dynarray_data(self->Acoord);
    int          num_coord  = dynarray_count(self->Acoord);

    int i, j, count;

    if (self->is_csc) 
        return;
    
    assert(num_coord != 0);
    
    qsort(coord, num_coord, sizeof(coord_t), coord_compare);
    
    count = 1;
    for (i = 1; i < dynarray_count(self->Acoord); ++i)
         if (coord_compare(&(coord[i]), &(coord[i-1])) != 0)
             ++count;
    
    self->nzval  = calloc( count,             sizeof(double) );
    self->colptr = calloc( self->nactive + 1, sizeof(int)    );
    self->rowind = calloc( count,             sizeof(int)    );
    
    j = 0;
    for (i = 0; i < num_coord; ++i) {
        if (i != 0 && coord_compare(&(coord[i]), &(coord[i-1])) != 0)
            ++j;
        self->nzval [j] += coord[i].Aij;
        self->rowind[j]  = coord[i].i;
        self->colptr[ coord[i].j+1 ] = j + 1;
    }
    self->nnz = j+1;
    
    for (i = 1; i < self->nactive; ++i) {
        if (self->colptr[i] == 0) {
            self->colptr[i] = self->colptr[i-1];
        }
    }
    self->colptr[self->nactive] = self->nnz;

    self->is_csc = 1;
    dynarray_destroy(self->Acoord);
}

