#include <string.h>
#include <stdlib.h>

#include "meshgen-lua.h"
#include "assemble.h"
#include "vars.h"

static int lua_material_tag;

static int lua_meshgen_node(lua_State* L)
{
    const char* name = NULL;
    mesh_t mesh;
    double pos[3] = {0, 0, 0};
    int i, node_id;

    if (lua_gettop(L) != 2 || !lua_istable(L,1)) 
        lua_error(L, "Incorrect call to mesh routine");
    mesh = (mesh_t) lua_touserdata(L,-1);
    lua_pop(L,1);

    lua_pushstring(L, "name");
    lua_rawget(L,1);
    if (lua_isstring(L,-1))
        name = lua_tostring(L,-1);
    lua_pop(L,1);
    for (i = 0; i < 3; ++i) {
        lua_pushnumber(L, i+1);
        lua_rawget(L,1);
        if (lua_isnumber(L,-1))
            pos[i] = lua_tonumber(L,-1);
        lua_pop(L,1);
    }

    node_id = mesh_add_node(mesh, name, pos[0], pos[1], pos[2]);
    lua_settop(L, 0);
    lua_pushnumber(L, node_id);

    return 1;
}

static int lua_meshgen_node_pos(lua_State* L)
{
    mesh_t mesh;
    double pos[3] = {0, 0, 0};
    int i, node_id = 0;
    mesh_node_t* node;

    if (lua_gettop(L) != 2 || !lua_istable(L,1)) 
        lua_error(L, "Incorrect call to mesh routine");
    mesh = (mesh_t) lua_touserdata(L,-1);
    lua_pop(L,1);

    for (i = 0; i < 3; ++i) {
        lua_pushnumber(L, i+1);
        lua_rawget(L,1);
        if (lua_isnumber(L,-1))
            pos[i] = lua_tonumber(L,-1);
        lua_pop(L,1);
    }
    lua_pushstring(L, "mesh index");
    lua_rawget(L, -2);
    if (lua_isnumber(L,-1)) {
         node_id = lua_tonumber(L, -1);
    }
    if (node_id < 1 || node_id > mesh_num_nodes(mesh))
         lua_error(L, "Node index out of range");
    lua_pop(L, 1);

    node = mesh_node(mesh, node_id);
    memcpy(node->x, pos, 3*sizeof(double));

    lua_settop(L, 0);
    return 0;
}

static int lua_meshgen_material(lua_State* L)
{
    mesh_t mesh;
    mesh_param_t* model_param;
    const char* model_name = NULL;
    int material_id;

    if (lua_gettop(L) != 2 || !lua_istable(L,1)) 
        lua_error(L, "Incorrect call to mesh routine");
    mesh = (mesh_t) lua_touserdata(L,-1);
    lua_pop(L,1);

    mesh_params_clear(mesh);
    lua_pushnil(L);
    while (lua_next(L,1) != 0) {
        if (lua_isnumber(L,-2)) {
        } else if (lua_isstring(L,-2)) {
            const char* key = lua_tostring(L,-2);
            if (lua_isnumber(L,-1)) {
                mesh_add_param_number(mesh, key, lua_tonumber(L,-1));
            } else if (lua_istable(L,-1)) {

                /* TODO: Clean this up! */
                lua_pushstring(L, "mesh index");
                lua_rawget(L,-2);
                if (lua_isnumber(L,-1)) {

                    /* Get mesh index out of node, element, or material */
                    mesh_add_param_number(mesh, key, lua_tonumber(L,-1));
                    lua_pop(L,1);

                } else {

                    /* Turn a numeric array into a matrix object (1-by-n) */
                    int i, n;
                    double* data;

                    lua_pop(L,1);
                    n = lua_getn(L,-1);
                    data = calloc(n, sizeof(double));
                    for (i = 0; i < n; ++i) {
                        lua_rawgeti(L, -1, i+1);
                        data[i] = lua_tonumber(L,-1);
                        lua_pop(L,1);
                    }
                    mesh_add_param_matrix(mesh, key, 1, n, data);
                    free(data);
                }

            } else if (lua_isstring(L,-1)) {
                mesh_add_param_string(mesh, key, lua_tostring(L,-1));
            }
        }
        lua_pop(L,1);
    }

    model_param = mesh_param_byname(mesh, "model");
    if (model_param != NULL && model_param->tag == MESH_PARAM_STRING)
        model_name = model_param->val.s;

    material_id = mesh_add_material(mesh, model_name);

    lua_settop(L,0);
    lua_newtable(L);
    lua_settag(L, lua_material_tag);
    lua_pushstring(L, "mesh index");
    lua_pushnumber(L, material_id);
    lua_rawset(L,1);

    return 1;
}

