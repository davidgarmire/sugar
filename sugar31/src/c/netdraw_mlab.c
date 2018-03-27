#include <sugar.h>

#include <mex.h>

#include <mesh.h>
#include <vars.h>
#include <assemble.h>
#include <affine.h>

#include <sugarmex.h>
#include <netdraw_mlab.h>

static void draw_beam(void* pself, double* transform,
                              double l, double w, double h,
                              int* vindex)
{
    double* x = (double*) pself;
    double* xlocal;
    int i;
    mxArray* prhs[5];

    prhs[0] = mxCreateDoubleMatrix(3, 4, mxREAL);
    memcpy(mxGetPr(prhs[0]), transform, 12*sizeof(double));

    prhs[1] = mx_from_double(l);
    prhs[2] = mx_from_double(w);
    prhs[3] = mx_from_double(h);

    prhs[4] = mxCreateDoubleMatrix(12, 1, mxREAL);
    xlocal  = mxGetPr(prhs[4]);
    if (x) {
        for (i = 0; i < 12; ++i)
            xlocal[i] = x[vindex[i]];
        for (i = 0; i < 12; i += 3)
            affine_apply_AT(transform, xlocal + i);
    } else
        memset(xlocal, 0, 12*sizeof(double));

    mexCallMATLAB(0, NULL, 5, prhs, "cho_draw_mlab_beam");
}

static double* get_x(void* pself)
{
    return (double*) pself;
}

void netdraw_mlab(mesh_t mesh, double* x)
{
    netdraw_fun_t methods;
    netdraw_gc_t  gc;
    double*       xall;

    int          i, n;
    vars_mgr_t   vars_mgr  = mesh_vars_mgr(mesh);
    assembler_t  assembler = mesh_assembler(mesh);
    int          nvars     = vars_count(vars_mgr);
    int          nactive   = assemble_get_active(assembler);

    double*      displacements = assemble_get_displacements(assembler);

    memset(&methods, 0, sizeof(methods));
    methods.beam = draw_beam;
    methods.line = NULL;
    methods.plane = NULL;
    methods.get_x = get_x;

    gc.methods   = &methods;
    xall         = mxCalloc(nvars, sizeof(double));
    if (x) {
        memcpy(xall, displacements, nvars * sizeof(double));
        memcpy(xall, x, nactive * sizeof(double));
    }

    gc.data = xall;
    n = mesh_num_elements(mesh);
    for (i = 1; i <= n; ++i)
        element_display( mesh_element(mesh, i), &gc );

    mxFree(xall);
}

