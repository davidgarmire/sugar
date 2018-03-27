#
# Makefile for SUGAR 3.0
#
include ../make.inc

all:
	(cd nw; make)
	(cd c;  make $(DEFAULT_PROGRAMS))

sugar-lua:
	(cd nw; make)
	(cd c;  make sugar-lua)

sugar-fltk:
	(cd nw; make)
	(cd c;  make sugar-fltk)

sugar-mex:
	(cd nw; make)
	(cd c; make mex; cp *.mex* ../matlab)

src:
	(cd nw; make)

doc: 
	(cd nw; make tex)
	(cd tex; make)

clean:
	(cd c  ; make clean)
	(cd tex; make clean-all)

clean-all: clean
	(cd c ; make clean-all)
	(cd nw; make clean)
	(cd nw; make clean-tex)
	rm -f matlab/*.mex*

