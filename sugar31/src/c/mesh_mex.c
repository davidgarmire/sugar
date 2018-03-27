#include <sugar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <lua.h>
#include <lualib.h>

#include <modelmgr.h>
#include <model_test.h>
#include <model_matlab.h>
#include <model_mf2wrap.h>
#include <uses_manager.h>
#include <netout.h>
#include <netout_file.h>
#include <netout_mlab.h>

#include <mex_handle.h>
#include <sugarmex.h>
#include <mesh_mex.h>

#include <uses_manager_lua.h>
#include <meshgen_lua.h>

static mempool_t       pool;
static model_mgr_t     model_mgr;
static uses_manager_t  uses_mgr;
static int             mesh_handle_tag;

static char* path_via_matlab(void* data, char* namebuf, unsigned buflen)
{
    mxArray* prhs;
    mxArray* plhs;
    int status;
    char* path;

    prhs = mxCreateString(namebuf);
    status = mexCallMATLAB(1, &plhs, 1, &prhs, "which");
    mxDestroyArray(prhs);

    if (status != 0) {
        return NULL;
    }

    path = mx_to_string(plhs);
    if (*path != 0 && strlen(path) + 1 <= buflen) {
        strcpy(namebuf, path);
        mxFree(path);
        return namebuf;
    } else {
        mxFree(path);
        return NULL;
    }
}

static void matlab_mesh_error_handler(void* arg, const char* msg)
{
    mexErrMsgTxt(msg);
}

static void matlab_mesh_warning_handler(void* arg, const char* msg)
{
    mexPrintf("Warning: %s\n", msg);
}

static void lua_mesh_error_handler(void* arg, const char* msg)
{
    lua_State* L = (lua_State*) arg;
    lua_error(L, msg);
}

static int lua_alert(lua_State* L)
{
    int n = lua_gettop(L);  /* number of arguments */
    const char* s;

    lua_getglobal(L, "tostring");
    lua_pushvalue(L, 1);
    lua_rawcall(L, 1, 1);

    s = lua_tostring(L, -1);
    if (s != NULL)
        mexPrintf("%s", s);

    lua_settop(L, 0);
    return 0;
}

static void mex_addpath(int nlhs, mxArray* plhs[],
                        int nrhs, const mxArray* prhs[])
{
    int i;
    for (i = 0; i < nrhs; ++i) {
        char* name = mx_to_string(prhs[i]);
        uses_manager_addpath(uses_mgr, name);
        mxFree(name);
    }
}

static void mex_use(int nlhs, mxArray* plhs[],
                    int nrhs, const mxArray* prhs[])
{
    char        name[256];
    mesh_t      mesh;
    lua_State*  L;

    if (nrhs < 1 || !mxIsChar(prhs[0]))
        mexErrMsgTxt("Syntax: sugarmex('use', filename, [params])");

    mxGetString(prhs[0], name, sizeof(name));
    if (uses_manager_search(uses_mgr, name, sizeof(name)) == NULL) {
        mexPrintf("Could not find %s\n", name);
        mexErrMsgTxt("Error loading file");
    }
    
    L = lua_open(0);

    lua_usesmgrlib(L);
    lua_set_usesmgr(L, uses_mgr);
    lua_baselibopen(L);
    lua_iolibopen(L);
    lua_mathlibopen(L);
    lua_register(L, "_ALERT", lua_alert);

    if (nrhs >= 2) {
        if (mxIsStruct(prhs[1])) {
            const mxArray* params = prhs[1];
            int nfields = mxGetNumberOfFields(params);
            int ifield;
            
            for (ifield = 0; ifield < nfields; ++ifield) {
                const char* name = mxGetFieldNameByNumber(params, ifield);
                mxArray* field = mxGetFieldByNumber(params, 0, ifield);
                if (mxIsChar(field)) {
                    lua_pushstring(L, mx_to_string(field));
                } else if (mxIsDouble(field)) {
                    lua_pushnumber(L, *mxGetPr(field));
                } else {
                    lua_close(L);
                    mexErrMsgTxt("Only real scalars and strings allowed");
                }
                lua_setglobal(L, name);
            }
            
        } else {
            lua_close(L);
            mexErrMsgTxt("Parameters must be passed via structure");
        }
    }

    mesh = mesh_lua_load(L, uses_mgr, name, model_mgr, lua_mesh_error_handler);
    lua_close(L);

    if (mesh == NULL)
        mexErrMsgTxt("Error while interpreting net file");

    if (nlhs == 0) {
        mesh_destroy(mesh);
    } else {
        mesh_set_handlers(mesh, matlab_mesh_error_handler, 
                          matlab_mesh_warning_handler, NULL);
        plhs[0] = mx_from_double(mex_handle_add(mesh, mesh_handle_tag));
    }
}

