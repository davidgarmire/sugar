#include <sugar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>

#include <uses_manager_lua.h>

static int use_lua_file(lua_State* L)
{
    uses_manager_t uses_manager = lua_get_usesmgr(L);
    int n = lua_gettop(L);
    int i;

    if (uses_manager == NULL)
        lua_error(L, "Uses manager not installed!\n");

    for (i = 1; i <= n; ++i) {
        char buf[256];
        if (!lua_isstring(L,i))
            lua_error(L, "Invalid argument to 'use'");
        strncpy(buf, lua_tostring(L,i), sizeof(buf));
        if (uses_manager_search(uses_manager, buf, sizeof(buf)) == NULL)
            lua_error(L, "Could not open file");
        if (lua_dofile(L, buf) != 0)
            lua_error(L, "Error opening file");
    }
    return 0;
}

static int addpath_lua(lua_State* L)
{
    uses_manager_t uses_manager = lua_get_usesmgr(L);
    int n = lua_gettop(L);
    int i;

    if (uses_manager == NULL)
        lua_error(L, "Uses manager not installed!\n");

    for (i = 1; i <= n; ++i) {
        if (!lua_isstring(L,i))
            lua_error(L, "Invalid argument to 'use'");
        uses_manager_addpath(uses_manager, lua_tostring(L,i));
    }
    return 0;
}

void lua_set_usesmgr(lua_State* L, uses_manager_t uses_mgr)
{
    lua_getregistry(L);
    lua_pushstring(L, "SUGAR uses manager");
    lua_pushuserdata(L, uses_mgr);
    lua_settable(L,-3);
    lua_pop(L,1);
}

uses_manager_t lua_get_usesmgr(lua_State* L)
{
    uses_manager_t uses_mgr;

    lua_getregistry(L);
    lua_pushstring(L, "SUGAR uses manager");
    lua_gettable(L,-2);
    if (!lua_isuserdata(L,-1))
        return NULL;
    uses_mgr = (uses_manager_t) lua_touserdata(L,-1);
    lua_pop(L,2);
    return uses_mgr;
}

void lua_usesmgrlib(lua_State* L)
{
    lua_register(L, "use", use_lua_file);
    lua_register(L, "addpath", addpath_lua);
}


