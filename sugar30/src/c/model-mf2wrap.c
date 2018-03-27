#line 27 "model-mf2wrap.nw"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <mex.h>

#include "modelmgr.h"
#include "mesh.h"
#include "vars.h"
#include "assemble.h"
#include "netdraw.h"
#include "netout.h"

#include "sugarmex.h"
#include "mesh-mex.h"
#include "vars-mex.h"
#include "assemble-mex.h"
#include "netdraw-mex.h"

#line 56 "model-mf2wrap.nw"
typedef struct model_matlab_t {
    element_t   element;
    const char* model;
    mxArray*    data;
} model_matlab_t;

#line 75 "model-mf2wrap.nw"
static element_t* init(mesh_t mesh, const char* name,
		       model_element_t* modelfunc)
{
    int i, n, mesh_id;
    mxArray* prhs[2];
    char** names;
    double* nodes;
    mesh_param_t* rotparam;

    mempool_t       pool = mesh_pool(mesh);
    model_matlab_t* self = (model_matlab_t*)
        mempool_cget(pool, sizeof(struct model_matlab_t));

    self->element.data = self;
    self->element.model = modelfunc;

    if (name == NULL) {
	mesh_error(mesh, "Error: model function not specified");
    }

    self->model = name;

    rotparam = mesh_param_byname(mesh, "rot");
    if (rotparam) {
        rotparam->val.m.m = 3;
        rotparam->val.m.n = 3;
    }

    n = mesh_num_param_nodes(mesh);
    nodes = mxCalloc(n, sizeof(double));
    for (i = 0; i < n; ++i)
        nodes[i] = mesh_param_node(mesh, i);
    mesh_add_param_matrix(mesh, "nodes", 1, n, nodes);

    prhs[0] = mxCreateString("init");
    prhs[1] = mx_from_params(mesh);

    if (mexCallMATLAB(1, &(self->data), 2, prhs, name) != 0) {
	mexPrintf("Error return from call to %s\n", name);
        mesh_error(mesh, "Error calling model function");
    }
    mexMakeArrayPersistent(self->data);

    mxDestroyArray(prhs[1]);
    mxDestroyArray(prhs[0]);
    mxFree(nodes);

    return &(self->element);
}

#line 128 "model-mf2wrap.nw"
static void set_position(void* pself, mesh_t mesh)
{
    model_matlab_t* self = (model_matlab_t*) pself;
    const char* fieldnames[2] = {"name", "pos"};

    mxArray* nodesmx   = mxGetField(self->data, 0, "nodes");
    int      num_nodes = mxGetM(nodesmx) * mxGetN(nodesmx);
    double*  node_ids  = mxGetPr(nodesmx);

    mxArray* old_pos = mxGetField(self->data, 0, "nodespos");
    mxArray* new_pos = mxCreateStructMatrix(num_nodes,1,2,fieldnames);
    mxArray* prhs[2];

    int i;

    for (i = 0; i < num_nodes; ++i) {
	mesh_node_t* node = mesh_node(mesh, node_ids[i]);
	mxArray* name = mxCreateString(node->name);
	mxArray* pos  = mxCreateDoubleMatrix(3,1,mxREAL);
	memcpy(mxGetPr(pos), node->x, 3*sizeof(double));
	mxSetFieldByNumber(new_pos, i, 0, name);
	mxSetFieldByNumber(new_pos, i, 1, pos);
    }

    mxSetField(self->data, 0, "nodespos", new_pos);
    mxDestroyArray(old_pos);

    prhs[0] = mxCreateString("postpos");
    prhs[1] = self->data;

    mexCallMATLAB(1, &(self->data), 2, prhs, self->model); 
    mexMakeArrayPersistent(self->data);

    mxDestroyArray(prhs[1]);
    mxDestroyArray(prhs[0]);
}

#line 168 "model-mf2wrap.nw"
static void destroy(void* pself)
{
    model_matlab_t* self = (model_matlab_t*) pself;
    mxDestroyArray(self->data);
}

