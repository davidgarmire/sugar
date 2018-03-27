#ifndef ASSEMBLEM_H
#define ASSEMBLEM_H

typedef void (*assemble_matrix_fun_t)(void* self, int* vars, int n, 
                                      double* local);

typedef struct assemble_matrix_t {
    void*   data;
    void  (*assemble)(void* self, int* vars, int n, double* local);
    void  (*destroy) (void* self);
} assemble_matrix_t;

void assemble_matrix_add(assemble_matrix_t* matrix, int* vars, int n, 
                                 double* local);
void assemble_matrix_add1(assemble_matrix_t* matrix, int var, 
                                  double local);

void assembler_matrix_free(assemble_matrix_t* assembler);

#endif /* ASSEMBLEM_H */