static int lua_meshgen_element(lua_State* L)
{
    mesh_t mesh;
    mesh_param_t* model_param;
    const char* model_name = NULL;
    int element_id = 0;
    int i;

    if (lua_gettop(L) != 2 || !lua_istable(L,1)) 
        lua_error(L, "Incorrect call to mesh routine");
    mesh = (mesh_t) lua_touserdata(L,-1);
    lua_pop(L,1);

    mesh_params_clear(mesh);
    lua_pushnil(L);
    while (lua_next(L,1) != 0) {
        if (lua_isnumber(L,-2)) {
        } else if (lua_isstring(L,-2)) {
            const char* key = lua_tostring(L,-2);
            if (lua_isnumber(L,-1)) {
                mesh_add_param_number(mesh, key, lua_tonumber(L,-1));
            } else if (lua_istable(L,-1)) {

                /* TODO: Clean this up! */
                lua_pushstring(L, "mesh index");
                lua_rawget(L,-2);
                if (lua_isnumber(L,-1)) {

                    /* Get mesh index out of node, element, or material */
                    mesh_add_param_number(mesh, key, lua_tonumber(L,-1));
                    lua_pop(L,1);

                } else {

                    /* Turn a numeric array into a matrix object (1-by-n) */
                    int i, n;
                    double* data;

                    lua_pop(L,1);
                    n = lua_getn(L,-1);
                    data = calloc(n, sizeof(double));
                    for (i = 0; i < n; ++i) {
                        lua_rawgeti(L, -1, i+1);
                        data[i] = lua_tonumber(L,-1);
                        lua_pop(L,1);
                    }
                    mesh_add_param_matrix(mesh, key, 1, n, data);
                    free(data);
                }

            } else if (lua_isstring(L,-1)) {
                mesh_add_param_string(mesh, key, lua_tostring(L,-1));
            }
        }
        lua_pop(L,1);
    }

    i = 0;
    ++i;
    lua_pushnumber(L,i);
    lua_gettable(L,1);
    while (!lua_isnil(L,-1)) {
        if (lua_isnumber(L,-1)) {
            mesh_add_param_node(mesh, lua_tonumber(L,-1));
        } else if (lua_istable(L,-1)) {
            lua_pushstring(L, "mesh index");
            lua_gettable(L,-2);
            if (lua_isnumber(L,-1))
                 mesh_add_param_node(mesh, lua_tonumber(L,-1));
            lua_pop(L,1);
        }
        lua_pop(L,1);
        ++i;
        lua_pushnumber(L,i);
        lua_gettable(L,1);
    }
    lua_pop(L,1);

    model_param = mesh_param_byname(mesh, "model");
    if (model_param != NULL && model_param->tag == MESH_PARAM_STRING)
        model_name = model_param->val.s;

    element_id = mesh_add_element(mesh, model_name);
    lua_settop(L,0);
    lua_pushnumber(L, element_id);

    return 1;
}

static void postload(lua_State* L)
{
    int i, n;
    lua_getglobal(L, "_postload");

    n = lua_getn(L, -1);
    for (i = 0; i < n; ++i) {
        lua_rawgeti(L, -1, i+1);
        if (lua_isstring(L,-1))
            lua_dostring(L, lua_tostring(L,-1));
        lua_pop(L,1);
    }
    lua_pop(L,1);
}

void mesh_lua_register(lua_State* L, mesh_t mesh, 
                       const char* name, lua_CFunction fn)
{
    lua_pushstring  (L, name);
    lua_pushuserdata(L, mesh);
    lua_pushcclosure(L, fn, 1);
    lua_settable    (L,-3);
}

void mesh_lua_unregister(lua_State* L, const char* name)
{
    lua_pushstring(L, name);
    lua_pushnil   (L);
    lua_settable  (L,-3);
}

