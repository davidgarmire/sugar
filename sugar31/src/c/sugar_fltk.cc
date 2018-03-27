#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

extern "C" {
#include <lua.h>
#include <lualib.h>

#include <modelmgr.h>
#include <model_test.h>
#include <vars.h>
#include <assemble.h>
#include <uses_manager.h>

#include <uses_manager_lua.h>
#include <meshgen_lua.h>
#include <mesh_lua.h>
#include <matrix_lua.h>
#include <superlu_lua.h>
}

#include <FL/Fl.H>
#include <FL/Fl_Group.H>
#include <FL/Fl_Double_Window.H>
#include <FL/fl_ask.H>
#include <FL/Fl_File_Chooser.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/Fl_Input.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Return_Button.H>
#include <FL/Fl_Text_Buffer.H>
#include <FL/Fl_Text_Editor.H>
#include <Fl/Fl_Box.H>
#include <Fl/fl_draw.H>

#include <netdraw_fltk.hh>

static lua_State*         L;
static uses_manager_t     uses_manager;
static model_mgr_t        model_mgr;
static Fl_Text_Buffer    *textbuf;

class EditorWindow : public Fl_Double_Window {
public:
    EditorWindow(int w, int h, const char* t);
    ~EditorWindow();

    Fl_Text_Editor     *editor;
    DrawBox            *drawbox;
};

static int lua_alert(lua_State* L)
{
    int n = lua_gettop(L);  /* number of arguments */

    lua_getglobal(L, "tostring");
    lua_pushvalue(L, 1);
    lua_rawcall(L, 1, 1);

    const char* s = lua_tostring(L, -1);
    if (s != NULL)
        fl_alert("%s", s);

    lua_settop(L, 0);
    return 0;
}

static void error_handler(void* arg, const char* msg)
{
    lua_State* L = (lua_State*) arg;
    lua_error(L, msg);
}

static int lua_print(lua_State *L)
{
    int n = lua_gettop(L);  /* number of arguments */

    Fl_Text_Editor* e = (Fl_Text_Editor*) lua_touserdata(L, -1);
    lua_pop(L,1);
    --n;

    lua_getglobal(L, "tostring");
    for (int i=1; i<=n; i++) {
        lua_pushvalue(L, -1);  /* function to be called */
        lua_pushvalue(L,  i);  /* value to print */
        lua_rawcall(L, 1, 1);
        const char* s = lua_tostring(L, -1);
        if (s == NULL)
            lua_error(L, "`tostring' must return a string to `print'");
        if (i>1) e->insert("\t");
        e->insert(s);
        lua_pop(L, 1);
    }
    e->insert("\n");
    e->show_insert_position();

    lua_settop(L, 0);
    return 0;
}

static int lua_netload(lua_State* L)
{
    int n = lua_gettop(L);
    mesh_t mesh;

    if (n != 1 || !lua_isstring(L,-1))
        lua_error(L, "Invalid argument to 'cho_load'");

    mesh = mesh_lua_load(L, uses_manager, lua_tostring(L,-1),
                         model_mgr, error_handler);

    if (mesh == NULL)
        lua_error(L, "Error opening file");

    lua_settop(L,0);
    lua_mesh_create(L, mesh);
    return 1;
}

static int lua_draw(lua_State* L)
{
    int n = lua_gettop(L);  /* number of arguments */

    DrawBox* box = (DrawBox*) lua_touserdata(L, -1);
    lua_pop(L, 1);
    --n;

    mesh_t  mesh = NULL;
    double* x    = NULL;

    if (n >= 1) {
        mesh = lua_mesh_get(L,1);
    }
    if (n >= 2) {
        lua_matrix_t xarg      = lua_matrix_get(L,2);
        vars_mgr_t   vars_mgr  = mesh_vars_mgr(mesh);
        assembler_t  assembler = mesh_assembler(mesh);
        int          nvars     = vars_count(vars_mgr);
        int          nactive   = assemble_get_active(assembler);
        int          i;
        
        double*      displacements = assemble_get_displacements(assembler);
        
        if (xarg->m != nactive || xarg->n != 1)
            lua_error(L, "State vector is wrong size");
        
        x = (double*) calloc(nvars, sizeof(double));
        memcpy(x, displacements, nvars * sizeof(double));
        memcpy(x, xarg->data, nactive * sizeof(double));
        
    }
    if (n > 2)
        lua_error(L, "Too many arguments to cho_display");

    box->set_mesh(mesh);
    box->set_disp(x);

    lua_settop(L, 0);
    return 0;
}

static int enter_key_cb(int, Fl_Text_Editor* e)
{
    int old_position = e->insert_position();
    e->insert("\n");
    lua_dostring(L, textbuf->line_text(old_position));
    e->show_insert_position();
    return 1;
}

static void changed_cb(int, int nInserted, int nDeleted,int, 
                       const char*, void* v) 
{
    EditorWindow *w = (EditorWindow *)v;
    w->editor->show_insert_position();
}

static void quit_cb(Fl_Widget*, void*) 
{
    lua_close(L);
    exit(0);
}

