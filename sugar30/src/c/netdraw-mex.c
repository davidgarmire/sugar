#include "mesh.h"
#include "netdraw-xt.h"
#include "netdraw-mlab.h"

#include "mex-handle.h"
#include "sugarmex.h"
#include "netdraw-mex.h"
#include "mesh-mex.h"

static int gc_handle_tag;

#ifdef HAS_X11
static void xt_draw(int nlhs, mxArray* plhs[],
                    int nrhs, const mxArray* prhs[])
{
    static int   argc = 1;
    static char* argv = "dummy";
    mesh_t mesh;
    double* x = NULL;

    mex_check_num_args(nrhs, 1);
    mesh = mx_to_mesh(prhs[0]);

    if (nrhs > 1) {
        int nactive = assemble_get_active(mesh_assembler(mesh));
        if (!mxIsNumeric(prhs[1]) || 
                mxGetM(prhs[1]) != nactive || mxGetN(prhs[1]) != 1)
            mexErrMsgTxt("Incorrect displacement vector");
        x = mxGetPr(prhs[1]);
    }

    xtnetdraw_init(argc, &argv);
    xtnetdraw(mesh, x);
}
#endif

static void mdraw(int nlhs, mxArray* plhs[],
                  int nrhs, const mxArray* prhs[])
{
    mesh_t  mesh;
    double* x = NULL;

    mex_check_num_args(nrhs, 1);
    mesh = mx_to_mesh(prhs[0]);

    if (nrhs > 1) {
        int nactive = assemble_get_active(mesh_assembler(mesh));
        if (!mxIsNumeric(prhs[1]) || 
                mxGetM(prhs[1]) != nactive || mxGetN(prhs[1]) != 1)
            mexErrMsgTxt("Incorrect displacement vector");
        x = mxGetPr(prhs[1]);
    }

    netdraw_mlab(mesh, x);
}

static void draw_beam(int nlhs, mxArray* plhs[],
                      int nrhs, const mxArray* prhs[])
{
    netdraw_gc_t* gc;
    double* transform;
    double l, w, h;
    int* vindex;
    int i;

    mex_check_num_args(nrhs, 6);
    gc = mx_to_gc(prhs[0]);
    l  = mx_to_double(prhs[2]);
    w  = mx_to_double(prhs[3]);
    h  = mx_to_double(prhs[4]);

    if (!mxIsNumeric(prhs[1]) || mxGetM(prhs[1]) != 3 || mxGetN(prhs[2]) != 4)
        mexErrMsgTxt("Invalid transform argument");

    if (!mxIsNumeric(prhs[2]) || mxGetM(prhs[5]) * mxGetN(prhs[5]) != 12)
        mexErrMsgTxt("Invalid variable index vector");

    transform = mxGetPr(prhs[1]);
    vindex = mxCalloc(12, sizeof(double));
    for (i = 0; i < 12; ++i)
        vindex[i] = (mxGetPr(prhs[5]))[i];

    netdraw_beam(gc, transform, l, w, h, vindex);

    mxFree(vindex);
}

netdraw_gc_t* mx_to_gc(const mxArray* array)
{
    return mx_to_handle(array, gc_handle_tag, "Invalid gc handle");
}

void gc_mex_register()
{
    gc_handle_tag = mex_handle_new_tag();
    #ifdef HAS_X11
    mexmgr_add("xt_draw", xt_draw);
    #endif
    mexmgr_add("mdraw", mdraw);
    mexmgr_add("draw_beam", draw_beam);
}

void gc_mex_shutdown()
{
}

int gc_mex_handle_tag()
{
    return gc_handle_tag;
}

