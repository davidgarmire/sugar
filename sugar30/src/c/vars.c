#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "mempool.h"
#include "dynarray.h"
#include "mesh.h"
#include "vars.h"

struct vars_mgr_struct {
    dynarray_t var_types;
    int* node_vars;
    int* branch_vars;
    int* elt_branch_vars;
    int var_count;
    int branch_var_count;
    mesh_t mesh;
    mempool_t pool;
    int mode;
};

#define ASSIGN_VARS 0
#define REGISTER_VARS 1

#define num_types(self) dynarray_count((self)->var_types)

vars_mgr_t vars_create(mesh_t mesh)
{
    mempool_t pool = mempool_create(MEMPOOL_DEFAULT_SPAN);
    vars_mgr_t self = (vars_mgr_t) mempool_cget(pool, sizeof(*self));

    self->pool = pool;
    self->var_types = dynarray_create(sizeof(const char*), 16);
    self->mesh = mesh;

    self->node_vars   = NULL;
    self->branch_vars = NULL;
    self->elt_branch_vars = NULL;

    return self;
}

void vars_destroy(vars_mgr_t self)
{
    dynarray_destroy(self->var_types);
    mempool_destroy(self->pool);
}

int vars_vartype(vars_mgr_t self, const char* name)
{
    int i, n;

    n = dynarray_count(self->var_types);
    for (i = 0; i < n; ++i)
        if (strcmp(name, *(char**) dynarray_get(self->var_types, i)) == 0)
            return i;
    return -1;
}

int vars_add_vartype(vars_mgr_t self, const char* name)
{
    int var_id = vars_vartype(self, name);
    if (var_id < 0) {
        name = mempool_strdup(self->pool, name);
        dynarray_append(self->var_types, &name);
        var_id = num_types(self)-1;
    }
    return var_id;
}

int vars_node(vars_mgr_t self, int node_id, const char* name)
{
    int var_type  = vars_vartype(self, name);
    int var_index = (node_id-1)*num_types(self) + var_type;

    if (self->mode == REGISTER_VARS) {
        vars_add_vartype(self, name);
        return 0;
    } else {
        if (var_type < 0)
            mesh_error(self->mesh, "Invalid nodal variable type");
        if (self->node_vars[var_index] == 0) 
            self->node_vars[var_index] = ++(self->var_count);
        return self->node_vars[var_index]-1;
    }
}

int vars_branch(vars_mgr_t self, const char* name)
{
    if (self->mode == REGISTER_VARS) {
        (self->branch_var_count)++;
        return 0;
    } else {
        if (self->branch_vars[self->branch_var_count] == 0) {
            return (self->branch_vars[(self->branch_var_count)++] = 
                    ++self->var_count)-1;
        } else {
            return self->branch_vars[(self->branch_var_count)++]-1;
        }
    }
}

void vars_assign(vars_mgr_t self)
{
    int i, n;

    if (self->node_vars)
         mempool_freeh(self->node_vars);

    self->node_vars = 
    (int*) mempool_cgeth(self->pool, num_types(self) * 
                         mesh_num_nodes(self->mesh) * sizeof(int));

    self->branch_vars = 
    (int*) mempool_cgeth(self->pool, self->branch_var_count * sizeof(int));

    self->elt_branch_vars = 
    (int*) mempool_cgeth(self->pool, (mesh_num_elements(self->mesh) + 1) * 
                         sizeof(int));

    self->mode = ASSIGN_VARS;
    self->var_count = 0;
    self->branch_var_count = 0;


    n = mesh_num_elements(self->mesh);
    for (i = 1; i <= n; ++i) {
        self->elt_branch_vars[i-1] = self->branch_var_count;
        element_vars( mesh_element(self->mesh, i), self );
    }
    self->elt_branch_vars[n] = self->branch_var_count;
}

int vars_count(vars_mgr_t self)
{
    return self->var_count;
}

void vars_permute(vars_mgr_t self, int* pi)
{
    int ntypes = num_types(self);
    int nnodes = mesh_num_nodes(self->mesh);
    int i, n;

    for (i = 0; i < nnodes * ntypes; ++i) {
        if (self->node_vars[i] != 0)
            self->node_vars[i] = pi[self->node_vars[i]-1] + 1;
    }

    for (i = 0; i < self->branch_var_count; ++i) {
        if (self->branch_vars[i] != 0)
            self->branch_vars[i] = pi[self->branch_vars[i]-1] + 1;
    }

    self->mode = ASSIGN_VARS;
    self->branch_var_count = 0;
    n = mesh_num_elements(self->mesh);
    for (i = 1; i <= n; ++i) {
        element_vars( mesh_element(self->mesh, i), self );
    }
}

void vars_get_vartypes(vars_mgr_t self)
{
    int i, n;

    self->mode = REGISTER_VARS;
    self->branch_var_count = 0;

    self->branch_var_count = 0;
    n = mesh_num_elements(self->mesh);
    for (i = 1; i <= n; ++i) {
        element_vars( mesh_element(self->mesh, i), self );
    }
}

int vars_lookup_node(vars_mgr_t self, int node_id, const char* name)
{
    int var_type, var_index;

    assert(self->node_vars);
    if (node_id <= 0 || node_id > mesh_num_nodes(self->mesh) ||
        (var_type = vars_vartype(self, name)) < 0) {
        return -1;
    }

    var_index = (node_id-1)*num_types(self) + var_type;
    return self->node_vars[var_index]-1;
}

int vars_lookup_branch(vars_mgr_t self, int elt_id, int var_number)
{
    int var_index;

    assert(self->elt_branch_vars);
    if (elt_id <= 0 || elt_id > mesh_num_elements(self->mesh)) {
        return -1;
    }   

    var_index = self->elt_branch_vars[elt_id-1] + var_number-1;
    if (var_index >= self->elt_branch_vars[elt_id]) {
        return -1;
    }

    return self->branch_vars[var_index];
}

void vars_lookup(vars_mgr_t self, vars_desc_t* v)
{
    int nnodes    = mesh_num_nodes(self->mesh);
    int nelements = mesh_num_elements(self->mesh);
    int ntypes    = dynarray_count(self->var_types);
    int i, j;

    for (i = 0; i < nnodes; ++i) {
        for (j = 0; j < ntypes; ++j) {
            int vid = self->node_vars[i*ntypes + j]-1;
            if (vid >= 0) {
                v[vid].type   = 'n';
                v[vid].parent = i+1;
                v[vid].name   = *(char**) dynarray_get(self->var_types, j);
            }
        }
    }

    for (i = 0; i < nelements; ++i) {
        for (j=self->elt_branch_vars[i]; j < self->elt_branch_vars[i+1]; ++j) {
            int vid = self->branch_vars[j]-1;
            v[vid].type   = 'b';
            v[vid].parent = i+1;
            v[vid].offset = j - self->elt_branch_vars[i];
        }
    }
}

