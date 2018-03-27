#ifndef MESHGEN_LUA_H
#define MESHGEN_LUA_H

#include <lua.h>
#include "uses-manager.h"
#include "mesh.h"

void lua_meshgen_add_table(lua_State* L, mesh_t mesh);
void lua_meshgen_rm_table (lua_State* L);

void lua_meshgen_register  (lua_State* L, mesh_t mesh);
void lua_meshgen_unregister(lua_State* L);

void mesh_lua_register(lua_State* L, mesh_t mesh, 
                       const char* name, lua_CFunction fn);
void mesh_lua_unregister(lua_State* L, const char* name);

mesh_t mesh_lua_load(lua_State* L, 
                     uses_manager_t uses_manager, const char* name,
                     model_mgr_t model_mgr,
                     void (*error_handler)(void* arg, const char* msg));

#endif /* MESHGEN_LUA_H */
