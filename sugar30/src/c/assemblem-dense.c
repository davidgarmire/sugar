#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "modelmgr.h"
#include "assemblem-dense.h"

struct assemble_dense_t {
    double* data;
    int     n;
    int     ldA;
};

struct assemble_bc_t {
    double* data;
    char*   has_bc;
    int     n;
};

static void dense_vector_put(void* pself, int* vars, int n, double* local)
{
    struct assemble_dense_t* self = pself;
    int i;

    for (i = 0; i < n; ++i) {
        int i_var = vars[i];
        if (i_var < self->n) {
            self->data[i_var] += local[i];
        }
    }
}

static void dense_vector_get(void* pself, int* vars, int n,
                                     double* local)
{
    struct assemble_dense_t* self = pself;
    int i;

    for (i = 0; i < n; ++i) {
        int i_var = vars[i];
        if (i_var < self->n) {
            local[i] = self->data[i_var];
        }
    }
}

static void dense_matrix_put(void* pself, int* vars, int n, double* local)
{
    struct assemble_dense_t* self = pself;
    int i, j;

    for (j = 0; j < n; ++j) {
        for (i = 0; i < n; ++i) {
            int i_var = vars[i];
            int j_var = vars[j];
            if (i_var < self->n && j_var < self->n) {
                self->data[j_var*self->ldA + i_var] += local[j*n+i];
            }
        }
    }
}

static assemble_matrix_t* dense_create(double* data, int n, int ldA)
{
    assemble_matrix_t* assembler  = malloc(sizeof(*assembler));
    struct assemble_dense_t* self = malloc(sizeof(*self));
    assembler->data   = self;
    self->n           = n;
    self->ldA         = ldA;
    self->data        = data;
    assembler->destroy  = free; 
    return assembler;
}

static void bc_put(void* pself, int* vars, int n, double* local)
{
    struct assemble_bc_t* self = pself;
    int i;

    for (i = 0; i < n; ++i) {
        if (vars[i] < self->n) {
            self->has_bc[vars[i]] = 1;
            self->data  [vars[i]] = local[i];
        }
    }
}

assemble_matrix_t* assembler_dense_vector(double* data, int n, int ldA)
{
    assemble_matrix_t* assembler = dense_create(data, n, ldA);
    assembler->assemble = dense_vector_put;
    return assembler;
}

assemble_matrix_t* assembler_dense_getvector(double* data, int n, int ldA)
{
    assemble_matrix_t* assembler = dense_create(data, n, ldA);
    assembler->assemble = dense_vector_get;
    return assembler;
}

assemble_matrix_t* assembler_dense_matrix(double* data, int n, int ldA)
{
    assemble_matrix_t* assembler = dense_create(data, n, ldA);
    assembler->assemble = dense_matrix_put;
    return assembler;
}

assemble_matrix_t* assembler_bc(double* displacements, char* has_bc, int n)
{
    assemble_matrix_t* assembler = malloc(sizeof(*assembler));
    struct assemble_bc_t* self = malloc(sizeof(*self));
    assembler->data = self;
    self->data          = displacements;
    self->has_bc        = has_bc;
    self->n             = n;
    assembler->assemble = bc_put;
    assembler->destroy  = free;
    return assembler;
}

