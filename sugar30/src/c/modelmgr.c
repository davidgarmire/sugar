#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mempool.h"
#include "hash.h"
#include "mesh.h"
#include "modelmgr.h"

/* Standard model headers */
#include "model-anchor.h"
#include "model-force.h"
#include "model-beam2d.h"
#include "model-rigid.h"
#include "model-gap.h"


struct model_mgr_struct {
    mempool_t pool;
    hash_t element_table;
    hash_t material_table;
    model_element_t*  default_element;
    model_material_t* default_material;
};

typedef struct element_entry_t {
    const char* name;
    model_element_t element;
} element_entry_t;

typedef struct material_entry_t {
    const char* name;
    model_material_t material;
} material_entry_t;

static unsigned element_hash(element_entry_t* entry)
{
    return hash_strhash(entry->name);
}

static unsigned material_hash(material_entry_t* entry)
{
    return hash_strhash(entry->name);
}

static int element_compare(element_entry_t* a, element_entry_t* b)
{
    return strcmp(a->name, b->name);
}

static int material_compare(material_entry_t* a, material_entry_t* b)
{
    return strcmp(a->name, b->name);
}

element_t* element_init(model_mgr_t model_mgr,
                        mesh_t mesh, const char* model)
{
    model_element_t* modelfunc = model_mgr_element(model_mgr, model);
    if (modelfunc == NULL) {
        mesh_error(mesh, "Unknown model");
        return NULL;
    }

    return modelfunc->init(mesh, model, modelfunc);
}

void element_destroy(element_t* self)
{
    if (self->model->destroy)
        self->model->destroy(self->data);
}

void element_set_position(element_t* self, struct mesh_struct* mesh)
{
    if (self->model->set_position)
        self->model->set_position(self->data, mesh);
}

void element_vars(element_t* self, struct vars_mgr_struct* vars)
{
    if (self->model->vars)
        self->model->vars(self->data, vars);
}

void element_displace(element_t* self, struct assemble_matrix_t* assembler)
{
    if (self->model->displace)
        self->model->displace(self->data, assembler);
}

void element_R(element_t* self, struct assemble_matrix_t* R,
               struct assemble_matrix_t* x,
               struct assemble_matrix_t* v,
               struct assemble_matrix_t* a)
{
    if (self->model->R)
        self->model->R(self->data, R, x, v, a);
}

void element_dR(element_t* self, struct assemble_matrix_t* dR,
                double cx, struct assemble_matrix_t* x,
                double cv, struct assemble_matrix_t* v,
                double ca, struct assemble_matrix_t* a)
{
    if (self->model->dR)
        self->model->dR(self->data, dR, cx, x, cv, v, ca, a);
}

void element_output(element_t* self, struct netout_t* netout)
{
    if (self->model->output)
        self->model->output(self->data, netout);
}

void element_display(element_t* self, struct netdraw_gc_t* gc)
{
    if (self->model->display)
        self->model->display(self->data, gc);
}

material_t* material_init(model_mgr_t model_mgr,
                          mesh_t mesh, const char* model)
{
    model_material_t* modelfunc = model_mgr_material(model_mgr, model);
    if (modelfunc == NULL) {
        mesh_error(mesh, "Unknown model");
        return NULL;
    }

    return modelfunc->init(mesh, model, modelfunc);
}

void material_destroy(material_t* self)
{
    if (self->model->destroy)
        self->model->destroy(self->data);
}

struct mesh_param_t* material_param(material_t* self, const char* name)
{
    if (self->model->param)
        return self->model->param(self->data, name);
    return NULL;
}

void material_output(material_t* self, struct netout_t* netout)
{
    if (self->model->output)
        self->model->output(self->data, netout);
}

model_mgr_t model_mgr_init()
{
    mempool_t pool = mempool_create(MEMPOOL_DEFAULT_SPAN);
    model_mgr_t self = 
        (model_mgr_t) mempool_cget(pool, sizeof(struct model_mgr_struct));

    self->pool = pool;

    self->element_table = hash_pcreate(pool, 100, 
                                       (hash_compare_fun_t) element_compare,
                                       (hash_hash_fun_t)    element_hash,
                                       NULL);

    self->material_table = hash_pcreate(pool, 100,
                                        (hash_compare_fun_t) material_compare,
                                        (hash_hash_fun_t)    material_hash,
                                        NULL);

    self->default_element = NULL;
    self->default_material = NULL;

    return self;
}

void model_mgr_destroy(model_mgr_t self)
{
    mempool_destroy(self->pool);
}

void model_mgr_add_element(model_mgr_t self, const char* name,
                           model_element_t* element)
{
    element_entry_t* entry = 
        (element_entry_t*) mempool_get(self->pool, sizeof(*entry));
    
    entry->name = mempool_strdup(self->pool, name);
    memcpy(&(entry->element), element, sizeof(*element));
    if (entry->name == NULL)
        self->default_element = &(entry->element);
    else
        hash_insert(self->element_table, entry);
}

void model_mgr_add_material(model_mgr_t self, const char* name,
                            model_material_t* material)
{
    material_entry_t* entry = 
        (material_entry_t*) mempool_get(self->pool, sizeof(*entry));
    
    entry->name = mempool_strdup(self->pool, name);
    memcpy(&(entry->material), material, sizeof(*material));
    if (entry->name == NULL)
        self->default_material = &(entry->material);
    else
        hash_insert(self->material_table, entry);
}

model_element_t* model_mgr_element(model_mgr_t self, const char* name)
{
    element_entry_t key;
    element_entry_t* entry = NULL;
    key.name = name;
    if (name != NULL)
        entry = hash_retrieve(self->element_table, &key);
    return (entry == NULL) ? self->default_element : &(entry->element);
}

model_material_t* model_mgr_material(model_mgr_t self, const char* name)
{
    material_entry_t key;
    material_entry_t* entry = NULL;
    key.name = name;
    if (name != NULL)
        entry = hash_retrieve(self->material_table, &key);
    return (entry == NULL) ? self->default_material : &(entry->material);
}

void model_mgr_standard_register(model_mgr_t self)
{
    model_anchor_register (self);
    model_force_register  (self);
    model_beam2d_register (self);
    model_rigid_register  (self);
    model_gap_register    (self);
}

