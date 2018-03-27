#ifndef ASSEMBLE_MEX_H
#define ASSEMBLE_MEX_H

#include <mex.h>
#include <assemble.h>

void assemble_mex_register();
int  assemble_mex_tag();

assemble_matrix_t* mx_to_assemble(const mxArray* array);

#endif /* ASSEMBLE_MEX_H */
