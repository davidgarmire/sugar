#ifndef NETOUT_MEX_H
#define NETOUT_MEX_H

#include <mex.h>
#include <netout.h>

void netout_mex_register();
int  netout_mex_tag();

netout_t* mx_to_netout(const mxArray* array);

#endif /* NETOUT_MEX_H */
