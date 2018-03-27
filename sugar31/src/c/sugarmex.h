#ifndef SUGARMEX_H
#define SUGARMEX_H

#include <mex.h>

typedef void (*mex_fun_t)(int nlhs, mxArray* plhs[], 
                          int nrhs, const mxArray* prhs[]);

void mexmgr_add(const char* name, mex_fun_t fun);

void*    mx_to_handle  (const mxArray* array, int tag, const char* errmsg);
char*    mx_to_string  (const mxArray* array);
double   mx_to_double  (const mxArray* array);
mxArray* mx_from_double(double x);
mxArray* mx_from_nil   ();

void     mex_check_num_args(int nrhs, int expected);

#endif /* SUGARMEX_H */
