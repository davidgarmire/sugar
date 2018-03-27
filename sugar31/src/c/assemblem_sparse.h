#ifndef ASSEMBLEM_SPARSE_H
#define ASSEMBLEM_SPARSE_H

#include <assemblem.h>

assemble_matrix_t* assembler_sparse_matrix(int n, int nactive);
void    assembler_sparse_csc   (assemble_matrix_t* assembler);
int     assembler_sparse_nnz   (assemble_matrix_t* assembler);
double* assembler_sparse_nzval (assemble_matrix_t* assembler);
int*    assembler_sparse_colptr(assemble_matrix_t* assembler);
int*    assembler_sparse_rowind(assemble_matrix_t* assembler);

#endif /* ASSEMBLEM_SPARSE_H */
