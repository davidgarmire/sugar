#
# Makefile for SUGAR 3.0
#

SUGARBASE=../..

include $(SUGARBASE)/make.inc

# -----------------------------------------------------------------

LUA_DIR =   ../../tools/lua
LUA_INC =  -I$(LUA_DIR)/include
LUA_LIBS =  $(LUA_DIR)/lib/liblua.a $(LUA_DIR)/lib/liblualib.a

CLIBS = $(LUA_LIBS) $(SUPERLU_LIB) \
        $(LAPACK_LIBS) $(BLAS_LIBS) $(FLIBS) \
        $(X_LIBS) $(READLINE_LIBS) $(LIBS)
       
CINCL   = -I. -I$(SUGARBASE) $(SUPERLU_DIR) $(LUA_INC) 
CXXINCL = $(CINCL)

MEXFLAGS = -g $(LUA_INC) $(SUPERLU_DIR) 
MEXLIBS  =  $(LUA_LIBS) $(SUPERLU_LIB) $(X_LIBS) $(FLTK_LIBS) 

# -----------------------------------------------------------------

OBJS = affine.o dynarray.o hash.o mempool.o uses_manager.o \
       mesh.o vars.o \
       assemblem.o assemblem_dense.o assemblem_sparse.o assemble.o \
       superlu.o newton.o \
       netdraw.o netdraw_xt.o netdraw_java.o \
       netout.o netout_file.o \
       modelmgr.o model_test.o model_anchor.o model_force.o \
       model_beam2d.o model_beam3d.o model_rigid.o model_gap.o \
       model_circuit.o \
       meshgen_lua.o uses_manager_lua.o

SUGARLIB  = sugarlib.a

LUA_OBJS = sugar_lua.o mesh_lua.o matrix_lua.o superlu_lua.o

LUA_FLTK_OBJS = sugar_fltk.cc netdraw_fltk.cc \
	mesh_lua.o matrix_lua.o superlu_lua.o

MEX_SRCS = sugarmex.c mex_handle.c \
           mesh_mex.c vars_mex.c assemble_mex.c \
           netdraw_mex.c  netout_mex.c \
           netdraw_mlab.c netout_mlab.c model_matlab.c model_mf2wrap.c

ALL_SRCS = ${OBJS:.o=.c} ${LUA_OBJS:.o=.c} ${MEX_SRCS}

# -----------------------------------------------------------------

.SUFFIXES : .cc .hh .c .h .o

.c.o:
	$(CC) -c $(CFLAGS) $(CINCL) $<

# -----------------------------------------------------------------

sugar-lua: $(LUA_OBJS) $(SUGARLIB)
	$(CC) -o sugar-lua $(LUA_OBJS) $(SUGARLIB) $(CLIBS)

sugar-fltk: $(LUA_FLTK_OBJS) $(SUGARLIB) netdraw_fltk.hh
	$(CXX) $(CXXFLAGS) $(FLTK_CXXFLAGS) -o sugar-fltk \
		$(LUA_FLTK_OBJS) $(SUGARLIB) $(FLTK_LIBS) $(CLIBS)

mex: $(MEX_SRCS) ${MEX_SRCS:.c=.h} $(SUGARLIB)
	$(MEX) $(CINCL) $(MEXFLAGS) $(MEX_SRCS) $(SUGARLIB) $(MEXLIBS)

$(SUGARLIB): $(OBJS)
	$(ARCH) $(ARCHFLAGS) $(SUGARLIB) $(OBJS)
	$(RANLIB) $(SUGARLIB)

clean:
	rm -f *.o $(SUGARLIB)

clean-all: clean
	rm -f sugar-lua sugar-fltk sugarmex.mex* 

depend:
	makedepend -Y -I. *.c >& /dev/null


# DO NOT DELETE

