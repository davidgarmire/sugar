#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "superlu-lua.h"
#include "matrix-lua.h"

static int lua_superlu_tag;


#ifdef HAS_SUPERLU
static int lua_matrix_print(lua_State* L)
{
    superlu_t A = lua_touserdata(L,-1);

    if (lua_gettop(L) != 1)
        lua_error(L, "Wrong number of arguments");

    superlu_print(A);

    lua_settop(L, 0);
    return 0;
}

static int lua_matrix_factor(lua_State* L)
{
    superlu_t A = lua_touserdata(L,-1);

    superlu_factor(A);

    lua_settop(L,0);
    lua_pushusertag(L, A, lua_superlu_tag);
    return 1;
}

static int lua_matrix_solve(lua_State* L)
{
    superlu_t A = lua_touserdata(L,-1);
    lua_matrix_t B;

    if (lua_gettop(L) != 2)
        lua_error(L, "Wrong number of arguments");

    B = lua_matrix_get(L,1);

    if (superlu_n(A) != B->m)
        lua_error(L, "Dimension mismatch");

    superlu_solve(A, B->data);

    lua_settop(L,0);
    lua_pushusertag(L, B, lua_superlu_tag);
    return 1;
}

static int lua_matrix_free(lua_State* L)
{
    superlu_t A = lua_touserdata(L,-1);

    if (lua_gettop(L) != 1)
        lua_error(L, "Too many arguments");

    superlu_destroy(A);

    lua_settop(L,0);
    return 0;
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
    else
        lua_error(L, "Invalid method name");

    return 1;
}

#endif

void lua_superlu_push(lua_State* L, superlu_t matrix)
{
    lua_pushusertag(L, matrix, lua_superlu_tag);
}

superlu_t lua_superlu_get(lua_State* L, int index)
{
    if (index > lua_gettop(L))
        lua_error(L, "Index out of range");

    if (lua_tag(L,index) != lua_superlu_tag)
        lua_error(L, "Variable is not a SuperLU matrix");

    return lua_touserdata(L,index);
}

void lua_superlu_register(lua_State* L)
{
    lua_superlu_tag = lua_newtag(L);
#ifdef HAS_SUPERLU
    lua_pushcclosure(L, lua_matrix_getmethod, 0);
    lua_settagmethod(L, lua_superlu_tag, "gettable");
#endif
}

