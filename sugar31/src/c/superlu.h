#ifndef SUPERLU_H
#define SUPERLU_H

#include <assemblem_sparse.h>

typedef struct superlu_struct* superlu_t;

superlu_t superlu_create  (assemble_matrix_t* assembler, int n);
void      superlu_destroy (superlu_t self);
void      superlu_print   (superlu_t self);
void      superlu_factor  (superlu_t self);
void      superlu_solve   (superlu_t self, double* b);
int       superlu_n       (superlu_t self);

#endif /* SUPERLU_H   */
