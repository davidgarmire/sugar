#ifndef MESH_LUA_H
#define MESH_LUA_H

#include <lua.h>
#include "mesh.h"
#include "netdraw-xt.h"
#include "netdraw-java.h"

void lua_mesh_register(lua_State* L);

void   lua_mesh_create(lua_State* L, mesh_t mesh);
mesh_t lua_mesh_get   (lua_State* L, int i);

#endif /* MESH_LUA_H */
