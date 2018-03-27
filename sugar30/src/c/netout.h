#ifndef NETOUT_H
#define NETOUT_H

typedef struct netout_fun_t {
    void (*write_string)(void* pself, const char* name, const char* s);
    void (*write_double)(void* pself, const char* name, double d);
    void (*write_int)   (void* pself, const char* name, int i);
    void (*write_double_matrix)(void* pself, const char* name, 
                                double* d, int m, int n);
    void (*write_int_matrix)   (void* pself, const char* name, 
                                int* i, int m, int n);
} netout_fun_t;

typedef struct netout_t {
    netout_fun_t* methods;
    void* data;
} netout_t;

void netout_string(netout_t* self, const char* name, const char* s);
void netout_double(netout_t* self, const char* name, double d);
void netout_int   (netout_t* self, const char* name, int i);
void netout_double_matrix(netout_t* self, const char* name,
                          double* d, int m, int n);
void netout_int_matrix   (netout_t* self, const char* name,
                          int* i, int m, int n);

#endif /* NETOUT_H */
