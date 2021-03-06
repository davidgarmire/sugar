#include <sugar.h>

#include <netout.h>
#include <netout_file.h>

static void write_string(void* pself, const char* name, const char* s)
{
    FILE* fp = (FILE*) pself;
    fprintf(fp, "  %s = '%s'\n", name, s);
}

static void write_double(void* pself, const char* name, double d)
{
    FILE* fp = (FILE*) pself;
    fprintf(fp, "  %s = %g\n", name, d);
}

static void write_int(void* pself, const char* name, int i)
{
    FILE* fp = (FILE*) pself;
    fprintf(fp, "  %s = %d\n", name, i);
}

static void write_double_matrix(void* pself, const char* name,
                                        double* darray, int m, int n)
{
    FILE* fp = (FILE*) pself;
    int i, j;

    fprintf(fp, "  %s = [\n", name);
    for (i = 0; i < m; ++i) {
        fprintf(fp, "    ");
        for (j = 0; j < n; ++j)
            fprintf(fp, "%g ", darray[j*m+i]);
        fprintf(fp, "\n");
    }
    fprintf(fp, "  ]\n");
}

static void write_int_matrix(void* pself, const char* name,
                                     int* iarray, int m, int n)
{
    FILE* fp = (FILE*) pself;
    int i, j;

    fprintf(fp, "  %s = [\n", name);
    for (i = 0; i < m; ++i) {
        fprintf(fp, "    ");
        for (j = 0; j < n; ++j)
            fprintf(fp, "%d ", iarray[j*m+i]);
        fprintf(fp, "\n");
    }
    fprintf(fp, "  ]\n");
}

void netout_file(FILE* fp, mesh_t mesh)
{
    int i, n;
    netout_fun_t funs;
    netout_t     self;

    funs.write_string = write_string;
    funs.write_double = write_double;
    funs.write_int    = write_int;
    funs.write_double_matrix = write_double_matrix;
    funs.write_int_matrix    = write_int_matrix;

    self.methods = &funs;
    self.data    = fp;

    n = mesh_num_nodes(mesh);
    for (i = 1; i <= n; ++i) {
        mesh_node_t* node = mesh_node(mesh, i);
        fprintf(fp, "%d: %s = [%g, %g, %g]\n", i, 
                (node->name == NULL ? "(anon)" : node->name),
                node->x[0], node->x[1], node->x[2]);
    }
    fprintf(fp, "\n");

    n = mesh_num_materials(mesh);
    for (i = 1; i <= n; ++i) {
        material_t* material = mesh_material(mesh, i);
        fprintf(fp, "material {\n");
        material_output(material, &self);
        fprintf(fp, "}\n\n");
    }

    n = mesh_num_elements(mesh);
    for (i = 1; i <= n; ++i) {
        element_t* element = mesh_element(mesh, i);
        fprintf(fp, "element {\n");
        element_output(element, &self);
        fprintf(fp, "}\n\n");
    }
}

