#ifndef MESH_H
#define MESH_H

#include <mempool.h>
#include <modelmgr.h>

typedef struct mesh_struct* mesh_t;

typedef struct mesh_node_t {
    const char* name;
    double x[3];
} mesh_node_t;

enum {
    MESH_PARAM_NUMBER,
    MESH_PARAM_STRING,
    MESH_PARAM_MATRIX
};

typedef struct mesh_param_t {
    int tag;
    const char* name;
    union {
        double d;
        const char* s;
        struct {
            int m, n;
            double* data;
        } m;
    } val;
} mesh_param_t;

mesh_t mesh_create(model_mgr_t model_mgr);
void   mesh_destroy(mesh_t self);

mempool_t mesh_pool(mesh_t self);

void mesh_set_handlers(mesh_t self,
       void (*error_handler)  (void* arg, const char* msg),
       void (*warning_handler)(void* arg, const char* msg),
       void* arg);
void mesh_error       (mesh_t self, const char* msg);
void mesh_warning     (mesh_t self, const char* msg);


void mesh_add_param_string(mesh_t self, const char* name, 
                                   const char* s);
void mesh_add_param_number(mesh_t self, const char* name, double d);
void mesh_add_param_matrix(mesh_t self, const char* name, int m, int n,
                                   double* data);
void mesh_add_param_node  (mesh_t self, int node_id);

mesh_param_t* mesh_param       (mesh_t self, int index);
mesh_param_t* mesh_param_byname(mesh_t self, material_t* material,
                                        const char* name);
int           mesh_param_node  (mesh_t self, int index);

int mesh_num_params     (mesh_t self);
int mesh_num_param_nodes(mesh_t self);

void mesh_params_clear(mesh_t self);

void mesh_param_get_nodes(mesh_t self, int* nodes, int node_count);

const char* mesh_param_string  (mesh_t self, mesh_param_t* param);
double      mesh_param_number  (mesh_t self, mesh_param_t* param);
int         mesh_param_int     (mesh_t self, mesh_param_t* param);
material_t* mesh_param_material(mesh_t self, mesh_param_t* param);

const char* mesh_param_get_string  (mesh_t self, material_t* material,
                                            const char* name);
double      mesh_param_get_number  (mesh_t self, material_t* material,
                                            const char* name);
int         mesh_param_get_int     (mesh_t self, material_t* material,
                                            const char* name);
material_t* mesh_param_get_material(mesh_t self, const char* name);

int mesh_add_node    (mesh_t self, const char* name, 
                              double x, double y, double z);
int mesh_add_element (mesh_t self, const char* model);
int mesh_add_material(mesh_t self, const char* model);

mesh_node_t*  mesh_node    (mesh_t self, int id);
element_t*    mesh_element (mesh_t self, int id);
material_t*   mesh_material(mesh_t self, int id);

int mesh_num_nodes    (mesh_t self);
int mesh_num_elements (mesh_t self);
int mesh_num_materials(mesh_t self);

struct vars_mgr_struct*  mesh_vars_mgr(mesh_t self);
struct assembler_struct* mesh_assembler(mesh_t self);

#endif /* MESH_H */