static void open_cb(Fl_Widget*, void*) 
{
    char *newfile = fl_file_chooser("Open File?", "*.lua", NULL);
    if (newfile != NULL) 
        lua_dofile(L, newfile);
}

static void load_cb(Fl_Widget*, void*) 
{
    char *newfile = fl_file_chooser("Open File?", "*.net", NULL);
    if (newfile != NULL) {
        char buf[256];
        sprintf(buf, "net = cho_load(\"%s\"); draw(net);", newfile);
        lua_dostring(L, buf);
    }
}

static void save_file(int save_as) 
{
    static char filename[128] = "";

    char* newfile = filename;
    if (save_as || *filename == '\0')
            newfile = fl_file_chooser("Save File As?", "*", filename);

    if (newfile != NULL && textbuf->savefile(newfile))
        fl_alert("Error writing to file \'%s\':\n%s.", 
                 newfile, strerror(errno));
}

static void saveas_cb(void) 
{
    save_file(1);
}

static void save_cb(void) 
{
    save_file(0);
}

static void new_net_cb(Fl_Widget*, void*)
{
    mesh_t mesh = mesh_create(model_mgr);
    mesh_set_handlers(mesh, error_handler, NULL, L);
    lua_meshgen_register(L, mesh);

    lua_mesh_create(L, mesh);
    lua_setglobal(L, "net");

    lua_dostring(L, "draw(net)");
    lua_dostring(L, "element = function(p) %element(p); draw(net); end");
}

static void end_net_cb(Fl_Widget*, void*)
{
    lua_meshgen_unregister(L);

    lua_getglobal(L, "net");
    mesh_t mesh = (mesh_t) lua_touserdata(L,-1);
    lua_pop(L,1);

    vars_get_vartypes(mesh_vars_mgr(mesh));
    vars_assign      (mesh_vars_mgr(mesh));
    assemble_displace(mesh_assembler(mesh));
}

static Fl_Menu_Item menuitems[] = {
  { "&File",              0, 0, 0, FL_SUBMENU },

    { "&Open",            FL_CTRL + 'o', (Fl_Callback *) open_cb, 0 },
    { "&Load net",        FL_CTRL + 'l', (Fl_Callback *) load_cb, 0 },
    { "&Save File",       FL_CTRL + 's', (Fl_Callback *) save_cb, 0 },
    { "Save File &As...", FL_CTRL + FL_SHIFT + 's', 
       (Fl_Callback *)saveas_cb, 0, FL_MENU_DIVIDER }, 
    
    { "&Create netlist...", 0, (Fl_Callback *) new_net_cb, 0, 0 }, 
    { "&End netlist...",    0, (Fl_Callback *) end_net_cb, 0, 
                            FL_MENU_DIVIDER }, 
    

    { "E&xit", FL_CTRL + 'q', (Fl_Callback *) quit_cb, 0 },
    { 0 },

  { 0 }
};

EditorWindow::EditorWindow(int w, int h, const char* t) : 
    Fl_Double_Window(w, h, t) 
{
    begin();

      Fl_Menu_Bar* m = new Fl_Menu_Bar(0, 0, 512, 30);
      m->copy(menuitems, this);

      editor = new Fl_Text_Editor(0, 30, 512, 354);
      editor->buffer(textbuf);
      editor->textfont(FL_COURIER);

      drawbox = new DrawBox(0, 384, 512, 300);

    end();

    editor->add_key_binding(FL_Enter, FL_TEXT_EDITOR_ANY_STATE,
                               enter_key_cb);

    resizable(editor);
    callback((Fl_Callback *)quit_cb, this);

    textbuf->add_modify_callback(changed_cb, this);
    textbuf->call_modify_callbacks();
}

EditorWindow::~EditorWindow() 
{
}

int main(int argc, char **argv) 
{
    textbuf = new Fl_Text_Buffer;
    EditorWindow* window = 
        new EditorWindow(512, 684, "SUGAR interface window");

    window->show(1, argv);

    mempool_t pool = mempool_create(MEMPOOL_DEFAULT_SPAN);
    model_mgr      = model_mgr_init();
    uses_manager   = uses_manager_create(pool);

    xtnetdraw_init(argc, argv);
    model_test_register(model_mgr);
    model_mgr_standard_register(model_mgr);

    L = lua_open(0);

    lua_matrix_register(L);
    lua_superlu_register(L);
    lua_mesh_register(L);
    lua_usesmgrlib(L);
    lua_set_usesmgr(L, uses_manager);

    lua_baselibopen(L);
    lua_mathlibopen(L);
    lua_iolibopen(L);

    uses_manager_envpath(uses_manager, "SUGAR_LUA_PATH");
    
    lua_register(L, "_ALERT", lua_alert);
    
    lua_pushuserdata(L, window->editor);
    lua_pushcclosure(L, lua_print, 1);
    lua_setglobal(L, "print");
    
    lua_register(L, "cho_load", lua_netload);
    
    lua_pushuserdata(L, window->drawbox);
    lua_pushcclosure(L, lua_draw, 1);
    lua_setglobal(L, "cho_display");
    

    window->editor->insert("");
    return Fl::run();
}