#line 181 "model-mf2wrap.nw"
static void vars(void* pself, vars_mgr_t varsmgr)
{
    model_matlab_t* self = (model_matlab_t*) pself;

    int num_vars = 0;
    int num_bcs  = 0;
    double var_ids[64];   /* TODO: Hope you don't have many nodal vars */
    double bc_ids[64];    /* TODO: Hope you don't have many nodal vars */

    mxArray* nodesmx   = mxGetField(self->data, 0, "nodes");
    int      num_nodes = mxGetM(nodesmx) * mxGetN(nodesmx);
    double*  node_ids  = mxGetPr(nodesmx);

    mxArray* mf2vars = mxGetField(self->data, 0, "mf2vars");
    mxArray* nodal   = mxGetField(mf2vars, 0, "dynamic");
    mxArray* ground  = mxGetField(mf2vars, 0, "ground");
    mxArray* branch  = mxGetField(mf2vars, 0, "branch");

    mxArray* old_vars = mxGetField(self->data, 0, "vars");
    mxArray* old_bcs  = mxGetField(self->data, 0, "bcvars");
    mxArray* new_vars;
    mxArray* new_bcs;

    if (nodal) {
        int M = mxGetM(nodal);
        int i;

        for (i = 0; i < M; ++i) {
            mxArray* which_node;
            mxArray* vars;
            int node_num, node_id, N, j;

            mxAssert(mxIsCell(nodal), "Not a cell!\n");
            mxAssert(mxGetN(nodal) == 2, "Not two columns!\n");

            which_node = mxGetCell(nodal, i);
            mxAssert(mxIsNumeric(which_node), "Node not a number!\n");

            vars = mxGetCell(nodal, M+i);
            mxAssert(mxIsCell(vars), "Vars not a cell!\n");

            node_num = (int) mxGetScalar(which_node);
            mxAssert(node_num > 0 && node_num <= num_nodes,
                     "Node number out of range");

            node_id = (int) node_ids[node_num-1];
            N = mxGetN(vars);

            for (j = 0; j < N; ++j) {
                char name[128];
                mxGetString(mxGetCell(vars, j), name, sizeof(name));
                var_ids[num_vars++] = vars_node(varsmgr, node_id, name);
            }
        }
    }

    if (branch) {
        int N = mxGetN(branch);
        int j;

        for (j = 0; j < N; ++j) {
            char name[128];
            mxGetString(mxGetCell(branch, j), name, sizeof(name));
            var_ids[num_vars++] = vars_branch(varsmgr, name);
        }
    }

    if (ground) {
        int M, i;
        M = mxGetM(ground);

        for (i = 0; i < M; ++i) {
            mxArray* which_node;
            mxArray* vars;
            int node_num, node_id, N, j;

            mxAssert(mxIsCell(ground), "Not a cell!\n");
            mxAssert(mxGetN(ground) == 2, "Not two columns!\n");

            which_node = mxGetCell(ground, i);
            mxAssert(mxIsNumeric(which_node), "Node not a number!\n");

            vars = mxGetCell(ground, M+i);
            mxAssert(mxIsCell(vars), "Vars not a cell!\n");

            node_num = (int) mxGetScalar(which_node);
            mxAssert(node_num > 0 && node_num <= num_nodes,
                     "Node number out of range");

            node_id = (int) node_ids[node_num-1];
            N = mxGetN(vars);
 
            for (j = 0; j < N; ++j) {
                char name[128];
                mxGetString(mxGetCell(vars, j), name, sizeof(name));
		bc_ids[num_bcs++] = vars_node(varsmgr, node_id, name);
            }
        }
    }

    if (mxGetN(old_vars) * mxGetM(old_vars) == num_vars) {
	new_vars = old_vars;
    } else {
	new_vars = mxCreateDoubleMatrix(1,num_vars,mxREAL);
	mxSetField(self->data, 0, "vars", new_vars);
	mxDestroyArray(old_vars);
    }
    memcpy(mxGetPr(new_vars), var_ids, num_vars * sizeof(double));

    if (mxGetN(old_bcs) * mxGetM(old_bcs) == num_bcs) {
	new_bcs = old_bcs;
    } else {
	new_bcs = mxCreateDoubleMatrix(1,num_bcs,mxREAL);
	mxSetField(self->data, 0, "bcvars", new_bcs);
	mxDestroyArray(old_bcs);
    }
    memcpy(mxGetPr(new_bcs), bc_ids, num_bcs * sizeof(double));
}

#line 307 "model-mf2wrap.nw"
static void displace(void* pself, assemble_matrix_t* displace)
{
    model_matlab_t* self = (model_matlab_t*) pself;

    mxArray* bcmx = mxGetField(self->data, 0, "bcvars");
    if (bcmx) {
        int i;
	int n = mxGetN(bcmx) * mxGetM(bcmx);
	double* bc = mxGetPr(bcmx);

	for (i = 0; i < n; ++i)
	    assemble_matrix_add1(displace, (int) bc[i], 0);
    }
}

#line 327 "model-mf2wrap.nw"
static void add_R(void* pself, assemble_matrix_t* R, 
		  assemble_matrix_t* x,
		  assemble_matrix_t* v,
		  assemble_matrix_t* a)
{
    mxArray* prhs[6];
    model_matlab_t* self = (model_matlab_t*) pself;

    int R_id = mex_handle_add(R, assemble_mex_handle_tag());
    int x_id, v_id, a_id;

    prhs[0] = mxCreateString("R");
    prhs[1] = self->data;
    prhs[2] = mx_from_double(R_id);

    
#line 405 "model-mf2wrap.nw"
if (x) x_id = mex_handle_add(x, assemble_mex_handle_tag());
if (v) v_id = mex_handle_add(v, assemble_mex_handle_tag());
if (a) a_id = mex_handle_add(a, assemble_mex_handle_tag());
#line 343 "model-mf2wrap.nw"
    prhs[3] = (x) ? mx_from_double(x_id) : mx_from_nil();
    prhs[4] = (v) ? mx_from_double(v_id) : mx_from_nil();
    prhs[5] = (a) ? mx_from_double(a_id) : mx_from_nil();

    mexCallMATLAB(0, NULL, 6, prhs, self->model); 

    mxDestroyArray(prhs[5]);
    mxDestroyArray(prhs[4]);
    mxDestroyArray(prhs[3]);
    mxDestroyArray(prhs[2]);
    mxDestroyArray(prhs[0]);

    
#line 411 "model-mf2wrap.nw"
if (a) mex_handle_remove(a_id);
if (v) mex_handle_remove(v_id);
if (x) mex_handle_remove(x_id);
#line 356 "model-mf2wrap.nw"
    mex_handle_remove(R_id);
}

