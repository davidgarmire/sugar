#ifndef MATRIX_LUA_H
#define MATRIX_LUA_H 

#include <lua.h>

struct lua_matrix_struct {
    int owns_data;  /* True if this matrix owns storage  */
    int m, n, ld;   /* Array size and leading dimension  */
    double* data;   /* Array data                        */
    int* ipiv;      /* Pivot array for factored matrices */
};

typedef struct lua_matrix_struct* lua_matrix_t;

void         lua_matrix_register(lua_State* L);
void         lua_matrix_push(lua_State* L, lua_matrix_t A);
lua_matrix_t lua_matrix_get(lua_State* L, int index);

lua_matrix_t lua_matrix_create(int m, int n);
lua_matrix_t lua_matrix_slice(lua_matrix_t A, int i1, int i2, int j1, int j2);
void         lua_matrix_destroy(lua_matrix_t self);

#endif /* MATRIX_LUA_H */
