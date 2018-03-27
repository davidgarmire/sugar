#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>
#include <luadebug.h>

#ifdef HAS_READLINE
#include <readline/readline.h>
#include <readline/history.h>
#endif /* HAS_READLINE */

#include "modelmgr.h"
#include "model-test.h"
#include "vars.h"
#include "assemble.h"
#include "uses-manager.h"

#include "uses-manager-lua.h"
#include "meshgen-lua.h"
#include "mesh-lua.h"
#include "matrix-lua.h"
#include "superlu-lua.h"

static uses_manager_t uses_manager;
static model_mgr_t    model_mgr;

static void error_handler(void* arg, const char* msg)
{
    lua_State* L = (lua_State*) arg;
    lua_error(L, msg);
}

static int lua_netload(lua_State* L)
{
    int n = lua_gettop(L);
    mesh_t mesh;

    if (n != 1 || !lua_isstring(L,-1))
        lua_error(L, "Invalid argument to 'cho_load'");

    mesh = mesh_lua_load(L, uses_manager, lua_tostring(L,-1),
                         model_mgr, error_handler);

    if (mesh == NULL)
        lua_error(L, "Error opening file");

    lua_settop(L,0);
    lua_mesh_create(L, mesh);
    return 1;
}

int main(int argc, char** argv)
{
    lua_State* L = lua_open(0);
    char* line;

    mempool_t pool = mempool_create(MEMPOOL_DEFAULT_SPAN);
    model_mgr    = model_mgr_init();
    uses_manager = uses_manager_create(pool);

#ifdef HAS_X11
    xtnetdraw_init(argc, argv);
#endif

    model_test_register(model_mgr);
    model_mgr_standard_register(model_mgr);

    lua_register(L, "cho_load", lua_netload);
    lua_matrix_register(L);
    lua_superlu_register(L);
    lua_mesh_register(L);
    lua_usesmgrlib(L);
    lua_set_usesmgr(L, uses_manager);
    lua_baselibopen(L);
    lua_mathlibopen(L);
    lua_iolibopen(L);
    uses_manager_envpath(uses_manager, "SUGAR_LUA_PATH");
    --argc, ++argv;
    while (argc != 0) {
        lua_dofile(L, *argv);
        --argc, ++argv;
    }
    #ifdef HAS_READLINE

    read_history(".sugar-lua");
    while ((line = readline("sugar> ")) != NULL) {
        lua_dostring(L, line);
        add_history(line);
        free(line);
    }
    write_history(".sugar-lua");
    printf("\n");

    #else /* ! HAS_READLINE */

    line = malloc(256);
    printf("sugar> ");
    while (fgets(line, 256, stdin)) {
        lua_dostring(L, line);
        printf("sugar> ");
    }
    printf("\n");
    free(line);

    #endif /* HAS_READLINE */


    lua_close(L);

    model_mgr_destroy(model_mgr);
    mempool_destroy(pool);

    return 0;
}


