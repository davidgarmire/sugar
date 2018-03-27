#ifndef ASSEMBLEM_DENSE_H
#define ASSEMBLEM_DENSE_H

#include <assemblem.h>

assemble_matrix_t* assembler_dense_vector   (double* data, int n, 
                                                     int ldA);
assemble_matrix_t* assembler_dense_getvector(double* data, int n, 
                                                     int ldA);
assemble_matrix_t* assembler_dense_matrix   (double* data, int n, 
                                                     int ldA);
assemble_matrix_t* assembler_bc(double* displacements, char* has_bc, 
                                        int n);

#endif /* ASSEMBLEM_DENSE_H */
