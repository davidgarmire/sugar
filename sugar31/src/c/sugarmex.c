#include <sugar.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dynarray.h>
#include <sugarmex.h>
#include <mex_handle.h>
#include <mesh_mex.h>
#include <vars_mex.h>
#include <assemble_mex.h>
#include <netdraw_mex.h>
#include <netout_mex.h>

typedef struct mex_entry_t {
    const char*  name;
    mex_fun_t    fun;
} mex_entry_t;

static dynarray_t entries;

static void mex_help(int nlhs, mxArray* plhs[],
                     int nrhs, const mxArray* prhs[])
{
    mexPrintf("Help!\n");
}

static mex_fun_t mexmgr_find(const char* name)
{
    int i;
    mex_entry_t* data;
    int          n;

    data = dynarray_data (entries);
    n    = dynarray_count(entries);

    for (i = 0; i < n; ++i)
        if (strcmp(data[i].name, name) == 0)
            return data[i].fun;

    return NULL;
}

static void mex_initialize()
{
    mexLock();
    entries = dynarray_create(sizeof(struct mex_entry_t), 32);
    mexmgr_add("help", mex_help);
    mex_handle_create();
    mesh_mex_register();
    vars_mex_register();
    assemble_mex_register();
    netout_mex_register();
    gc_mex_register();
}

static void mex_shutdown()
{
    mex_handle_destroy();
    mesh_mex_shutdown();
    vars_mex_shutdown();
    gc_mex_shutdown();
    dynarray_destroy(entries);
}

void* mx_to_handle(const mxArray* array, int tag, const char* errmsg)
{
    int handle_id;

    if (!mxIsNumeric(array) || mxGetM(array) != 1 || mxGetN(array) != 1)
        mexErrMsgTxt(errmsg);

    handle_id = (int) mx_to_double(array);
    if (mex_handle_tag(handle_id) != tag)
        mexErrMsgTxt(errmsg);

    return mex_handle_data(handle_id);
}

char* mx_to_string(const mxArray* array)
{
    if (!mxIsChar(array)) {
        mexErrMsgTxt("Value should be a string");
    } else {
        int namelen = mxGetN(array)+1;
        char* name  = (char*) mxCalloc(namelen, sizeof(char));
        mxGetString(array, name, namelen);
        return name;
    }
    return NULL;
}

double mx_to_double(const mxArray* array)
{
    if (!mxIsNumeric(array))
        mexErrMsgTxt("Value should be numeric");

    if (mxGetN(array) != 1)
        mexErrMsgTxt("Value should be a scalar");

    return *mxGetPr(array);
}

mxArray* mx_from_double(double x)
{
    mxArray* array  = mxCreateDoubleMatrix(1, 1, mxREAL);
    *mxGetPr(array) = x;
    return array;
}

mxArray* mx_from_nil()
{
    return mxCreateDoubleMatrix(0, 0, mxREAL);
}

void mex_check_num_args(int nrhs, int expected)
{
    if (nrhs < expected)
        mxErrMsgTxt("Not enough arguments");
}

void mexmgr_add(const char* name, mex_fun_t fun)
{
    mex_entry_t new_entry;
    new_entry.name = name;
    new_entry.fun  = fun;
    dynarray_append(entries, &new_entry);
}

void mexFunction(int nlhs, mxArray* plhs[],
                 int nrhs, const mxArray* prhs[])
{
    int          namelen;
    char*        name;
    mex_fun_t    fun;
    static int   first_call = 1;

    if (first_call) {
        first_call = 0;
        mex_initialize();
        mexAtExit(mex_shutdown);
    }
            
    if (nrhs == 0 || !mxIsChar(prhs[0])) {
        mexPrintf(SUGAR_MESSAGE);
        return;
    }

    name = mx_to_string(prhs[0]);

    fun = mexmgr_find(name);
    
    if (fun == NULL) {
        mexPrintf("Unknown function option: %s\n", name);
        mexErrMsgTxt("Error calling SUGAR MEX file.");
    }
    
    fun(nlhs, plhs, nrhs-1, prhs+1);

    mxFree(name);
}