affine.o: affine.h
assemble.o: modelmgr.h assemble.h mempool.h mesh.h vars.h assemblem.h
assemble.o: assemblem_sparse.h assemblem_dense.h superlu.h
assemblem.o: modelmgr.h assemblem.h
assemblem_dense.o: modelmgr.h assemblem_dense.h assemblem.h
assemble_mex.o: mex_handle.h sugarmex.h mesh_mex.h mesh.h mempool.h
assemble_mex.o: modelmgr.h assemble_mex.h assemble.h vars.h assemblem.h
assemble_mex.o: assemblem_sparse.h assemblem_dense.h superlu.h
assemblem_sparse.o: dynarray.h assemblem_sparse.h assemblem.h
dynarray.o: dynarray.h
dynarray_test.o: dynarray.h
hash.o: mempool.h hash.h
matrix_lua.o: matrix_lua.h
mempool.o: mempool.h
mesh.o: mempool.h hash.h mesh.h modelmgr.h vars.h assemble.h assemblem.h
mesh.o: assemblem_sparse.h assemblem_dense.h superlu.h dynarray.h
meshgen_lua.o: meshgen_lua.h uses_manager.h mempool.h mesh.h modelmgr.h
meshgen_lua.o: assemble.h vars.h assemblem.h assemblem_sparse.h
meshgen_lua.o: assemblem_dense.h superlu.h
mesh_lua.o: meshgen_lua.h uses_manager.h mempool.h mesh.h modelmgr.h
mesh_lua.o: mesh_lua.h netdraw_xt.h netdraw_java.h matrix_lua.h superlu_lua.h
mesh_lua.o: superlu.h assemblem_sparse.h assemblem.h vars.h assemble.h
mesh_lua.o: assemblem_dense.h netout.h netout_file.h newton.h
mesh_mex.o: modelmgr.h model_test.h model_matlab.h model_mf2wrap.h
mesh_mex.o: uses_manager.h mempool.h netout.h netout_file.h mesh.h
mesh_mex.o: netout_mlab.h mex_handle.h sugarmex.h mesh_mex.h
mesh_mex.o: uses_manager_lua.h meshgen_lua.h
mex_handle.o: mex_handle.h dynarray.h
model_anchor.o: model_anchor.h modelmgr.h mesh.h mempool.h vars.h assemble.h
model_anchor.o: assemblem.h assemblem_sparse.h assemblem_dense.h superlu.h
model_anchor.o: netout.h
model_beam2d.o: model_beam2d.h modelmgr.h affine.h mesh.h mempool.h vars.h
model_beam2d.o: assemble.h assemblem.h assemblem_sparse.h assemblem_dense.h
model_beam2d.o: superlu.h netdraw.h netout.h
model_beam3d.o: model_beam3d.h modelmgr.h affine.h mesh.h mempool.h vars.h
model_beam3d.o: assemble.h assemblem.h assemblem_sparse.h assemblem_dense.h
model_beam3d.o: superlu.h netdraw.h netout.h
model_circuit.o: model_circuit.h modelmgr.h mesh.h mempool.h vars.h
model_circuit.o: assemble.h assemblem.h assemblem_sparse.h assemblem_dense.h
model_circuit.o: superlu.h netdraw.h netout.h
model_force.o: model_force.h modelmgr.h mesh.h mempool.h vars.h assemble.h
model_force.o: assemblem.h assemblem_sparse.h assemblem_dense.h superlu.h
model_force.o: netout.h
model_gap.o: model_gap.h modelmgr.h affine.h mesh.h mempool.h vars.h
model_gap.o: assemble.h assemblem.h assemblem_sparse.h assemblem_dense.h
model_gap.o: superlu.h netdraw.h netout.h
model_matlab.o: modelmgr.h mesh.h mempool.h vars.h assemble.h assemblem.h
model_matlab.o: assemblem_sparse.h assemblem_dense.h superlu.h netdraw.h
model_matlab.o: netout.h sugarmex.h mesh_mex.h vars_mex.h assemble_mex.h
model_matlab.o: netdraw_mex.h
model_mf2wrap.o: modelmgr.h mesh.h mempool.h vars.h assemble.h assemblem.h
model_mf2wrap.o: assemblem_sparse.h assemblem_dense.h superlu.h netdraw.h
model_mf2wrap.o: netout.h sugarmex.h mesh_mex.h vars_mex.h assemble_mex.h
model_mf2wrap.o: netdraw_mex.h
modelmgr.o: mempool.h hash.h mesh.h modelmgr.h model_anchor.h model_force.h
modelmgr.o: model_beam2d.h model_beam3d.h model_rigid.h model_gap.h
model_rigid.o: model_rigid.h modelmgr.h affine.h mesh.h mempool.h vars.h
model_rigid.o: assemble.h assemblem.h assemblem_sparse.h assemblem_dense.h
model_rigid.o: superlu.h netdraw.h netout.h
model_test.o: model_test.h modelmgr.h mesh.h mempool.h netout.h
netdraw.o: affine.h netdraw.h
netdraw_mex.o: mesh.h mempool.h modelmgr.h netdraw_xt.h netdraw_mlab.h
netdraw_mex.o: netdraw.h assemble.h vars.h assemblem.h assemblem_sparse.h
netdraw_mex.o: assemblem_dense.h superlu.h mex_handle.h sugarmex.h
netdraw_mex.o: netdraw_mex.h mesh_mex.h
netdraw_mlab.o: mesh.h mempool.h modelmgr.h vars.h assemble.h assemblem.h
netdraw_mlab.o: assemblem_sparse.h assemblem_dense.h superlu.h affine.h
netdraw_mlab.o: sugarmex.h netdraw_mlab.h netdraw.h
netout.o: netout.h
netout_file.o: netout.h netout_file.h mesh.h mempool.h modelmgr.h
netout_mex.o: sugarmex.h mex_handle.h netout_mex.h netout.h
netout_mlab.o: mesh.h mempool.h modelmgr.h netout.h netout_mlab.h
newton.o: mesh.h mempool.h modelmgr.h assemble.h vars.h assemblem.h
newton.o: assemblem_sparse.h assemblem_dense.h superlu.h
sugar_lua.o: modelmgr.h model_test.h vars.h assemble.h mempool.h mesh.h
sugar_lua.o: assemblem.h assemblem_sparse.h assemblem_dense.h superlu.h
sugar_lua.o: uses_manager.h uses_manager_lua.h meshgen_lua.h mesh_lua.h
sugar_lua.o: netdraw_xt.h netdraw_java.h matrix_lua.h superlu_lua.h
sugarmex.o: dynarray.h sugarmex.h mex_handle.h mesh_mex.h mesh.h mempool.h
sugarmex.o: modelmgr.h vars_mex.h vars.h assemble_mex.h assemble.h
sugarmex.o: assemblem.h assemblem_sparse.h assemblem_dense.h superlu.h
sugarmex.o: netdraw_mex.h netdraw.h netout_mex.h netout.h buildversion.h
superlu_lua.o: superlu_lua.h superlu.h assemblem_sparse.h assemblem.h
superlu_lua.o: matrix_lua.h
uses_manager.o: uses_manager.h mempool.h
uses_manager_lua.o: uses_manager_lua.h uses_manager.h mempool.h
vars.o: mempool.h dynarray.h mesh.h modelmgr.h vars.h
vars_mex.o: mex_handle.h sugarmex.h mesh_mex.h mesh.h mempool.h modelmgr.h
vars_mex.o: vars_mex.h vars.h assemble.h assemblem.h assemblem_sparse.h
vars_mex.o: assemblem_dense.h superlu.h
wc.o: hash.h mempool.h
