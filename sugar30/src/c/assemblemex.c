#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mex-handle.h"
#include "sugarmex.h"
#include "mesh-mex.h"
#include "assemble-mex.h"

static int assemble_handle_tag;

static void mesh_nactive(int nlhs, mxArray* plhs[],
                         int nrhs, const mxArray* prhs[])
{
    mesh_t mesh;

    mex_check_num_args(nrhs, 1);
    mesh = mx_to_mesh(prhs[0]);

    plhs[0] = mx_from_double(assemble_get_active(mesh_assembler(mesh)));
}

static void mex_assemble_R(int nlhs, mxArray* plhs[],
                           int nrhs, const mxArray* prhs[])
{
    mesh_t      mesh;
    int         full_size;
    assembler_t assembler;
    double*     x = NULL;
    double*     v = NULL;
    double*     a = NULL;
    double*     R;
    int         Rsize;

    int nvars;
    int nactive;

    mex_check_num_args(nrhs, 2);
    mesh      = mx_to_mesh(prhs[0]);
    full_size = mx_to_double(prhs[1]);
    assembler = mesh_assembler(mesh);

    nvars   = vars_count(mesh_vars_mgr(mesh));
    nactive = assemble_get_active(assembler);

    Rsize   = (full_size ? nvars : nactive);

    x = mxMalloc(nvars * sizeof(double));
    memcpy(x, assemble_get_displacements(assembler), nvars * sizeof(double));
    if (nrhs > 2) {
        if (!mxIsNumeric(prhs[2]))
            mexErrMsgTxt("Argument must be a numeric vector");
        if (mxGetM(prhs[2]) != nactive || mxGetN(prhs[2]) != 1)
            mexErrMsgTxt("x vector is wrong size");
        memcpy(x, mxGetPr(prhs[2]), nactive * sizeof(double));
    }

    if (nrhs > 3) {
        v = mxCalloc(nvars, sizeof(double));
        if (!mxIsNumeric(prhs[3]))
            mexErrMsgTxt("Argument must be a numeric vector");
        if (mxGetM(prhs[3]) != nactive || mxGetN(prhs[3]) != 1)
            mexErrMsgTxt("v vector is wrong size");
        memcpy(v, mxGetPr(prhs[3]), nactive * sizeof(double));
    }

    if (nrhs > 4) {
        a = mxCalloc(nvars, sizeof(double));
        if (!mxIsNumeric(prhs[4]))
            mexErrMsgTxt("Argument must be a numeric vector");
        if (mxGetM(prhs[4]) != nactive || mxGetN(prhs[4]) != 1)
            mexErrMsgTxt("a vector is wrong size");
        memcpy(a, mxGetPr(prhs[4]), nactive * sizeof(double));
    }

    plhs[0] = mxCreateDoubleMatrix(Rsize, 1, mxREAL);
    R       = mxGetPr(plhs[0]);

    assemble_R(assembler, x, v, a, R, Rsize); 

    if (a) mxFree(a);
    if (v) mxFree(v);
    if (x) mxFree(x);
}

static void mex_assemble_dR(int nlhs, mxArray* plhs[],
                            int nrhs, const mxArray* prhs[])
{
    mesh_t      mesh;
    assembler_t assembler;
    double      c[3] = {1, 0, 0};
    double*     x = NULL;
    double*     v = NULL;
    double*     a = NULL;
    double*     dR;

    int nvars;
    int nactive;

    mex_check_num_args(nrhs, 2);
    mesh      = mx_to_mesh(prhs[0]);
    assembler = mesh_assembler(mesh);

    nvars   = vars_count(mesh_vars_mgr(mesh));
    nactive = assemble_get_active(assembler);

    if (!mxIsNumeric(prhs[1])) {
        mexErrMsgTxt("c argument must be a numeric vector");
    } else if (mxGetM(prhs[1]) * mxGetN(prhs[1]) > 3) {
        mexErrMsgTxt("c vector is wrong size");
    } else {
        memcpy(c, mxGetPr(prhs[1]), 
               mxGetM(prhs[1]) * mxGetN(prhs[1]) * sizeof(double));
    }

    x = mxMalloc(nvars * sizeof(double));
    memcpy(x, assemble_get_displacements(assembler), nvars * sizeof(double));
    if (nrhs > 2) {
        if (!mxIsNumeric(prhs[2]))
            mexErrMsgTxt("Argument must be a numeric vector");
        if (mxGetM(prhs[2]) != nactive || mxGetN(prhs[2]) != 1)
            mexErrMsgTxt("x vector is wrong size");
        memcpy(x, mxGetPr(prhs[2]), nactive * sizeof(double));
    }

    if (nrhs > 3) {
        v = mxCalloc(nvars, sizeof(double));
        if (!mxIsNumeric(prhs[3]))
            mexErrMsgTxt("Argument must be a numeric vector");
        if (mxGetM(prhs[3]) != nactive || mxGetN(prhs[3]) != 1)
            mexErrMsgTxt("v vector is wrong size");
        memcpy(v, mxGetPr(prhs[3]), nactive * sizeof(double));
    }

    if (nrhs > 4) {
        a = mxCalloc(nvars, sizeof(double));
        if (!mxIsNumeric(prhs[4]))
            mexErrMsgTxt("Argument must be a numeric vector");
        if (mxGetM(prhs[4]) != nactive || mxGetN(prhs[4]) != 1)
            mexErrMsgTxt("a vector is wrong size");
        memcpy(a, mxGetPr(prhs[4]), nactive * sizeof(double));
    }

    plhs[0] = mxCreateDoubleMatrix(nactive, nactive, mxREAL);
    dR      = mxGetPr(plhs[0]);

    assemble_dR(assembler, c[0], x, c[1], v, c[2], a, dR, nactive);

    if (a) mxFree(a);
    if (v) mxFree(v);
    if (x) mxFree(x);
}

