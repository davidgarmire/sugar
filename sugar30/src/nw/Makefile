
#
# Makefile for SUGAR 3.0
#

include ../make.inc

# -----------------------------------------------------------------

WEAVEOPTS=-delay -x

WEAVEHDR=\\documentclass{article} \
         \\usepackage{noweb}      \
         \\usepackage{fullpage}   \
         \\usepackage{amsmath}    \
         \\begin{document}

WEAVEFTR=\\bibliographystyle{plain} \
         \\bibliography{../tex/main} \
         \\end{document}

# -----------------------------------------------------------------

.SUFFIXES : .cc .hh .c .h .nw .tex .pdf .o

.nw.tex:
	$(NOWEAVE) $(WEAVEOPTS) $< > $@
	cp $@ ../tex

.nw.c:
	$(NOTANGLE) $(TANGLEOPTS) -R$@ $< > $@
	cp $@ ../c

.nw.cc:
	$(NOTANGLE)$ $(TANGLEOPTS) -R$@ $< > $@
	cp $@ ../c

.nw.h:
	$(NOTANGLE) $(TANGLEOPTS) -R$@ $< > $@
	cp $@ ../c

.nw.hh:
	$(NOTANGLE) $(TANGLEOPTS) -R$@ $< > $@
	cp $@ ../c

.nw.pdf:
	echo    $(WEAVEHDR)     >  $*.tex
	$(NOWEAVE) $(WEAVEOPTS) $< >> $*.tex
	echo    $(WEAVEFTR)     >> $*.tex
	pdflatex $*.tex
	bibtex $*
	pdflatex $*.tex
	pdflatex $*.tex
	rm $*.tex

.c.o:
	$(CC) -c $(CFLAGS) $(CINCL) $<

# -----------------------------------------------------------------

all: tex c lua matlab

tex:
	make `ls *.nw | $(AWK) 'gsub(/[.]nw/, ".tex")'`
	mv *.tex ../tex

c: 
	$(AWK) '/<<[-a-zA-Z0-9]*[.][ch]/ { \
	   gsub(/<</,  ""); \
	   gsub(/>>=/, ""); \
	   srcname = $$0;   \
	   system("$(NOTANGLE) -R" srcname " " ARGV[ARGIND] \
                  " > ../$@/" srcname); \
	 }' *.nw

lua:
	$(AWK) '/<<[-a-zA-Z0-9]*[.](lua|net)/ { \
	   gsub(/<</,  ""); \
	   gsub(/>>=/, ""); \
	   srcname = $$0;   \
	   system("$(NOTANGLE) -R" srcname " " ARGV[ARGIND] \
                  " > ../$@/" srcname); \
	 }' *.nw

matlab:
	$(AWK) '/<<[-a-zA-Z0-9]*[.]m/ { \
	   gsub(/<</,  ""); \
	   gsub(/>>=/, ""); \
	   srcname = $$0;   \
	   gsub(/-/, "_");  \
           destname = $$0;  \
	   system("$(NOTANGLE) -R" srcname " " ARGV[ARGIND] \
                  " > ../$@/" destname); \
	 }' *.nw

# -----------------------------------------------------------------

clean-all: clean-tex clean-c clean-matlab clean-lua

clean-tex:
	rm -f `ls *.nw | $(AWK) '{gsub(/[.]nw/, ".tex"); print "../tex/" $$0}'`

clean-c:
	$(AWK) '/<<[-a-zA-Z0-9]*[.][ch]/ { \
	   gsub(/<</,  ""); \
	   gsub(/>>=/, ""); \
	   system("rm -f ../c/" $$0); \
	 }' *.nw

clean-matlab:
	$(AWK) '/<<[-a-zA-Z0-9]*[.]m/ { \
	   gsub(/<</,  ""); \
	   gsub(/>>=/, ""); \
	   gsub(/-/, "_"); \
	   system("rm -f ../matlab/" $$0); \
	 }' *.nw

clean-lua:
	$(AWK) '/<<[-a-zA-Z0-9]*[.](lua|net)/ { \
	   gsub(/<</,  ""); \
	   gsub(/>>=/, ""); \
	   system("rm -f ../lua/" $$0); \
	 }' *.nw

