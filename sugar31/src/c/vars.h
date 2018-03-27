#ifndef VARS_H
#define VARS_H

struct mesh_struct;
typedef struct vars_mgr_struct* vars_mgr_t;

typedef struct vars_desc_t {
    const char*  name;
    int          offset;
    int          parent;
    char         type;
} vars_desc_t;

vars_mgr_t vars_create (struct mesh_struct* mesh);
void       vars_destroy(vars_mgr_t self);

int  vars_add_vartype (vars_mgr_t self, const char* name);
int  vars_vartype     (vars_mgr_t self, const char* name);
void vars_get_vartypes(vars_mgr_t self);

int vars_node  (vars_mgr_t self, int node_id, const char* name);
int vars_branch(vars_mgr_t self, const char* name);

void vars_assign(vars_mgr_t self);
int  vars_count (vars_mgr_t self);

void vars_permute(vars_mgr_t self, int* pi);

int vars_lookup_node  (vars_mgr_t self, int node_id, const char* name);
int vars_lookup_branch(vars_mgr_t self, int elt_id,  int var_number);
void vars_lookup(vars_mgr_t self, vars_desc_t* v);


#endif /* VARS_H */