static void mex_assemble_dR_sparse(int nlhs, mxArray* plhs[],
                                   int nrhs, const mxArray* prhs[])
{
    mesh_t      mesh;
    assembler_t assembler;
    double      c[3] = {1, 0, 0};
    double*     x = NULL;
    double*     v = NULL;
    double*     a = NULL;
    int*        pi;

    assemble_matrix_t* assembler_x = NULL;
    assemble_matrix_t* assembler_v = NULL;
    assemble_matrix_t* assembler_a = NULL;
    assemble_matrix_t* assembler_dR;

    int i, n;
    int nvars;
    int nactive;

    mex_check_num_args(nrhs, 1);
    mesh      = mx_to_mesh(prhs[0]);
    assembler = mesh_assembler(mesh);

    nvars   = vars_count(mesh_vars_mgr(mesh));
    nactive = assemble_get_active(assembler);

    if (!mxIsNumeric(prhs[1])) {
        mexErrMsgTxt("c argument must be a numeric vector");
    } else if (mxGetM(prhs[1]) * mxGetN(prhs[1]) > 3) {
        mexErrMsgTxt("c vector is wrong size");
    } else {
        memcpy(c, mxGetPr(prhs[1]), 
               mxGetM(prhs[1]) * mxGetN(prhs[1]) * sizeof(double));
    }

    x = mxMalloc(nvars * sizeof(double));
    memcpy(x, assemble_get_displacements(assembler), nvars * sizeof(double));
    if (nrhs > 2) {
        if (!mxIsNumeric(prhs[2]))
            mexErrMsgTxt("Argument must be a numeric vector");
        if (mxGetM(prhs[2]) != nactive || mxGetN(prhs[2]) != 1)
            mexErrMsgTxt("x vector is wrong size");
        memcpy(x, mxGetPr(prhs[2]), nactive * sizeof(double));
    }

    if (nrhs > 3) {
        v = mxCalloc(nvars, sizeof(double));
        if (!mxIsNumeric(prhs[3]))
            mexErrMsgTxt("Argument must be a numeric vector");
        if (mxGetM(prhs[3]) != nactive || mxGetN(prhs[3]) != 1)
            mexErrMsgTxt("v vector is wrong size");
        memcpy(v, mxGetPr(prhs[3]), nactive * sizeof(double));
    }

    if (nrhs > 4) {
        a = mxCalloc(nvars, sizeof(double));
        if (!mxIsNumeric(prhs[4]))
            mexErrMsgTxt("Argument must be a numeric vector");
        if (mxGetM(prhs[4]) != nactive || mxGetN(prhs[4]) != 1)
            mexErrMsgTxt("a vector is wrong size");
        memcpy(a, mxGetPr(prhs[4]), nactive * sizeof(double));
    }

    if (x) assembler_x = assembler_dense_getvector(x, nvars, nvars);
    if (v) assembler_v = assembler_dense_getvector(v, nvars, nvars);
    if (a) assembler_a = assembler_dense_getvector(a, nvars, nvars);
    assembler_dR = assembler_sparse_matrix(nactive, nactive);

    n = mesh_num_elements(mesh);
    for (i = 1; i <= n; ++i) {
        element_dR( mesh_element(mesh, i), assembler_dR, 
                    c[0], assembler_x,
                    c[1], assembler_v,
                    c[2], assembler_a);
    }

    assembler_sparse_csc(assembler_dR);

    n = assembler_sparse_nnz(assembler_dR);
    plhs[0] = mxCreateSparse(nactive, nactive, n, mxREAL);
    memcpy(mxGetPr(plhs[0]), assembler_sparse_nzval(assembler_dR), 
           n * sizeof(double));
    memcpy(mxGetIr(plhs[0]), assembler_sparse_rowind(assembler_dR), 
           n * sizeof(int));
    memcpy(mxGetJc(plhs[0]), assembler_sparse_colptr(assembler_dR), 
           (nactive+1) * sizeof(int));

    assembler_matrix_free(assembler_dR);
    if (assembler_a) assembler_matrix_free(assembler_a);
    if (assembler_v) assembler_matrix_free(assembler_v);
    if (assembler_x) assembler_matrix_free(assembler_x);

    if (a) mxFree(a);
    if (v) mxFree(v);
    if (x) mxFree(x);
}

