#ifndef MODELMGR_H
#define MODELMGR_H

#include <stdio.h>

struct mesh_struct;
struct mesh_param_t;
struct vars_mgr_struct;
struct assembler_struct;
struct assemble_matrix_t;
struct netdraw_gc_t;
struct netout_t;
typedef struct model_mgr_struct* model_mgr_t;

typedef struct model_element_t {
    struct element_t* (*init)(struct mesh_struct* mesh, const char* model,
                              struct model_element_t* modelfunc);
    void (*destroy)(void* self);
    void (*set_position)(void* self, struct mesh_struct* mesh);
    void (*vars)(void* self, struct vars_mgr_struct* vars);
    void (*displace)(void* self, struct assemble_matrix_t* assembler);
    void (*R) (void* self, struct assemble_matrix_t* R, 
               struct assemble_matrix_t* x,
               struct assemble_matrix_t* v,
               struct assemble_matrix_t* a);
    void (*dR)(void* self, struct assemble_matrix_t* dR,
               double cx, struct assemble_matrix_t* x,
               double cv, struct assemble_matrix_t* v,
               double ca, struct assemble_matrix_t* a);
    void (*output)(void* self, struct netout_t* netout);
    void (*display)(void* self, struct netdraw_gc_t* gc);
} model_element_t;

typedef struct element_t {
    model_element_t* model;
    void* data;
} element_t;

typedef struct model_material_t {
    struct material_t* (*init)(struct mesh_struct* mesh, const char* model,
                               struct model_material_t* modelfunc);
    void (*destroy)(void* self);
    struct mesh_param_t* (*param)(void* self, const char* name);
    void (*output)(void* self, struct netout_t* netout);
} model_material_t;

typedef struct material_t {
    model_material_t* model;
    void* data;
} material_t;

element_t* element_init(model_mgr_t model_mgr, 
                        struct mesh_struct* mesh, const char* model);
void element_destroy(element_t* self);
void element_set_position(element_t* self, struct mesh_struct* mesh);
void element_vars(element_t* self, struct vars_mgr_struct* vars);
void element_displace(element_t* self, struct assemble_matrix_t* assembler);
void element_R (element_t* self, struct assemble_matrix_t* R,
                struct assemble_matrix_t* x,
                struct assemble_matrix_t* v,
                struct assemble_matrix_t* a);
void element_dR(element_t* self, struct assemble_matrix_t* dR,
                double cx, struct assemble_matrix_t* x,
                double cv, struct assemble_matrix_t* v,
                double ca, struct assemble_matrix_t* a);
void element_output(element_t* self, struct netout_t* netout);
void element_display(element_t* self, struct netdraw_gc_t* gc);
struct material_t* material_init(model_mgr_t model_mgr, 
                                 struct mesh_struct* mesh, const char* model);
void material_destroy(material_t* self);
struct mesh_param_t* material_param(material_t* self, const char* name);
void material_output(material_t* self, struct netout_t* netout);
model_mgr_t model_mgr_init();
void        model_mgr_destroy(model_mgr_t self);

void        model_mgr_standard_register(model_mgr_t self);
void model_mgr_add_element (model_mgr_t self, const char* name, 
                            model_element_t*  funcs);
void model_mgr_add_material(model_mgr_t self, const char* name, 
                            model_material_t* funcs);

model_element_t*  model_mgr_element (model_mgr_t self, const char* name);
model_material_t* model_mgr_material(model_mgr_t self, const char* name);

#endif /* MODELMGR_H */
