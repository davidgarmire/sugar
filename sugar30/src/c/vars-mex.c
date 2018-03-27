#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mex-handle.h"
#include "sugarmex.h"
#include "mesh-mex.h"
#include "vars-mex.h"
#include "assemble.h"

static int vars_handle_tag;

static void mex_vars_node(int nlhs, mxArray* plhs[],
                          int nrhs, const mxArray* prhs[])
{
    vars_mgr_t vars;
    int        node_id;
    char*      name;

    mex_check_num_args(nrhs, 3);
    vars    = mx_to_vars_mgr    (prhs[0]);
    node_id = (int) mx_to_double(prhs[1]);
    name    = mx_to_string      (prhs[2]);

    plhs[0] = mx_from_double(vars_node(vars, node_id, name));
}

static void mex_vars_branch(int nlhs, mxArray* plhs[],
                            int nrhs, const mxArray* prhs[])
{
    vars_mgr_t vars;
    char*      name;

    mex_check_num_args(nrhs, 2);
    vars    = mx_to_vars_mgr    (prhs[0]);
    name    = mx_to_string      (prhs[1]);

    plhs[0] = mx_from_double(vars_branch(vars, name));
}

static void mex_vars_assign(int nlhs, mxArray* plhs[],
                            int nrhs, const mxArray* prhs[])
{
    mesh_t     mesh;
    vars_mgr_t vars;

    mex_check_num_args(nrhs, 1);
    mesh = mx_to_mesh(prhs[0]);
    vars = mesh_vars_mgr(mesh);

    vars_get_vartypes(vars);
    vars_assign(vars);
}

static void mex_vars_lookup_node(int nlhs, mxArray* plhs[],
                                 int nrhs, const mxArray* prhs[])
{
    mesh_t      mesh;
    int         node_id;
    const char* name;
    vars_mgr_t  vars;

    mex_check_num_args(nrhs, 3);
    mesh    = mx_to_mesh(prhs[0]);
    node_id = (int) mx_to_double(prhs[1]);
    name    = mx_to_string(prhs[2]);
    vars    = mesh_vars_mgr(mesh);

    plhs[0] = mx_from_double(vars_lookup_node(vars, node_id, name) + 1);
}

static void mex_vars_lookup_branch(int nlhs, mxArray* plhs[],
                                   int nrhs, const mxArray* prhs[])
{
    mesh_t      mesh;
    int         elt_id;
    int         var_number;
    vars_mgr_t  vars;

    mex_check_num_args(nrhs, 3);
    mesh        = mx_to_mesh(prhs[0]);
    elt_id      = (int) mx_to_double(prhs[1]);
    var_number  = (int) mx_to_double(prhs[2]);
    vars        = mesh_vars_mgr(mesh);

    plhs[0] = mx_from_double(vars_lookup_branch(vars, elt_id, var_number) + 1);
}

static void mex_vars_lookup(int nlhs, mxArray* plhs[],
                            int nrhs, const mxArray* prhs[])
{
    mesh_t       mesh;
    vars_mgr_t   vars;
    vars_desc_t* v;
    int          nvars;
    int          i;

    const char* fieldnames[4] = {
        "type",
        "parent",
        "name",
        "offset"
    };

    mex_check_num_args(nrhs, 1);
    mesh        = mx_to_mesh(prhs[0]);
    vars        = mesh_vars_mgr(mesh);
    nvars       = vars_count(vars);

    v = mxCalloc(nvars, sizeof(vars_desc_t));
    vars_lookup(vars, v);

    plhs[0] = mxCreateStructMatrix(nvars, 1, 4, fieldnames);
    for (i = 0; i < nvars; ++i) {
        if (v[i].type == 'n') {
            mxSetFieldByNumber(plhs[0], i, 0, mxCreateString("node"));
            mxSetFieldByNumber(plhs[0], i, 1, mx_from_double(v[i].parent));
            mxSetFieldByNumber(plhs[0], i, 2, mxCreateString(v[i].name));
            mxSetFieldByNumber(plhs[0], i, 3, mx_from_nil   ());
        } else if (v[i].type == 'b') {
            mxSetFieldByNumber(plhs[0], i, 0, mxCreateString("branch"));
            mxSetFieldByNumber(plhs[0], i, 1, mx_from_double(v[i].parent));
            mxSetFieldByNumber(plhs[0], i, 2, mxCreateString(""));
            mxSetFieldByNumber(plhs[0], i, 3, mx_from_double(v[i].offset));
        } else {
            mexErrMsgTxt("Internal error in variable indexing");
        }
    }
    mxFree(v);
}

static void mex_vars_nvars(int nlhs, mxArray* plhs[],
                           int nrhs, const mxArray* prhs[])
{
    mesh_t     mesh;
    vars_mgr_t vars;
    int        nvars;

    mex_check_num_args(nrhs, 1);
    mesh  = mx_to_mesh(prhs[0]);
    vars  = mesh_vars_mgr(mesh);
    nvars = vars_count(vars);

    plhs[0] = mx_from_double(nvars);
}

static void mex_vars_nactive(int nlhs, mxArray* plhs[],
                             int nrhs, const mxArray* prhs[])
{
    mesh_t      mesh;
    assembler_t assembler;
    int         nactive;

    mex_check_num_args(nrhs, 1);
    mesh      = mx_to_mesh(prhs[0]);
    assembler = mesh_assembler(mesh);
    nactive   = assemble_get_active(assembler);

    plhs[0] = mx_from_double(nactive);
}

void vars_mex_register()
{
    vars_handle_tag = mex_handle_new_tag();
    mexmgr_add("vars_node", mex_vars_node);
    mexmgr_add("vars_branch", mex_vars_branch);
    mexmgr_add("vars_assign", mex_vars_assign);
    mexmgr_add("vars_lookup_node", mex_vars_lookup_node);
    mexmgr_add("vars_lookup_branch", mex_vars_lookup_branch);
    mexmgr_add("vars_lookup", mex_vars_lookup);
    mexmgr_add("vars_nvars", mex_vars_nvars);
    mexmgr_add("vars_nactive", mex_vars_nactive);
}

void vars_mex_shutdown()
{
}

int vars_mex_handle_tag()
{
    return vars_handle_tag;
}

vars_mgr_t mx_to_vars_mgr(const mxArray* array)
{
    return mx_to_handle(array, vars_handle_tag, "Invalid vars manager handle");
}

