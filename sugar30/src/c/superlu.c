#ifdef HAS_SUPERLU

#include <stdlib.h>
#include <assert.h>

#include <dsp_defs.h>
#include <util.h>

#include "superlu.h"

struct superlu_struct {
    SuperMatrix A;
    SuperMatrix L;
    SuperMatrix U;
    int  is_factored;

    int* perm_r;
    int* perm_c;
    int* etree;

    double* row_scales;
    double* col_scales;
    double rowcnd, colcnd, Amax;
    char equed;

    int  n;
};

superlu_t superlu_create(assemble_matrix_t* assembler, int n)
{
    superlu_t self = calloc(1, sizeof(*self));

    int     nnz    = assembler_sparse_nnz(assembler);
    double* nzval  = doubleCalloc( nnz );
    int*    rowind = intCalloc   ( nnz );
    int*    colptr = intCalloc   ( n+1 );

    memcpy( nzval,  assembler_sparse_nzval(assembler),  nnz  * sizeof(double));
    memcpy( rowind, assembler_sparse_rowind(assembler), nnz  * sizeof(int) );
    memcpy( colptr, assembler_sparse_colptr(assembler), (n+1) * sizeof(int) );

    self->n = n;
    dCreate_CompCol_Matrix(&(self->A), n, n, nnz, nzval, rowind, colptr,
                           NC, _D, GE);

    self->perm_c = intMalloc( n );
    self->perm_r = intMalloc( n );
    self->etree  = intMalloc( n );

    self->col_scales = doubleMalloc( n );
    self->row_scales = doubleMalloc( n );

    return self;
}

void superlu_destroy(superlu_t self)
{
    if (self->is_factored) {
        Destroy_SuperNode_Matrix (&(self->L));
        Destroy_CompCol_Matrix   (&(self->U));
    }
    SUPERLU_FREE(self->row_scales);
    SUPERLU_FREE(self->col_scales);
    SUPERLU_FREE(self->perm_r);
    SUPERLU_FREE(self->perm_c);
    SUPERLU_FREE(self->etree);
    Destroy_CompCol_Matrix(&(self->A));
    free(self);
}

void superlu_print(superlu_t self)
{
    dPrint_CompCol_Matrix("A", &(self->A));
    if (self->is_factored) {
        dPrint_CompCol_Matrix   ("U", &(self->U));
        dPrint_SuperNode_Matrix ("L", &(self->L));
    }
}

void superlu_factor(superlu_t self)
{
    char* refact = "N";
    SuperMatrix AC;     /* Matrix postmultiplied by Pc */
    int lwork = 0;
    int info;
    int permc_spec = 2; /* Minimum degree on A + A^T */
    
    double diag_pivot_thresh = 1.0;
    double drop_tol          = 0;
    int    panel_size = sp_ienv(1); /* panel size */
    int    relax      = sp_ienv(2); /* no of columns in a relaxed snodes */

    if (self->is_factored)
        return;

    get_perm_c(permc_spec, &(self->A), self->perm_c);

    StatInit(panel_size, relax);
 
    sp_preorder(refact, &(self->A), self->perm_c, self->etree, &AC);

    /* Compute the LU factorization of A. */
    dgstrf(refact, &AC, diag_pivot_thresh, drop_tol, relax, panel_size,
           (self->etree), NULL, lwork, 
           (self->perm_r), (self->perm_c), &(self->L), &(self->U), 
           &info);

    Destroy_CompCol_Permuted(&AC);

    StatFree();
    assert(info == 0);

    self->is_factored = 1;
}

void superlu_solve(superlu_t self, double* b)
{
    char* trans = "N";
    int info;
    
    int panel_size = sp_ienv(1); /* panel size */
    int relax      = sp_ienv(2); /* no of columns in a relaxed snodes */

    SuperMatrix B;
    dCreate_Dense_Matrix(&B, self->n, 1, b, self->n, DN, _D, GE);

    if (!self->is_factored)
        superlu_factor(self);

    StatInit(panel_size, relax);
    dgstrs(trans, &(self->L), &(self->U), self->perm_r, self->perm_c, 
           &B, &info);
    StatFree();

    Destroy_SuperMatrix_Store(&B);
    assert(info == 0);
}

int superlu_n(superlu_t self)
{
    return self->n;
}


#endif /* HAS_SUPERLU */
