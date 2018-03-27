#include "sugarmex.h"
#include "mex-handle.h"
#include "netout-mex.h"

static int netout_handle_tag;

static void netout(int nlhs, mxArray* plhs[],
                   int nrhs, const mxArray* prhs[])
{
    netout_t*   netout;
    const char* name;

    mex_check_num_args(nrhs, 3);
    netout = mx_to_netout(prhs[0]);
    name   = mx_to_string(prhs[1]);

    if (mxIsChar(prhs[2]))
        netout_string(netout, name, mx_to_string(prhs[2]));
    else if (mxIsNumeric(prhs[2]))
        netout_double_matrix(netout, name, mxGetPr(prhs[2]), 
                             mxGetM(prhs[2]), mxGetN(prhs[2]));
    else {
/*        mexErrMsgTxt("Cannot output a non-string and non-number type\n");*/
netout_string(netout, name, "***");
    }
}

void netout_mex_register()
{
    netout_handle_tag = mex_handle_new_tag();
    mexmgr_add("netout", netout);
}

int netout_mex_tag()
{
    return netout_handle_tag;
}

netout_t* mx_to_netout(const mxArray* array)
{
    return mx_to_handle(array, netout_handle_tag, "Invalid netout handle");
}

