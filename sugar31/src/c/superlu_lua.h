#ifndef SUPERLU_LUA_H
#define SUPERLU_LUA_H 

#include <lua.h>
#include <superlu.h>

void      lua_superlu_register(lua_State* L);
void      lua_superlu_push(lua_State* L, superlu_t matrix);
superlu_t lua_superlu_get(lua_State* L, int index);

#endif /* SUPERLU_LUA_H */