static void mex_mesh_free(int nlhs, mxArray* plhs[],
                          int nrhs, const mxArray* prhs[])
{
    mex_check_num_args(nrhs, 1);
    mesh_destroy(mx_to_mesh(prhs[0]));
}

static void mex_printmesh(int nlhs, mxArray* plhs[],
                          int nrhs, const mxArray* prhs[])
{
    mex_check_num_args(nrhs, 1);
    netout_file(stdout, mx_to_mesh(prhs[0]));
}

static void mex_mesh_to_matlab(int nlhs, mxArray* plhs[],
                               int nrhs, const mxArray* prhs[])
{
    mex_check_num_args(nrhs, 1);
    plhs[0] = netout_mlab(mx_to_mesh(prhs[0]));
}

static void mex_num_elements(int nlhs, mxArray* plhs[],
                             int nrhs, const mxArray* prhs[])
{
    mex_check_num_args(nrhs, 1);
    plhs[0] = mx_from_double(mesh_num_elements(mx_to_mesh(prhs[0])));
}

static void mex_num_materials(int nlhs, mxArray* plhs[],
                              int nrhs, const mxArray* prhs[])
{
    mex_check_num_args(nrhs, 1);
    plhs[0] = mx_from_double(mesh_num_materials(mx_to_mesh(prhs[0])));
}

static void mex_num_nodes(int nlhs, mxArray* plhs[],
                          int nrhs, const mxArray* prhs[])
{
    mex_check_num_args(nrhs, 1);
    plhs[0] = mx_from_double(mesh_num_nodes(mx_to_mesh(prhs[0])));
}

static void mex_get_node_name(int nlhs, mxArray* plhs[],
                              int nrhs, const mxArray* prhs[])
{
    mesh_t       mesh;
    mesh_node_t* node;

    mex_check_num_args(nrhs, 1);
    mesh = mx_to_mesh(prhs[0]);
    node = mx_to_node(mesh, prhs[1]);

    plhs[0] = mxCreateString((node->name != NULL) ? node->name : "");
}

static void mex_get_node_pos(int nlhs, mxArray* plhs[],
                             int nrhs, const mxArray* prhs[])
{
    mesh_t       mesh;
    mesh_node_t* node;

    mex_check_num_args(nrhs, 1);
    mesh = mx_to_mesh(prhs[0]);
    node = mx_to_node(mesh, prhs[1]);

    plhs[0] = mxCreateDoubleMatrix(3,1,mxREAL);
    memcpy(mxGetPr(plhs[0]), node->x, 3 * sizeof(double));
}

static void mex_mesh_node_lookup(int nlhs, mxArray* plhs[],
                                 int nrhs, const mxArray* prhs[])
{
    mesh_t       mesh;
    mesh_node_t* node;
    const char*  name;
    int          nnodes;
    int          i;

    mex_check_num_args(nrhs, 2);
    mesh = mx_to_mesh(prhs[0]);
    name = mx_to_string(prhs[1]);
    nnodes = mesh_num_nodes(mesh);

    for (i = 1; i <= nnodes; ++i) {
        if (strcmp(name, mesh_node(mesh, i)->name) == 0) {
            plhs[0] = mx_from_double(i);
            return;
        }
    }

    plhs[0] = mx_from_nil();
}

static void mex_material_param(int nlhs, mxArray* plhs[],
                               int nrhs, const mxArray* prhs[])
{
    mesh_t        mesh;
    material_t*   material;
    char*         name;
    mesh_param_t* param;

    mex_check_num_args(nrhs, 3);
    mesh     = mx_to_mesh(prhs[0]);
    material = mx_to_material(mesh, prhs[1]);
    name     = mx_to_string(prhs[2]);

    if (material->model->param)
        param = (material->model->param)(material->data, name);

    mxFree(name);
    plhs[0] = mx_from_param(param);
}

static void mex_mesh_params(int nlhs, mxArray* plhs[],
                            int nrhs, const mxArray* prhs[])
{
    mesh_t mesh;
    int i, n;
    const char** names;

    mex_check_num_args(nrhs, 1);
    mesh = mx_to_mesh(prhs[0]);

    plhs[0] = mx_from_params(mesh);
}

static void mex_mesh_param(int nlhs, mxArray* plhs[],
                           int nrhs, const mxArray* prhs[])
{
    mesh_t mesh;

    mex_check_num_args(nrhs, 2);
    mesh = mx_to_mesh(prhs[0]);

    if (mxIsChar(prhs[1])) {
        char* name = mx_to_string(prhs[1]);
        plhs[0] = mx_from_param(mesh_param_byname(mesh, NULL, name));
    } else {
        int i = (int) mx_to_double(prhs[1]);
        plhs[0] = mx_from_param(mesh_param(mesh, i));
    }
}