int lua_material_gettable(lua_State* L)
{
    mesh_t mesh;
    const char* name;
    int material_id;
    mesh_param_t* param;
    material_t* material;

    mesh = lua_touserdata(L,-1);
    lua_pop(L,1);

    if (!lua_isstring(L,2))
        lua_error(L, "Invalid index");
    name = lua_tostring(L,2);
    lua_pop(L,1);

    lua_pushstring(L, "mesh index");
    lua_rawget(L,1);

    material_id = lua_tonumber(L,-1);
    lua_pop(L,2);

    material = mesh_material(mesh, material_id);
    param = material_param(material, name);

    lua_settop(L,0);
    if (param == NULL) {
        lua_pushnil(L);
    } else if (param->tag == MESH_PARAM_NUMBER) {
        lua_pushnumber(L, param->val.d);
    } else if (param->tag == MESH_PARAM_STRING) {
        lua_pushstring(L, param->val.s);
    } else if (param->tag == MESH_PARAM_MATRIX) {
        int i;
        lua_newtable(L);
        for (i = 0; i < param->val.m.m * param->val.m.n; ++i) {
            lua_pushnumber(L, i+1);
            lua_pushnumber(L, param->val.m.data[i]);
            lua_settable(L,-3);
        }
    } else {
        lua_pushnil(L);
    }

    return 1;
}

void lua_meshgen_add_table(lua_State* L, mesh_t mesh)
{
    lua_material_tag = lua_newtag(L);
    mesh_lua_register(L, mesh, "node", lua_meshgen_node);
    lua_dostring(L, 
    "node = function(p) local id=%node(p); p[\"mesh index\"] = id; return p; end");
    mesh_lua_register(L, mesh, "node_position", lua_meshgen_node_pos);
    mesh_lua_register(L, mesh, "material", lua_meshgen_material);
    lua_pushuserdata(L, mesh);
    lua_pushcclosure(L, lua_material_gettable, 1);
    lua_settagmethod(L, lua_material_tag, "gettable");
    mesh_lua_register(L, mesh, "element", lua_meshgen_element);
    lua_dostring(L, 
    "element = function(p) local id=%element(p); p[\"mesh index\"] = id; return p; end");
    lua_dostring(L, 
                 "_postload = {n = 0};"
                 "function postload(s) "
                 "  _postload.n = _postload.n + 1;"
                 "  _postload[_postload.n] = s;"
                 "end");
}

void lua_meshgen_rm_table(lua_State* L)
{
    mesh_lua_unregister(L, "node");
    mesh_lua_unregister(L, "node_position");
    mesh_lua_unregister(L, "material");
    mesh_lua_unregister(L, "element");
    lua_dostring(L, "_postload = nil");
}

void lua_meshgen_register(lua_State* L, mesh_t mesh)
{
    lua_getglobals(L);
    lua_meshgen_add_table(L, mesh);
    lua_pop(L,1);
}

void lua_meshgen_unregister(lua_State* L)
{
    lua_getglobals(L);
    lua_meshgen_rm_table(L);
    lua_pop(L,1);
}

mesh_t mesh_lua_load(lua_State* L, 
                     uses_manager_t uses_manager, const char* name,
                     model_mgr_t model_mgr,
                     void (*error_handler)(void* arg, const char* msg))
{
    char buf[256];
    mesh_t mesh;
    int i, status;

    strncpy(buf, name, sizeof(buf));
    if (uses_manager != NULL &&
            uses_manager_search(uses_manager, buf, sizeof(buf)) == NULL)
        return NULL;

    mesh = mesh_create(model_mgr);
    mesh_set_handlers(mesh, error_handler, NULL, L);
    lua_meshgen_register(L, mesh);
    status = lua_dofile(L, buf);
    postload(L);
    lua_meshgen_unregister(L);

    if (status != 0) {
        mesh_destroy(mesh);
        return NULL;
    }

    vars_get_vartypes(mesh_vars_mgr(mesh));
    vars_assign      (mesh_vars_mgr(mesh));
    for (i = 1; i <= mesh_num_elements(mesh); ++i) {
        element_t* element = mesh_element(mesh, i);
        element_set_position(element, mesh);
    }
    assemble_displace(mesh_assembler(mesh));
    return mesh;
}

