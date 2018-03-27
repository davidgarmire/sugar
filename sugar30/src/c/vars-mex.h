#ifndef VARS_MEX_H
#define VARS_MEX_H

#include "vars.h"

void vars_mex_register();
void vars_mex_shutdown();
int  vars_mex_handle_tag();

vars_mgr_t mx_to_vars_mgr(const mxArray* array);

#endif /* VARS_MEX_H */