# -----------------------------------------------------------------

depend:
	# Step 1: Chop off everything after "Generated dependencies"
	#
	cat Makefile | $(AWK) '\
		{ if (!done) { print }; \
	  	  if (/^# --- Gen/) { done=1; print " " } }' \
		> Makefile.depend

	# Step 2: Generate dependencies of C sources on noweb files
	#
	$(AWK) '/<<[-a-zA-Z0-9]*[.][ch]/ { \
	   gsub(/<</,  ""); \
	   gsub(/>>=/, ""); \
	   srcname = $$0;   \
	   print "cdeps: ../c/" srcname "\n" \
	         "../c/" srcname ": " ARGV[ARGIND] "\n" \
	         "\t$$\(NOTANGLE\) $$\(TANGLEOPTS\) -R" srcname " $$< > $$@\n" \
	         >> "Makefile.depend" \
	 }' *.nw

	# Step 3: Save the current Makefile and swap in the new Makefile
	#         in its place
	cp Makefile Makefile.bak
	mv Makefile.depend Makefile

# --- Generated dependencies
 
cdeps: ../c/affine.h
../c/affine.h: affine.nw
	$(NOTANGLE) $(TANGLEOPTS) -Raffine.h $< > $@

cdeps: ../c/affine.c
../c/affine.c: affine.nw
	$(NOTANGLE) $(TANGLEOPTS) -Raffine.c $< > $@

cdeps: ../c/assemblem-dense.h
../c/assemblem-dense.h: assemblem-dense.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rassemblem-dense.h $< > $@

cdeps: ../c/assemblem-dense.c
../c/assemblem-dense.c: assemblem-dense.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rassemblem-dense.c $< > $@

cdeps: ../c/assemble-mex.h
../c/assemble-mex.h: assemble-mex.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rassemble-mex.h $< > $@

cdeps: ../c/assemble-mex.c
../c/assemble-mex.c: assemble-mex.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rassemble-mex.c $< > $@

cdeps: ../c/assemblem.h
../c/assemblem.h: assemblem.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rassemblem.h $< > $@

cdeps: ../c/assemblem.c
../c/assemblem.c: assemblem.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rassemblem.c $< > $@

cdeps: ../c/assemblem-sparse.h
../c/assemblem-sparse.h: assemblem-sparse.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rassemblem-sparse.h $< > $@

cdeps: ../c/assemblem-sparse.c
../c/assemblem-sparse.c: assemblem-sparse.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rassemblem-sparse.c $< > $@

cdeps: ../c/assemble.h
../c/assemble.h: assemble.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rassemble.h $< > $@

cdeps: ../c/assemble.c
../c/assemble.c: assemble.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rassemble.c $< > $@

cdeps: ../c/dynarray.h
../c/dynarray.h: dynarray.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rdynarray.h $< > $@

cdeps: ../c/dynarray.c
../c/dynarray.c: dynarray.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rdynarray.c $< > $@

cdeps: ../c/dynarray-test.c
../c/dynarray-test.c: dynarray.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rdynarray-test.c $< > $@

cdeps: ../c/hash.h
../c/hash.h: hash.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rhash.h $< > $@

cdeps: ../c/hash.c
../c/hash.c: hash.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rhash.c $< > $@

cdeps: ../c/wc.c
../c/wc.c: hash.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rwc.c $< > $@

cdeps: ../c/matrix-lua.h
../c/matrix-lua.h: matrix-lua.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmatrix-lua.h $< > $@

cdeps: ../c/matrix-lua.c
../c/matrix-lua.c: matrix-lua.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmatrix-lua.c $< > $@

cdeps: ../c/mempool.h
../c/mempool.h: mempool.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmempool.h $< > $@

cdeps: ../c/mempool.c
../c/mempool.c: mempool.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmempool.c $< > $@

cdeps: ../c/meshgen-lua.h
../c/meshgen-lua.h: meshgen-lua.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmeshgen-lua.h $< > $@

