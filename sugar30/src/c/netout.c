#include <assert.h>
#include "netout.h"

void netout_string(netout_t* self, const char* name, const char* s)
{
    assert(self->methods->write_string);
    self->methods->write_string(self->data, name, s);
}

void netout_double(netout_t* self, const char* name, double d)
{
    assert(self->methods->write_double);
    self->methods->write_double(self->data, name, d);
}

void netout_int(netout_t* self, const char* name, int i)
{
    assert(self->methods->write_int);
    self->methods->write_int(self->data, name, i);
}

void netout_double_matrix(netout_t* self, const char* name,
                          double* d, int m, int n)
{
    assert(self->methods->write_double_matrix);
    self->methods->write_double_matrix(self->data, name, d, m, n);
}

void netout_int_matrix(netout_t* self, const char* name,
                       int* i, int m, int n)
{
    assert(self->methods->write_int_matrix);
    self->methods->write_int_matrix(self->data, name, i, m, n);
}

