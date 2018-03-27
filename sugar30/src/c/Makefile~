#
# Makefile for SUGAR 3.0
#

include ../make.inc

# -----------------------------------------------------------------

OBJS = affine.o dynarray.o hash.o mempool.o uses-manager.o \
       mesh.o vars.o \
       assemblem.o assemblem-dense.o assemblem-sparse.o assemble.o \
       superlu.o newton.o \
       netdraw.o netdraw-xt.o netdraw-java.o \
       netout.o netout-file.o \
       modelmgr.o model-test.o model-anchor.o model-force.o \
       model-beam2d.o model-rigid.o model-gap.o \
       meshgen-lua.o uses-manager-lua.o

SUGARLIB  = sugarlib.a
SUGARLIBN = sugarlib.a

LUA_OBJS = sugar-lua.o mesh-lua.o matrix-lua.o superlu-lua.o

LUA_FLTK_OBJS = sugar-fltk.cc netdraw-fltk.cc \
	mesh-lua.o matrix-lua.o superlu-lua.o

MEX_SRCS = sugarmex.c mex-handle.c \
           mesh-mex.c vars-mex.c assemble-mex.c \
           netdraw-mex.c  netout-mex.c \
           netdraw-mlab.c netout-mlab.c model-matlab.c model-mf2wrap.c

ALL_SRCS = ${OBJS:.o=.c} ${LUA_OBJS:.o=.c} ${MEX_SRCS}

# -----------------------------------------------------------------

.SUFFIXES : .cc .hh .c .h .o

.c.o:
	$(CC) -c $(CFLAGS) $(CINCL) $<

# -----------------------------------------------------------------

# --- Main routines

sugar-lua: $(LUA_OBJS) $(SUGARLIB)
	$(CC) -o sugar-lua $(LUA_OBJS) $(SUGARLIB) $(CLIBS)

sugar-fltk: $(LUA_FLTK_OBJS) $(SUGARLIB) netdraw-fltk.hh
	g++ -o sugar-fltk $(LUA_FLTK_OBJS) $(SUGARLIB) -lfltk $(CLIBS)

mex: $(MEX_SRCS) ${MEX_SRCS:.c=.h} $(SUGARLIB)
	$(CMEX) $(MEXFLAGS) $(MEX_SRCS) $(SUGARLIB) $(MEXLIBS)

$(SUGARLIBN): $(OBJS)
	$(ARCH) $(ARCHFLAGS) $(SUGARLIBN) $(OBJS)
	$(RANLIB) $(SUGARLIBN)


# --- Assuming you're using lcc as your compiler, and you have cygwin,
#     and you've already compiled most of your stuff, this is how you
#     want to prepare the MEX file.

sugarlib.lib: $(OBJS)
	lcclib /OUT:sugarlib.lib *.obj

makemex.m:
	cp ../../tools/lua/lib/liblua.lib .
	cp ../../tools/lua/lib/liblualib.lib .
	echo "mex $(MEXFLAGS) ..."      >> makemex.m
	echo "  $(MEX_SRCS) ..." | gawk '{gsub(/-/, ""); print}' >> makemex.m
	echo "  sugarlib.lib ..."                      >> makemex.m
	echo "  liblua.lib    ..." >> makemex.m
	echo "  liblualib.lib"     >> makemex.m
	cp mex-handle.c mexhandle.c
	cp mesh-mex.c meshmex.c
	cp vars-mex.c varsmex.c
	cp assemble-mex.c assemblemex.c
	cp netdraw-mex.c netdrawmex.c
	cp netout-mex.c netoutmex.c
	cp netdraw-mlab.c netdrawmlab.c
	cp netout-mlab.c netoutmlab.c
	cp model-matlab.c modelmatlab.c
	cp model-mf2wrap.c modelmf2wrap.c


# --- Helpers

clean:
	rm -f *.o $(SUGARLIBN)

clean-all: clean
	rm -f sugar-lua sugar-fltk sugarmex.mex* 

count:
	wc -l *.nw


# Note: I do know and like makedepend.  But it seems to be broken under
#  RH 7.1 (no -Y option).  Sigh...

depend:
	# Step 1: Chop off everything after "Generated dependencies"
	#
	cat Makefile | $(AWK) '\
		{ if (!done) { print }; \
	  	  if (/^# --- Gen/) { done=1; print " " } }' \
		> Makefile.depend

	# Step 2: Get C source dependencies, then filter out any
	#	not in the current directory (omitting system headers
	#	is insufficient)
	#
	gcc -MG -MM $(CFLAGS) $(CINCL) ${OBJS:.o=.c} ${LUA_OBJS:.o=.c} \
                ${LUA_FLTK_OBJS:.o=.c} | \
		$(AWK) '{ gsub(/\/[^ ]*/, ""); print }' \
		>> Makefile.depend

	# Step 3: Save the current Makefile and swap in the new Makefile
	# 	in its place.
	#
	cp Makefile Makefile.bak
	mv Makefile.depend Makefile


# --- Generated dependencies
 
