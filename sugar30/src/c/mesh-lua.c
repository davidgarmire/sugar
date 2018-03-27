#include <stdlib.h>
#include <string.h>

#include "meshgen-lua.h"
#include "mesh-lua.h"
#include "matrix-lua.h"
#include "superlu-lua.h"
#include "vars.h"
#include "assemble.h"
#include "netout.h"
#include "netout-file.h"
#include "newton.h"

static int lua_mesh_tag;

static int lua_mesh_print(lua_State* L)
{
    mesh_t mesh = lua_touserdata(L,-1);
    if (lua_gettop(L) != 1)
        lua_error(L, "Too many arguments");

    netout_file(stdout, mesh);
    lua_settop(L,0);
    return 0;
}

static int lua_mesh_vars(lua_State* L)
{
    mesh_t     mesh;
    vars_mgr_t vars;

    if (lua_gettop(L) != 1)
        lua_error(L, "Too many arguments");

    mesh = lua_touserdata(L,-1);
    vars = mesh_vars_mgr(mesh);
    vars_get_vartypes(vars);
    vars_assign(vars);

    lua_settop(L,0);
    return 0;
}

static int lua_mesh_bc(lua_State* L)
{
    if (lua_gettop(L) != 1)
        lua_error(L, "To many arguments");

    assemble_displace(mesh_assembler(lua_touserdata(L,-1)));

    lua_settop(L,0);
    return 0;
}

static int lua_mesh_R(lua_State* L)
{
    mesh_t      mesh      = lua_touserdata(L,-1);
    vars_mgr_t  vars      = mesh_vars_mgr(mesh);
    assembler_t assembler = mesh_assembler(mesh);
    int         nvars     = vars_count(vars);
    int         nactive   = assemble_get_active(assembler);

    lua_matrix_t dR;
    lua_matrix_t R;

    double* x;
    if (lua_gettop(L) > 2)
        lua_error(L, "Too many arguments");

    x = calloc(nvars, sizeof(double));
    memcpy(x, assemble_get_displacements(assembler), nvars * sizeof(double));
    if (lua_gettop(L) == 2) {
        lua_matrix_t xarg = lua_matrix_get(L,1);
        if (xarg->m > nvars || xarg->n > 1) {
            free(x);
            lua_error(L, "State vector too large");
        }
        memcpy(x, xarg->data, xarg->m * sizeof(double));
    }

    R  = lua_matrix_create(nactive, 1);
    dR = lua_matrix_create(nactive, nactive);

    R->m  = nactive;
    dR->m = nactive;
    dR->n = nactive;

    assemble_R   (assembler, x, NULL, NULL, R->data, nactive);
    assemble_dR  (assembler, 1, x, 0, NULL, 0, NULL, dR->data, nactive);

    free(x);
    lua_settop(L,0);
    lua_matrix_push(L,R);
    lua_matrix_push(L,dR);
    return 2;
}

static int lua_mesh_R_csc(lua_State* L)
{
#ifdef HAS_SUPERLU
    mesh_t mesh = lua_touserdata(L,-1);

    vars_mgr_t  vars      = mesh_vars_mgr(mesh);
    assembler_t assembler = mesh_assembler(mesh);
    int         nvars     = vars_count(vars);

    double*      x;
    lua_matrix_t R;
    superlu_t    dR;
    if (lua_gettop(L) > 2)
        lua_error(L, "Too many arguments");

    x = calloc(nvars, sizeof(double));
    memcpy(x, assemble_get_displacements(assembler), nvars * sizeof(double));
    if (lua_gettop(L) == 2) {
        lua_matrix_t xarg = lua_matrix_get(L,1);
        if (xarg->m > nvars || xarg->n > 1) {
            free(x);
            lua_error(L, "State vector too large");
        }
        memcpy(x, xarg->data, xarg->m * sizeof(double));
    }

    R = lua_matrix_create(nvars, 1);
    R->m = assemble_get_active(assembler);

    assemble_R(assembler, x, NULL, NULL, R->data, R->m);
    dR = assemble_dR_csc(assembler, 1, x, 0, NULL, 0, NULL, R->m);

    free(x);
    lua_settop(L, 0);
    lua_matrix_push(L, R);
    lua_superlu_push(L, dR);
    return 2;

#else

    lua_error(L, "SuperLU library not linked");
    lua_settop(L, 0);
    return 0;

#endif
}

static int lua_mesh_newton(lua_State* L)
{
#ifdef HAS_SUPERLU
    mesh_t mesh = lua_touserdata(L,-1);
    assembler_t assembler = mesh_assembler(mesh);

    vars_mgr_t  vars      = mesh_vars_mgr(mesh);
    int         nvars     = vars_count(vars);

    lua_matrix_t x   = lua_matrix_create(nvars, 1);
    lua_matrix_t R   = lua_matrix_create(nvars, 1);

    memcpy(x->data, assemble_get_displacements(assembler), 
           nvars * sizeof(double));
    x->m = assemble_get_active(assembler);
    R->m = assemble_get_active(assembler);

    newton(mesh, x->m, x->data, R->data, NULL,
           1e-4, 1e-10, 5);

    lua_settop(L, 0);
    lua_matrix_push(L, x);
    lua_matrix_push(L, R);
    return 2;

#else

    lua_error(L, "SuperLU library not linked");
    lua_settop(L, 0);
    return 0;

#endif
}