#line 362 "model-mf2wrap.nw"
static void add_dR(void* pself, assemble_matrix_t* dR,
		   double cx, assemble_matrix_t* x,
		   double cv, assemble_matrix_t* v,
		   double ca, assemble_matrix_t* a)
{
    mxArray* prhs[7];
    model_matlab_t* self = (model_matlab_t*) pself;

    int dR_id = mex_handle_add(dR, assemble_mex_handle_tag());
    int x_id, v_id, a_id;
    double* c;

    prhs[0] = mxCreateString("dR");
    prhs[1] = self->data;
    prhs[2] = mx_from_double(dR_id);

    prhs[3] = mxCreateDoubleMatrix(1, 3, mxREAL);
    c = mxGetPr(prhs[3]);
    c[0] = cx;
    c[1] = cv;
    c[2] = ca;

    
#line 405 "model-mf2wrap.nw"
if (x) x_id = mex_handle_add(x, assemble_mex_handle_tag());
if (v) v_id = mex_handle_add(v, assemble_mex_handle_tag());
if (a) a_id = mex_handle_add(a, assemble_mex_handle_tag());
#line 385 "model-mf2wrap.nw"
    prhs[4] = (x) ? mx_from_double(x_id) : mx_from_nil();
    prhs[5] = (v) ? mx_from_double(v_id) : mx_from_nil();
    prhs[6] = (a) ? mx_from_double(a_id) : mx_from_nil();

    mexCallMATLAB(0, NULL, 7, prhs, self->model); 

    mxDestroyArray(prhs[6]);
    mxDestroyArray(prhs[5]);
    mxDestroyArray(prhs[4]);
    mxDestroyArray(prhs[3]);
    mxDestroyArray(prhs[2]);
    mxDestroyArray(prhs[0]);

    
#line 411 "model-mf2wrap.nw"
if (a) mex_handle_remove(a_id);
if (v) mex_handle_remove(v_id);
if (x) mex_handle_remove(x_id);
#line 399 "model-mf2wrap.nw"
    mex_handle_remove(dR_id);
}

#line 422 "model-mf2wrap.nw"
static void draw(void* pself, netdraw_gc_t* gc)
{
    mxArray* prhs[4];
    model_matlab_t* self = (model_matlab_t*) pself;
    int gc_id;
    int x_id;

    double* x = NULL;
    assemble_matrix_t* xassemble = NULL;

    x = netdraw_get_x(gc);

    if (x) {
	xassemble = /* FIXME */
	    assembler_dense_getvector(x, INT_MAX, INT_MAX);
    }

    gc_id = mex_handle_add(gc, gc_mex_handle_tag());
    x_id  = mex_handle_add(xassemble, assemble_mex_handle_tag());

    prhs[0] = mxCreateString("draw");
    prhs[1] = self->data;
    prhs[2] = mx_from_double(gc_id);
    prhs[3] = (x ? mx_from_double(x_id) : mx_from_nil());

    mexCallMATLAB(0, NULL, 4, prhs, self->model); 

    mxDestroyArray(prhs[3]);
    mxDestroyArray(prhs[2]);
    mxDestroyArray(prhs[0]);

    mex_handle_remove(x_id);
    mex_handle_remove(gc_id);

    if (x) {
	assembler_matrix_free(xassemble);
    }
}

#line 473 "model-mf2wrap.nw"
static void output(void* pself, netout_t* netout)
{
    mxArray* prhs[3];
    model_matlab_t* self = (model_matlab_t*) pself;
    int handle_id = mex_handle_add(netout, netout_mex_tag());

    prhs[0] = mxCreateString("output");
    prhs[1] = self->data;
    prhs[2] = mx_from_double(handle_id);

    netout_string(netout, "model", self->model);
    mexCallMATLAB(0, NULL, 3, prhs, self->model); 

    mxDestroyArray(prhs[2]);
    mxDestroyArray(prhs[0]);

    mex_handle_remove(handle_id);
}

#line 498 "model-mf2wrap.nw"
void model_mf2wrap_register(model_mgr_t model_mgr)
{
    model_element_t  element;

    memset(&element,  0, sizeof(element));

    element.init         = init;
    element.destroy      = destroy;
    element.set_position = set_position;
    element.vars         = vars;
    element.displace     = displace;
    element.R            = add_R;
    element.dR           = add_dR;
    element.display      = draw;
    element.output       = output; 

    model_mgr_add_element(model_mgr, "mf2wrap", &element);
}