affine.o: affine.c affine.h
dynarray.o: dynarray.c dynarray.h
hash.o: hash.c mempool.h hash.h
mempool.o: mempool.c mempool.h
uses-manager.o: uses-manager.c uses-manager.h mempool.h
mesh.o: mesh.c mempool.h hash.h mesh.h modelmgr.h vars.h assemble.h \
  assemblem.h assemblem-sparse.h assemblem-dense.h superlu.h dynarray.h
vars.o: vars.c mempool.h dynarray.h mesh.h modelmgr.h vars.h
assemblem.o: assemblem.c modelmgr.h assemblem.h
assemblem-dense.o: assemblem-dense.c modelmgr.h assemblem-dense.h \
  assemblem.h
assemblem-sparse.o: assemblem-sparse.c dynarray.h assemblem-sparse.h \
  assemblem.h
assemble.o: assemble.c modelmgr.h assemble.h mempool.h mesh.h vars.h \
  assemblem.h assemblem-sparse.h assemblem-dense.h superlu.h
superlu.o: superlu.c  \
  superlu.h assemblem-sparse.h assemblem.h
netdraw.o: netdraw.c affine.h netdraw.h
netdraw-xt.o: netdraw-xt.c netdraw-xt.h mesh.h mempool.h modelmgr.h \
  netdraw.h affine.h
netdraw-java.o: netdraw-java.c netdraw-java.h mesh.h mempool.h modelmgr.h \
  netdraw.h
netout.o: netout.c netout.h
netout-file.o: netout-file.c netout.h netout-file.h mesh.h mempool.h \
  modelmgr.h
modelmgr.o: modelmgr.c mempool.h hash.h mesh.h modelmgr.h
model-test.o: model-test.c model-test.h modelmgr.h mesh.h mempool.h \
  netout.h
model-anchor.o: model-anchor.c model-anchor.h modelmgr.h mesh.h mempool.h \
  vars.h assemble.h assemblem.h assemblem-sparse.h assemblem-dense.h \
  superlu.h netout.h
model-force.o: model-force.c model-force.h modelmgr.h mesh.h mempool.h \
  vars.h assemble.h assemblem.h assemblem-sparse.h assemblem-dense.h \
  superlu.h netout.h
model-beam2d.o: model-beam2d.c model-beam2d.h modelmgr.h affine.h mesh.h \
  mempool.h vars.h assemble.h assemblem.h assemblem-sparse.h \
  assemblem-dense.h superlu.h netdraw.h netout.h
model-rigid.o: model-rigid.c model-rigid.h modelmgr.h affine.h mesh.h \
  mempool.h vars.h assemble.h assemblem.h assemblem-sparse.h \
  assemblem-dense.h superlu.h netdraw.h netout.h
model-gap.o: model-gap.c model-gap.h modelmgr.h
meshgen-lua.o: meshgen-lua.c meshgen-lua.h uses-manager.h mempool.h \
  mesh.h modelmgr.h assemble.h vars.h assemblem.h assemblem-sparse.h \
  assemblem-dense.h superlu.h
uses-manager-lua.o: uses-manager-lua.c uses-manager-lua.h uses-manager.h \
  mempool.h
sugar-lua.o: sugar-lua.c modelmgr.h model-test.h model-anchor.h \
  model-force.h model-beam2d.h model-rigid.h vars.h assemble.h mempool.h \
  mesh.h assemblem.h assemblem-sparse.h assemblem-dense.h superlu.h \
  uses-manager.h uses-manager-lua.h meshgen-lua.h mesh-lua.h netdraw-xt.h \
  netdraw-java.h matrix-lua.h superlu-lua.h
mesh-lua.o: mesh-lua.c meshgen-lua.h uses-manager.h mempool.h mesh.h \
  modelmgr.h mesh-lua.h netdraw-xt.h netdraw-java.h matrix-lua.h \
  superlu-lua.h superlu.h assemblem-sparse.h assemblem.h vars.h \
  assemble.h assemblem-dense.h netout.h netout-file.h
matrix-lua.o: matrix-lua.c matrix-lua.h
superlu-lua.o: superlu-lua.c superlu-lua.h superlu.h assemblem-sparse.h \
  assemblem.h matrix-lua.h
sugar-fltk.o: sugar-fltk.cc modelmgr.h model-test.h model-anchor.h \
  model-force.h model-beam2d.h model-rigid.h vars.h assemble.h mempool.h \
  mesh.h assemblem.h assemblem-sparse.h assemblem-dense.h superlu.h \
  uses-manager.h uses-manager-lua.h meshgen-lua.h mesh-lua.h netdraw-xt.h \
  netdraw-java.h matrix-lua.h superlu-lua.h netdraw-fltk.hh
netdraw-fltk.o: netdraw-fltk.cc netdraw.h mesh.h mempool.h modelmgr.h \
  affine.h netdraw-fltk.hh
mesh-lua.o: mesh-lua.c meshgen-lua.h uses-manager.h mempool.h mesh.h \
  modelmgr.h mesh-lua.h netdraw-xt.h netdraw-java.h matrix-lua.h \
  superlu-lua.h superlu.h assemblem-sparse.h assemblem.h vars.h \
  assemble.h assemblem-dense.h netout.h netout-file.h
matrix-lua.o: matrix-lua.c matrix-lua.h
superlu-lua.o: superlu-lua.c superlu-lua.h superlu.h assemblem-sparse.h \
  assemblem.h matrix-lua.h
