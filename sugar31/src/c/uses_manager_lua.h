#ifndef USES_MANAGER_LUA_H
#define USES_MANAGER_LUA_H

#include <uses_manager.h>

void lua_usesmgrlib(lua_State* L);
void lua_set_usesmgr(lua_State* L, uses_manager_t uses_mgr);
uses_manager_t lua_get_usesmgr(lua_State* L);

#endif /* USES_MANAGER_LUA_H */