static int lua_mesh_draw(lua_State* L)
{
    mesh_t mesh = lua_touserdata(L,-1);

    const char* name = NULL;
    double*     x    = NULL;

    lua_pop(L,1);
    if (lua_gettop(L) > 0 && lua_isuserdata(L,-1)) {
        lua_matrix_t xarg = lua_matrix_get(L,-1);
        vars_mgr_t   vars_mgr  = mesh_vars_mgr(mesh);
        assembler_t  assembler = mesh_assembler(mesh);
        int          nvars     = vars_count(vars_mgr);
        int          nactive   = assemble_get_active(assembler);

        double*      displacements = assemble_get_displacements(assembler);

        if (xarg->m != nactive || xarg->n != 1)
            lua_error(L, "State vector is wrong size");

        x = calloc(nvars, sizeof(double));
        memcpy(x, displacements, nvars * sizeof(double));
        memcpy(x, xarg->data, nactive * sizeof(double));
         
        lua_pop(L,1);
    }

    if (lua_gettop(L) > 0 && lua_isstring(L,-1)) {
        name = lua_tostring(L,-1);
        lua_pop(L,1);
    }

    if (lua_gettop(L) > 0)
        lua_error(L, "Wrong number of parameters");


    if (name != NULL) {
        #ifdef HAS_XDR
            FILE* fp = fopen(name, "w");
            javawrite(fp, mesh, x, vars_count(mesh_vars_mgr(mesh)));
            fclose(fp);
        #else
            lua_error(L, "XDR libraries required for Java output\n");
        #endif
    } else {
        #ifdef HAS_X11
            xtnetdraw(mesh, x);
        #else
            lua_error(L, "X libraries not linked\n");
        #endif
    }

    if (x)
        free(x);

    lua_settop(L,0);
    return 0;
}

static int lua_mesh_free(lua_State* L)
{
    mesh_t mesh = lua_touserdata(L,-1);
    if (lua_gettop(L) != 1)
        lua_error(L, "Too many arguments");

    mesh_destroy(mesh);

    lua_settop(L,0);
    return 0;
}

static int lua_mesh_nvars(lua_State* L)
{
    int nvars = vars_count(mesh_vars_mgr(lua_touserdata(L,-1)));
    lua_settop(L,0);
    lua_pushnumber(L, nvars);
    return 1;
}

static int lua_mesh_nactive(lua_State* L)
{
    int nvars = assemble_get_active(mesh_assembler(lua_touserdata(L,-1)));
    lua_settop(L,0);
    lua_pushnumber(L, nvars);
    return 1;
}

static int lua_mesh_getmethod(lua_State* L)
{
    const char* name;

    if (!lua_isstring(L,2))
        lua_error(L, "Invalid index");
    name = lua_tostring(L,2);
    lua_pop(L,1);

    if (strcmp(name, "print") == 0)
        lua_pushcclosure(L, lua_mesh_print, 1);
    else if (strcmp(name, "vars") == 0)
        lua_pushcclosure(L, lua_mesh_vars, 1);
    else if (strcmp(name, "bc") == 0)
        lua_pushcclosure(L, lua_mesh_bc, 1);
    else if (strcmp(name, "R") == 0)
        lua_pushcclosure(L, lua_mesh_R, 1);
    else if (strcmp(name, "R_csc") == 0)
        lua_pushcclosure(L, lua_mesh_R_csc, 1);
    else if (strcmp(name, "newton") == 0)
        lua_pushcclosure(L, lua_mesh_newton, 1);
    else if (strcmp(name, "draw") == 0)
        lua_pushcclosure(L, lua_mesh_draw, 1);
    else if (strcmp(name, "free") == 0)
        lua_pushcclosure(L, lua_mesh_free, 1);
    else if (strcmp(name, "nvars") == 0)
        lua_mesh_nvars(L);
    else if (strcmp(name, "nactive") == 0)
        lua_mesh_nactive(L);
    else
        lua_error(L, "Invalid field or method name");

    return 1;
}

void lua_mesh_register(lua_State* L)
{
    lua_mesh_tag = lua_newtag(L);
    lua_pushcclosure(L, lua_mesh_getmethod, 0);
    lua_settagmethod(L, lua_mesh_tag, "gettable");
}

void lua_mesh_create(lua_State* L, mesh_t mesh)
{
    lua_pushusertag(L, mesh, lua_mesh_tag);
}

mesh_t lua_mesh_get(lua_State* L, int i)
{
    if (lua_gettop(L) < i)
        lua_error(L, "Index out of range");

    if (lua_tag(L,i) != lua_mesh_tag)
        lua_error(L, "Not a mesh!");

    return lua_touserdata(L,i);
}

