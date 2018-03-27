#
# Makefile for SUGAR 3.0
#

all:
	(cd tools; make)
	(cd src;   make)

clean:
	(cd src;   make clean)

clean-all:
	(cd src;   make clean-all)
	(cd src;   make clean)
	(cd tools; make clean)

autoconf:
	autoconf
	autoheader

clean-config:
	rm -f make.inc config.h sugar.h config.log config.status
