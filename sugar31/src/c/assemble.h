#ifndef ASSEMBLE_H
#define ASSEMBLE_H

#include <mempool.h>
#include <mesh.h>
#include <vars.h>

#include <assemblem.h>
#include <assemblem_sparse.h>
#include <assemblem_dense.h>
#include <superlu.h>

typedef struct assembler_struct* assembler_t;

assembler_t assemble_pcreate(mempool_t pool, mesh_t mesh);
assembler_t assemble_create (mesh_t mesh);
void        assemble_destroy(assembler_t self);

void    assemble_add_displace(assembler_t self, 
                                      int var_id, double value);
void    assemble_displace    (assembler_t self);

double* assemble_get_displacements(assembler_t self);
int     assemble_get_active       (assembler_t self);

void assemble_R   (assembler_t self, double* x, double* v, double* a,
                           double* R, int Rsize);
void assemble_dR  (assembler_t self,
                           double cx, double* x, 
                           double cv, double* v, 
                           double ca, double* a, 
                           double* dR,
                           int dRsize);
superlu_t assemble_dR_csc(assembler_t self,
                          double cx, double* x, 
                          double cv, double* v, 
                          double ca, double* a,
                          int dRsize);


#endif /* ASSEMBLE_H */
