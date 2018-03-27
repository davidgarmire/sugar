#ifndef NETDRAW_MEX_H
#define NETDRAW_MEX_H

#include <mex.h>
#include "netdraw.h"

void gc_mex_register();
void gc_mex_shutdown();
int  gc_mex_handle_tag();

netdraw_gc_t* mx_to_gc(const mxArray* array);

#endif /* NETDRAW_MEX_H */