cdeps: ../c/meshgen-lua.c
../c/meshgen-lua.c: meshgen-lua.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmeshgen-lua.c $< > $@

cdeps: ../c/mesh-lua.h
../c/mesh-lua.h: mesh-lua.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmesh-lua.h $< > $@

cdeps: ../c/mesh-lua.c
../c/mesh-lua.c: mesh-lua.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmesh-lua.c $< > $@

cdeps: ../c/mesh-mex.h
../c/mesh-mex.h: mesh-mex.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmesh-mex.h $< > $@

cdeps: ../c/mesh-mex.c
../c/mesh-mex.c: mesh-mex.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmesh-mex.c $< > $@

cdeps: ../c/mesh.h
../c/mesh.h: mesh.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmesh.h $< > $@

cdeps: ../c/mesh.c
../c/mesh.c: mesh.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmesh.c $< > $@

cdeps: ../c/mex-handle.h
../c/mex-handle.h: mex-handle.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmex-handle.h $< > $@

cdeps: ../c/mex-handle.c
../c/mex-handle.c: mex-handle.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmex-handle.c $< > $@

cdeps: ../c/model-anchor.h
../c/model-anchor.h: model-anchor.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-anchor.h $< > $@

cdeps: ../c/model-anchor.c
../c/model-anchor.c: model-anchor.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-anchor.c $< > $@

cdeps: ../c/model-beam2d.h
../c/model-beam2d.h: model-beam2d.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-beam2d.h $< > $@

cdeps: ../c/model-beam2d.c
../c/model-beam2d.c: model-beam2d.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-beam2d.c $< > $@

cdeps: ../c/model-force.h
../c/model-force.h: model-force.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-force.h $< > $@

cdeps: ../c/model-force.c
../c/model-force.c: model-force.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-force.c $< > $@

cdeps: ../c/model-gap.h
../c/model-gap.h: model-gap.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-gap.h $< > $@

cdeps: ../c/model-gap.c
../c/model-gap.c: model-gap.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-gap.c $< > $@

cdeps: ../c/model-matlab.h
../c/model-matlab.h: model-matlab.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-matlab.h $< > $@

cdeps: ../c/model-matlab.c
../c/model-matlab.c: model-matlab.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-matlab.c $< > $@

cdeps: ../c/model-mf2wrap.h
../c/model-mf2wrap.h: model-mf2wrap.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-mf2wrap.h $< > $@

cdeps: ../c/model-mf2wrap.c
../c/model-mf2wrap.c: model-mf2wrap.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-mf2wrap.c $< > $@

cdeps: ../c/modelmgr.h
../c/modelmgr.h: modelmgr.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodelmgr.h $< > $@

cdeps: ../c/modelmgr.c
../c/modelmgr.c: modelmgr.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodelmgr.c $< > $@

cdeps: ../c/model-rigid.h
../c/model-rigid.h: model-rigid.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-rigid.h $< > $@

cdeps: ../c/model-rigid.c
../c/model-rigid.c: model-rigid.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-rigid.c $< > $@

cdeps: ../c/model-test.h
../c/model-test.h: model-test.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-test.h $< > $@

cdeps: ../c/model-test.c
../c/model-test.c: model-test.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rmodel-test.c $< > $@

cdeps: ../c/netdraw-fltk.hh
../c/netdraw-fltk.hh: netdraw-fltk.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetdraw-fltk.hh $< > $@

cdeps: ../c/netdraw-fltk.cc
../c/netdraw-fltk.cc: netdraw-fltk.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetdraw-fltk.cc $< > $@

cdeps: ../c/netdraw-java.h
../c/netdraw-java.h: netdraw-java.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetdraw-java.h $< > $@

cdeps: ../c/netdraw-java.c
../c/netdraw-java.c: netdraw-java.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetdraw-java.c $< > $@

cdeps: ../c/netdraw-mex.h
../c/netdraw-mex.h: netdraw-mex.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetdraw-mex.h $< > $@

cdeps: ../c/netdraw-mex.c
../c/netdraw-mex.c: netdraw-mex.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetdraw-mex.c $< > $@

