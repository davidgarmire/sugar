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

typedef struct model_matlab_t {
    element_t   element;
    const char* model;
    mxArray*    data;
} model_matlab_t;

static element_t* init(mesh_t mesh, const char* name,
                       model_element_t* modelfunc)
{
    int i, n, mesh_id;
    mxArray* prhs[2];

    mempool_t       pool = mesh_pool(mesh);
    model_matlab_t* self = (model_matlab_t*)
        mempool_cget(pool, sizeof(struct model_matlab_t));

    self->element.data = self;
    self->element.model = modelfunc;

    if (name == NULL) {
        mesh_error(mesh, "Error: model function not specified");
    }

    self->model = name;
    mesh_id = mex_handle_add(mesh, mesh_mex_handle_tag());

    prhs[0] = mxCreateString("init");
    prhs[1] = mx_from_double(mesh_id);

    if (mexCallMATLAB(1, &(self->data), 2, prhs, name) != 0) {
        mexPrintf("Error return from call to %s\n", name);
        mesh_error(mesh, "Error calling model function");
    }
    mexMakeArrayPersistent(self->data);

    mxDestroyArray(prhs[1]);
    mxDestroyArray(prhs[0]);

    mex_handle_remove(mesh_id);
    return &(self->element);
}

static void set_position(void* pself, mesh_t mesh)
{
    mxArray* plhs[1];
    mxArray* prhs[3];
    model_matlab_t* self = (model_matlab_t*) pself;
    int mesh_id;

    mesh_id = mex_handle_add(mesh, mesh_mex_handle_tag());

    prhs[0] = mxCreateString("pos");
    prhs[1] = self->data;
    prhs[2] = mx_from_double(mesh_id);

    mexCallMATLAB(1, plhs, 3, prhs, self->model); 

    mxDestroyArray(prhs[2]);
    mxDestroyArray(prhs[0]);

    mex_handle_remove(mesh_id);

    if (mxGetN(plhs[0]) != 0) {
        mxDestroyArray(self->data);
        self->data = plhs[0];
        mexMakeArrayPersistent(self->data);
    }
}

static void destroy(void* pself)
{
    model_matlab_t* self = (model_matlab_t*) pself;
    mxDestroyArray(self->data);
}

static void vars(void* pself, vars_mgr_t vars)
{
    mxArray* plhs[1];
    mxArray* prhs[3];

    model_matlab_t* self = (model_matlab_t*) pself;
    int vars_id;

    vars_id = mex_handle_add(vars, vars_mex_handle_tag());

    prhs[0] = mxCreateString("vars");
    prhs[1] = self->data;
    prhs[2] = mx_from_double(vars_id);

    mexCallMATLAB(1, plhs, 3, prhs, self->model); 

    mxDestroyArray(prhs[2]);
    mxDestroyArray(prhs[0]);

    mex_handle_remove(vars_id);

    if (mxGetN(plhs[0]) != 0) {
        mxDestroyArray(self->data);
        self->data = plhs[0];
        mexMakeArrayPersistent(self->data);
    }
}

static void displace(void* pself, assemble_matrix_t* displace)
{
    mxArray* prhs[3];
    model_matlab_t* self = (model_matlab_t*) pself;

    int displace_id = mex_handle_add(displace, assemble_mex_handle_tag());

    prhs[0] = mxCreateString("displace");
    prhs[1] = self->data;
    prhs[2] = mx_from_double(displace_id);

    mexCallMATLAB(0, NULL, 3, prhs, self->model); 

    mxDestroyArray(prhs[2]);
    mxDestroyArray(prhs[0]);

    mex_handle_remove(displace_id);
}

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

    if (x) x_id = mex_handle_add(x, assemble_mex_handle_tag());
    if (v) v_id = mex_handle_add(v, assemble_mex_handle_tag());
    if (a) a_id = mex_handle_add(a, assemble_mex_handle_tag());
    prhs[3] = (x) ? mx_from_double(x_id) : mx_from_nil();
    prhs[4] = (v) ? mx_from_double(v_id) : mx_from_nil();
    prhs[5] = (a) ? mx_from_double(a_id) : mx_from_nil();

    mexCallMATLAB(0, NULL, 6, prhs, self->model); 

    mxDestroyArray(prhs[5]);
    mxDestroyArray(prhs[4]);
    mxDestroyArray(prhs[3]);
    mxDestroyArray(prhs[2]);
    mxDestroyArray(prhs[0]);

    if (a) mex_handle_remove(a_id);
    if (v) mex_handle_remove(v_id);
    if (x) mex_handle_remove(x_id);
    mex_handle_remove(R_id);
}

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

    if (x) x_id = mex_handle_add(x, assemble_mex_handle_tag());
    if (v) v_id = mex_handle_add(v, assemble_mex_handle_tag());
    if (a) a_id = mex_handle_add(a, assemble_mex_handle_tag());
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

    if (a) mex_handle_remove(a_id);
    if (v) mex_handle_remove(v_id);
    if (x) mex_handle_remove(x_id);
    mex_handle_remove(dR_id);
}

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

void model_matlab_register(model_mgr_t model_mgr)
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

    model_mgr_add_element(model_mgr, NULL, &element);
}

