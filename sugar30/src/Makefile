
#
# Makefile for SUGAR 3.0
#

sugar-lua: cdeps
	(cd c; make sugar-lua)

sugar-fltk: cdeps
	(cd c; make sugar-fltk)

sugar-mex: cdeps
	(cd c; make mex; cp *.mex* ../matlab)

cdeps:
	(cd nw; make cdeps)

src:
	(cd nw; make)

clean:
	(cd c  ; make clean-all)
	(cd tex; make clean-all)

clean-all: clean
	(cd nw; make clean-all)

