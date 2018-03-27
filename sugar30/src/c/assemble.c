#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "modelmgr.h"
#include "assemble.h"

struct assembler_struct {
    mesh_t     mesh;
    vars_mgr_t vars_mgr;
    int        via_pool;
    int        num_vars;
    int        num_active;
    double*    displacements;
};

assembler_t assemble_create(mesh_t mesh)
{
    assembler_t self = calloc(1, sizeof(*self));
    self->mesh       = mesh;
    self->vars_mgr   = mesh_vars_mgr(mesh);
    self->via_pool   = 0;
    return self;
}

assembler_t assemble_pcreate(mempool_t pool, mesh_t mesh)
{
    assembler_t self = (assembler_t) mempool_cget(pool, sizeof(*self));
    self->mesh       = mesh;
    self->vars_mgr   = mesh_vars_mgr(mesh);
    self->via_pool   = 1;
    return self;
}

void assemble_destroy(assembler_t self)
{
    if (self->displacements) free(self->displacements);

    if (!self->via_pool) {
        free(self);
    }
}

void assemble_displace(assembler_t self)
{
    int i, n, active_id, inactive_id;
    assemble_matrix_t* bc_assembler;
    int*  permutation;
    char* has_bc;

    if (self->displacements) free(self->displacements);

    self->num_vars      = vars_count(self->vars_mgr);
    self->displacements = calloc(self->num_vars, sizeof(double));
    permutation         = calloc(self->num_vars, sizeof(int));
    has_bc              = calloc(self->num_vars, 1);
    bc_assembler = assembler_bc(self->displacements, has_bc, self->num_vars);

    n = mesh_num_elements(self->mesh);
    for (i = 1; i <= n; ++i)
        element_displace( mesh_element(self->mesh, i), bc_assembler );

    self->num_active = 0;
    for (i = 0; i < self->num_vars; ++i)
        if (!has_bc[i])
            self->num_active++;
    active_id   = 0;
    inactive_id = self->num_active;
    for (i = 0; i < self->num_vars; ++i) {
        if (has_bc[i])
            permutation[i] = inactive_id++;
        else
            permutation[i] = active_id++;
    }
    for (i = self->num_vars - 1; i >= 0; --i) {
        if (permutation[i] >= self->num_active) {
            self->displacements[permutation[i]] = self->displacements[i];
        }
    }
    memset(self->displacements, 0, self->num_active * sizeof(double));

    vars_permute(self->vars_mgr, permutation);
    assembler_matrix_free(bc_assembler);
    free(permutation);
    free(has_bc);
}

double* assemble_get_displacements(assembler_t self)
{
    return self->displacements;
}

int assemble_get_active(assembler_t self)
{
    return self->num_active;
}

void assemble_R(assembler_t self, double* x, double* v, double* a, double* R,
                int Rsize)
{
    int i, n;

    assemble_matrix_t* assembler_x;
    assemble_matrix_t* assembler_v;
    assemble_matrix_t* assembler_a;
    assemble_matrix_t* assembler_R;

    assembler_x = (x == NULL) ? NULL : 
        assembler_dense_getvector(x, self->num_vars, self->num_vars);

    assembler_v = (v == NULL) ? NULL : 
        assembler_dense_getvector(v, self->num_vars, self->num_vars);

    assembler_a = (a == NULL) ? NULL : 
        assembler_dense_getvector(a, self->num_vars, self->num_vars);

    assembler_R = 
        assembler_dense_vector(R, Rsize, Rsize);

    memset(R, 0, self->num_active * sizeof(double));
    n = mesh_num_elements(self->mesh);
    for (i = 1; i <= n; ++i)
        element_R( mesh_element(self->mesh, i), assembler_R, 
                   assembler_x, assembler_v, assembler_a );

    assembler_matrix_free(assembler_R);
    if (assembler_a) assembler_matrix_free(assembler_a);
    if (assembler_v) assembler_matrix_free(assembler_v);
    if (assembler_x) assembler_matrix_free(assembler_x);
}

void assemble_dR(assembler_t self, 
                 double cx, double* x, 
                 double cv, double* v, 
                 double ca, double* a, 
                 double* dR,
                 int dRsize)
{
    int i, n;

    assemble_matrix_t* assembler_x;
    assemble_matrix_t* assembler_v;
    assemble_matrix_t* assembler_a;
    assemble_matrix_t* assembler_dR;

    assembler_x = (x == NULL) ? NULL : 
        assembler_dense_getvector(x, self->num_vars, self->num_vars);

    assembler_v = (v == NULL) ? NULL : 
        assembler_dense_getvector(v, self->num_vars, self->num_vars);

    assembler_a = (a == NULL) ? NULL : 
        assembler_dense_getvector(a, self->num_vars, self->num_vars);

    assembler_dR = 
        assembler_dense_matrix(dR, dRsize, dRsize);

    memset(dR, 0, self->num_active * self->num_active * sizeof(double));
    n = mesh_num_elements(self->mesh);
    for (i = 1; i <= n; ++i)
        element_dR( mesh_element(self->mesh, i), assembler_dR, 
                    cx, assembler_x,
                    cv, assembler_v,
                    ca, assembler_a );

    assembler_matrix_free(assembler_dR);
    if (assembler_a) assembler_matrix_free(assembler_a);
    if (assembler_v) assembler_matrix_free(assembler_v);
    if (assembler_x) assembler_matrix_free(assembler_x);
}

superlu_t assemble_dR_csc(assembler_t self,
                          double cx, double* x, 
                          double cv, double* v, 
                          double ca, double* a,
                          int dRsize)
{
#ifdef HAS_SUPERLU
    int i, n;
    superlu_t dRsuper;

    assemble_matrix_t* assembler_x;
    assemble_matrix_t* assembler_v;
    assemble_matrix_t* assembler_a;
    assemble_matrix_t* assembler_dR;

    assembler_x = (x == NULL) ? NULL : 
        assembler_dense_getvector(x, self->num_vars, self->num_vars);

    assembler_v = (v == NULL) ? NULL : 
        assembler_dense_getvector(v, self->num_vars, self->num_vars);

    assembler_a = (a == NULL) ? NULL : 
        assembler_dense_getvector(a, self->num_vars, self->num_vars);

    assembler_dR = 
        assembler_sparse_matrix(dRsize, dRsize);

    n = mesh_num_elements(self->mesh);
    for (i = 1; i <= n; ++i)
        element_dR( mesh_element(self->mesh, i), assembler_dR, 
                    cx, assembler_x,
                    cv, assembler_v,
                    ca, assembler_a);

    assembler_sparse_csc(assembler_dR);
    dRsuper = superlu_create(assembler_dR, dRsize);

    assembler_matrix_free(assembler_dR);
    if (assembler_a) assembler_matrix_free(assembler_a);
    if (assembler_v) assembler_matrix_free(assembler_v);
    if (assembler_x) assembler_matrix_free(assembler_x);

    return dRsuper;
#else
    printf("Library not compiled with SuperLU support\n");
    assert(0);
#endif
}