static void mex_mesh_param_nodes(int nhls, mxArray* plhs[],
                                 int nrhs, const mxArray* prhs[])
{
    mesh_t mesh;
    int i, n;
    double* data;

    mex_check_num_args(nrhs, 1);    
    mesh = mx_to_mesh(prhs[0]);

    n = mesh_num_param_nodes(mesh);
    plhs[0] = mxCreateDoubleMatrix(n,1,mxREAL);

    data = mxGetPr(plhs[0]);
    for (i = 0; i < n; ++i)
        data[i] = mesh_param_node(mesh, i);
}

void mesh_mex_register()
{
    pool      = mempool_create(MEMPOOL_DEFAULT_SPAN);
    model_mgr = model_mgr_init();
    uses_mgr  = uses_manager_create(pool);

    uses_manager_envpath(uses_mgr, "SUGAR_LUA_PATH");
    uses_manager_addfunc(uses_mgr, path_via_matlab, NULL);

    model_test_register         (model_mgr);
    model_matlab_register       (model_mgr);
    model_mf2wrap_register      (model_mgr);
    model_mgr_standard_register (model_mgr);

    mesh_handle_tag = mex_handle_new_tag();
    mexmgr_add("addpath", mex_addpath);
    mexmgr_add("use", mex_use);
    mexmgr_add("mesh_free", mex_mesh_free);
    mexmgr_add("mesh_print", mex_printmesh);
    mexmgr_add("mesh_to_matlab", mex_mesh_to_matlab);
    mexmgr_add("mesh_nelements",  mex_num_elements );
    mexmgr_add("mesh_nmaterials", mex_num_materials);
    mexmgr_add("mesh_nnodes",     mex_num_nodes    );
    mexmgr_add("mesh_node_pos", mex_get_node_pos);
    mexmgr_add("mesh_node_name", mex_get_node_name);
    mexmgr_add("mesh_node_lookup", mex_mesh_node_lookup);
    mexmgr_add("mesh_material_param", mex_material_param);
    mexmgr_add("mesh_param",  mex_mesh_param );
    mexmgr_add("mesh_params", mex_mesh_params);
    mexmgr_add("mesh_param_nodes", mex_mesh_param_nodes);
}

void mesh_mex_shutdown()
{
    mempool_destroy   (pool);
    model_mgr_destroy (model_mgr);
}

int mesh_mex_handle_tag()
{
    return mesh_handle_tag;
}

mesh_t mx_to_mesh(const mxArray* array)
{
    return mx_to_handle(array, mesh_handle_tag, "Invalid mesh handle");
}

mesh_node_t* mx_to_node(mesh_t mesh, const mxArray* array)
{
    int node_id;
    mesh_node_t* node;

    if (!mxIsNumeric(array))
        mexErrMsgTxt("Invalid node identifier");

    node_id = (int) mx_to_double(array);
    node = mesh_node(mesh, node_id);
    if (node == NULL)
        mexErrMsgTxt("Invalid node identifier");

    return node;
}

material_t* mx_to_material(mesh_t mesh, const mxArray* array)
{
    int material_id;
    material_t* material;

    if (!mxIsNumeric(array))
        mexErrMsgTxt("Invalid material identifier");

    material_id = (int) mx_to_double(array);
    material = mesh_material(mesh, material_id);
    if (material == NULL)
        mexErrMsgTxt("Invalid node identifier");

    return material;
}

mxArray* mx_from_param(mesh_param_t* param)
{
    mxArray* value;

    if (param == NULL) {
        value = mxCreateDoubleMatrix(0, 0, mxREAL);
    } else if (param->tag == MESH_PARAM_NUMBER) {
        value = mx_from_double(param->val.d);
    } else if (param->tag == MESH_PARAM_STRING) {
        value = mxCreateString(param->val.s);
    } else if (param->tag == MESH_PARAM_MATRIX) {
        value = mxCreateDoubleMatrix(param->val.m.m, param->val.m.n, mxREAL);
        memcpy(mxGetPr(value), param->val.m.data,
               param->val.m.m * param->val.m.n * sizeof(double));
    } else
        mxAssert(0, "Invalid SUGAR parameter type");
    
    return value;
}

mxArray* mx_from_params(mesh_t mesh)
{
    int i, n;
    const char** names;
    mxArray* result;

    n     = mesh_num_params(mesh);
    names = mxCalloc(n, sizeof(char*));
    for (i = 0; i < n; ++i)
        names[i] = mesh_param(mesh, i)->name;

    result = mxCreateStructMatrix(1,1,n,names);
    mxFree(names);

    for (i = 0; i < n; ++i)
        mxSetFieldByNumber(result, 0, i, mx_from_param(mesh_param(mesh, i)));

    return result;
}

