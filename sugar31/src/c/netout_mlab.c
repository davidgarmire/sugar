#include <sugar.h>

#include <mesh.h>
#include <netout.h>
#include <netout_mlab.h>

typedef struct netout_mlab_stack_t {
    const char* name;
    char        type;
    const char* s;
    double*     d;
    int         m, n;
    struct netout_mlab_stack_t* next;
} netout_mlab_stack_t;

typedef struct {
    mempool_t pool;
    netout_mlab_stack_t* head;
    void* frame;
} netout_mlab_t;

static mxArray* netout_mlab_pack(netout_t* netout)
{
    int i, n;
    netout_mlab_t*       self = netout->data;
    netout_mlab_stack_t* link;
    const char**         names;
    mxArray*             result;

    n = 0;
    for (link = self->head; link != NULL; link = link->next)
        ++n;

    i = 0;
    names = mxCalloc(n, sizeof(char*));
    for (link = self->head; link != NULL; link = link->next)
        names[i++] = link->name;

    result = mxCreateStructMatrix(1,1,n,names);
    mxFree(names);

    i = 0;
    for (link = self->head; link != NULL; link = link->next) {
        mxArray* array;
        if (link->type == 's') {
            array = mxCreateString(link->s);
        } else if (link->type == 'd') {
            array = mxCreateDoubleMatrix(link->m, link->n, mxREAL);
            memcpy(mxGetPr(array), link->d, link->m*link->n * sizeof(double));
        } else {
            mexErrMsgTxt("Error in netout -- unidentified argument type\n");
        }
        mxSetFieldByNumber(result, 0, i++, array);
    }

    if (self->head) {
        for (link = self->head; link->next != NULL; link = link->next);
        self->head = NULL;
        mempool_freeh(link);
    }

    return result;
}

netout_mlab_stack_t* get_link(netout_mlab_t* self)
{
    netout_mlab_stack_t* link;

    if (self->head == NULL)
        link = mempool_cgeth(self->pool, sizeof(*link));
    else
        link = mempool_cget(self->pool, sizeof(*link));

    link->next = self->head;
    self->head = link;

    return link;
}

static void write_string(void* pself, const char* name, const char* s)
{
    netout_mlab_t*       self = pself;
    netout_mlab_stack_t* link = get_link(self);

    link->name = mempool_strdup(self->pool, name);
    link->type = 's';
    link->s    = mempool_strdup(self->pool, s);
}

static void write_double_matrix(void* pself, const char* name, 
                                        double* data, int m, int n)
{
    netout_mlab_t*       self = pself;
    netout_mlab_stack_t* link = get_link(self);

    link->name = mempool_strdup(self->pool, name);
    link->type = 'd';
    link->d    = mempool_cget(self->pool, m*n * sizeof(double));
    link->m    = m;
    link->n    = n;
    memcpy(link->d, data, m*n * sizeof(double));
}

static void write_int_matrix(void* pself, const char* name, int* data,
                                     int m, int n)
{
    int i;
    netout_mlab_t*       self = pself;
    netout_mlab_stack_t* link = get_link(self);

    link->name = mempool_strdup(self->pool, name);
    link->type = 'd';
    link->d    = mempool_cget(self->pool, m*n * sizeof(double));
    link->m    = m;
    link->n    = n;
    for (i = 0; i < m*n; ++i)
        link->d[i] = *data++;
}

static void write_int(void* pself, const char* name, int i)
{
    write_int_matrix(pself, name, &i, 1, 1);
}

static void write_double(void* pself, const char* name, double d)
{
    write_double_matrix(pself, name, &d, 1, 1);
}

netout_t* netout_mlab_create()
{
    static netout_fun_t funs = {
        write_string, write_double, write_int,
        write_double_matrix, write_int_matrix
    };

    mempool_t      pool   = mempool_create(MEMPOOL_DEFAULT_SPAN);
    netout_t*      netout = mempool_cget(pool, sizeof(*netout));
    netout_mlab_t* self   = mempool_cget(pool, sizeof(*self));

    netout->methods = &funs;
    netout->data    = self;

    self->pool  = pool;
    self->head  = NULL;

    return netout;
}

void netout_mlab_destroy(netout_t* self)
{
    mempool_destroy(((netout_mlab_t*) (self->data))->pool);
}

mxArray* netout_mlab_node(netout_t* netout, mesh_node_t* node)
{
    if (node->name)
        netout_string(netout, "name", node->name);
    netout_double_matrix(netout, "x", node->x, 1, 3);
    return netout_mlab_pack(netout);
}

mxArray* netout_mlab_element(netout_t* netout, element_t* element)
{
    element_output(element, netout);
    return netout_mlab_pack(netout);
}

mxArray* netout_mlab_material(netout_t* netout, material_t* material)
{
    material_output(material, netout);
    return netout_mlab_pack(netout);
}

mxArray* netout_mlab(mesh_t mesh)
{
    const char* names[] = {"node", "material", "element"};
    int i, n;

    mxArray* nodes     = mxCreateCellMatrix(mesh_num_nodes(mesh),     1);
    mxArray* materials = mxCreateCellMatrix(mesh_num_materials(mesh), 1);
    mxArray* elements  = mxCreateCellMatrix(mesh_num_elements(mesh),  1);
    mxArray* netlist   = mxCreateStructMatrix(1, 1, 3, names);

    netout_t* self = netout_mlab_create();

    n = mesh_num_nodes(mesh);
    for (i = 0; i < n; ++i) {
        mesh_node_t* node = mesh_node(mesh, i+1);
        mxArray* data = netout_mlab_node(self, node);
        mxSetCell(nodes, i, data);
    }

    n = mesh_num_materials(mesh);
    for (i = 0; i < n; ++i) {
        mxArray* data = netout_mlab_material(self, mesh_material(mesh, i+1));
        mxSetCell(materials, i, data);
    }

    n = mesh_num_elements(mesh);
    for (i = 0; i < n; ++i) {
        mxArray* data = netout_mlab_element(self, mesh_element(mesh, i+1));
        mxSetCell(elements, i, data);
    }

    mxSetFieldByNumber(netlist, 0, 0, nodes);
    mxSetFieldByNumber(netlist, 0, 1, materials);
    mxSetFieldByNumber(netlist, 0, 2, elements);

    netout_mlab_destroy(self);

    return netlist;
}

