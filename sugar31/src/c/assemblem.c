#include <sugar.h>

#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <modelmgr.h>
#include <assemblem.h>

void assemble_matrix_add(assemble_matrix_t* matrix, int* vars, int n, 
                                 double* local)
{
    assert(matrix != NULL && matrix->assemble != NULL);
    matrix->assemble(matrix->data, vars, n, local);
}

void assemble_matrix_add1(assemble_matrix_t* matrix, int var,
                                  double local)
{
    assemble_matrix_add(matrix, &var, 1, &local);
}

void assembler_matrix_free(assemble_matrix_t* matrix)
{
    assert(matrix != NULL);
    if (matrix->destroy)
        matrix->destroy(matrix->data);
    free(matrix);
}