static void mex_assemble_bc(int nlhs, mxArray* plhs[],
                            int nrhs, const mxArray* prhs[])
{
    mesh_t      mesh;
    assembler_t assembler;

    mex_check_num_args(nrhs, 1);
    mesh      = mx_to_mesh(prhs[0]);
    assembler = mesh_assembler(mesh);

    assemble_displace(assembler);
}

static void mex_add_displace(int nlhs, mxArray* plhs[],
                             int nrhs, const mxArray* prhs[])
{
    assemble_matrix_t* assembler;

    int     var_id;
    double  value;

    mex_check_num_args(nrhs, 3);
    assembler = mx_to_assemble(prhs[0]);
    var_id    = (int) mx_to_double(prhs[1]);
    value     = mx_to_double(prhs[2]);

    assemble_matrix_add1(assembler, var_id, value);
}

static void mex_add_R(int nlhs, mxArray* plhs[],
                      int nrhs, const mxArray* prhs[])
{
    assemble_matrix_t* assembler;

    int*    var_ids;
    double* value;
    int     num_vars;
    int     i;

    mex_check_num_args(nrhs, 3);
    assembler = mx_to_assemble(prhs[0]);

    if (!mxIsNumeric(prhs[1]) || !mxIsNumeric(prhs[2]))
        mexErrMsgTxt("Arguments must be numeric");

    num_vars  = mxGetM(prhs[1]) * mxGetN(prhs[1]);
    if (mxGetM(prhs[2]) != num_vars || mxGetN(prhs[2]) != 1)
        mexErrMsgTxt("Mismatch between number of arguments and Rlocal");

    value     = mxGetPr(prhs[2]);
    var_ids   = mxCalloc(num_vars, sizeof(int));
    for (i = 0; i < num_vars; ++i)
        var_ids[i] = (int) (mxGetPr(prhs[1])[i]);

    assemble_matrix_add(assembler, var_ids, num_vars, value);
    mxFree(var_ids);
}

static void mex_add_dR(int nlhs, mxArray* plhs[],
                       int nrhs, const mxArray* prhs[])
{
    assemble_matrix_t* assembler;

    int*    var_ids;
    double* value;
    int     num_vars;
    int     i;

    mex_check_num_args(nrhs, 3);
    assembler = mx_to_assemble(prhs[0]);

    if (!mxIsNumeric(prhs[1]) || !mxIsNumeric(prhs[2]))
        mexErrMsgTxt("Arguments must be numeric");

    num_vars  = mxGetM(prhs[1]) * mxGetN(prhs[1]);
    if (mxGetM(prhs[2]) != num_vars || mxGetN(prhs[2]) != num_vars)
        mexErrMsgTxt("Mismatch between number of arguments and dRlocal");

    value     = mxGetPr(prhs[2]);
    var_ids   = mxCalloc(num_vars, sizeof(int));
    for (i = 0; i < num_vars; ++i)
        var_ids[i] = (int) (mxGetPr(prhs[1])[i]);

    assemble_matrix_add(assembler, var_ids, num_vars, value);
    mxFree(var_ids);
}

static void mex_x_local(int nlhs, mxArray* plhs[],
                        int nrhs, const mxArray* prhs[])
{
    assemble_matrix_t* assembler;

    int*    var_ids;
    double* value;
    int     num_vars;
    int     i;

    mex_check_num_args(nrhs, 2);
    assembler = mx_to_assemble(prhs[0]);

    if (!mxIsNumeric(prhs[1]))
        mexErrMsgTxt("Arguments must be numeric");

    num_vars  = mxGetM(prhs[1]) * mxGetN(prhs[1]);
    plhs[0]   = mxCreateDoubleMatrix(mxGetM(prhs[1]) * mxGetN(prhs[1]), 1, 
                                     mxREAL);
    value     = mxGetPr(plhs[0]);

    var_ids   = mxCalloc(num_vars, sizeof(int));
    for (i = 0; i < num_vars; ++i)
        var_ids[i] = (int) (mxGetPr(prhs[1])[i]);

    assemble_matrix_add(assembler, var_ids, num_vars, value);
    mxFree(var_ids);
}

void assemble_mex_register()
{
    assemble_handle_tag = mex_handle_new_tag();
    mexmgr_add("mesh_nactive", mesh_nactive);
    mexmgr_add("assemble_R", mex_assemble_R);
    mexmgr_add("assemble_dR", mex_assemble_dR);
    mexmgr_add("assemble_dR_sparse", mex_assemble_dR_sparse);
    mexmgr_add("assemble_bc", mex_assemble_bc);
    mexmgr_add("add_displace", mex_add_displace);
    mexmgr_add("add_R", mex_add_R);
    mexmgr_add("add_dR", mex_add_dR);
    mexmgr_add("x_local", mex_x_local);
}

void assemble_mex_shutdown()
{
}

int assemble_mex_handle_tag()
{
    return assemble_handle_tag;
}

assemble_matrix_t* mx_to_assemble(const mxArray* array)
{
    return mx_to_handle(array, assemble_handle_tag, 
                        "Invalid assembler handle");
}