cdeps: ../c/netdraw-mlab.h
../c/netdraw-mlab.h: netdraw-mlab.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetdraw-mlab.h $< > $@

cdeps: ../c/netdraw-mlab.c
../c/netdraw-mlab.c: netdraw-mlab.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetdraw-mlab.c $< > $@

cdeps: ../c/netdraw.h
../c/netdraw.h: netdraw.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetdraw.h $< > $@

cdeps: ../c/netdraw.c
../c/netdraw.c: netdraw.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetdraw.c $< > $@

cdeps: ../c/netdraw-xt.h
../c/netdraw-xt.h: netdraw-xt.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetdraw-xt.h $< > $@

cdeps: ../c/netdraw-xt.c
../c/netdraw-xt.c: netdraw-xt.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetdraw-xt.c $< > $@

cdeps: ../c/netout-file.h
../c/netout-file.h: netout-file.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetout-file.h $< > $@

cdeps: ../c/netout-file.c
../c/netout-file.c: netout-file.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetout-file.c $< > $@

cdeps: ../c/netout-mex.h
../c/netout-mex.h: netout-mex.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetout-mex.h $< > $@

cdeps: ../c/netout-mex.c
../c/netout-mex.c: netout-mex.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetout-mex.c $< > $@

cdeps: ../c/netout-mlab.h
../c/netout-mlab.h: netout-mlab.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetout-mlab.h $< > $@

cdeps: ../c/netout-mlab.c
../c/netout-mlab.c: netout-mlab.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetout-mlab.c $< > $@

cdeps: ../c/netout.h
../c/netout.h: netout.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetout.h $< > $@

cdeps: ../c/netout.c
../c/netout.c: netout.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rnetout.c $< > $@

cdeps: ../c/sugar-fltk.cc
../c/sugar-fltk.cc: sugar-fltk.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rsugar-fltk.cc $< > $@

cdeps: ../c/sugar-lua.c
../c/sugar-lua.c: sugar-lua.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rsugar-lua.c $< > $@

cdeps: ../c/sugarmex.h
../c/sugarmex.h: sugarmex.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rsugarmex.h $< > $@

cdeps: ../c/sugarmex.c
../c/sugarmex.c: sugarmex.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rsugarmex.c $< > $@

cdeps: ../c/superlu-lua.h
../c/superlu-lua.h: superlu-lua.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rsuperlu-lua.h $< > $@

cdeps: ../c/superlu-lua.c
../c/superlu-lua.c: superlu-lua.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rsuperlu-lua.c $< > $@

cdeps: ../c/superlu.h
../c/superlu.h: superlu.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rsuperlu.h $< > $@

cdeps: ../c/superlu.c
../c/superlu.c: superlu.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rsuperlu.c $< > $@

cdeps: ../c/uses-manager-lua.h
../c/uses-manager-lua.h: uses-manager-lua.nw
	$(NOTANGLE) $(TANGLEOPTS) -Ruses-manager-lua.h $< > $@

cdeps: ../c/uses-manager-lua.c
../c/uses-manager-lua.c: uses-manager-lua.nw
	$(NOTANGLE) $(TANGLEOPTS) -Ruses-manager-lua.c $< > $@

cdeps: ../c/uses-manager.h
../c/uses-manager.h: uses-manager.nw
	$(NOTANGLE) $(TANGLEOPTS) -Ruses-manager.h $< > $@

cdeps: ../c/uses-manager.c
../c/uses-manager.c: uses-manager.nw
	$(NOTANGLE) $(TANGLEOPTS) -Ruses-manager.c $< > $@

cdeps: ../c/vars-mex.h
../c/vars-mex.h: vars-mex.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rvars-mex.h $< > $@

cdeps: ../c/vars-mex.c
../c/vars-mex.c: vars-mex.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rvars-mex.c $< > $@

cdeps: ../c/vars.h
../c/vars.h: vars.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rvars.h $< > $@

cdeps: ../c/vars.c
../c/vars.c: vars.nw
	$(NOTANGLE) $(TANGLEOPTS) -Rvars.c $< > $@

