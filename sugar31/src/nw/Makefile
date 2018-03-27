#
# Makefile for SUGAR 3.0
#

# -----------------------------------------------------------------
DSBWEB=../../tools/dsbweb/dsbweb 

src:
	$(DSBWEB) -d../c/      -underscore .c .h .cc .hh *.nw
	$(DSBWEB) -d../matlab/ -underscore .m  *.nw
	$(DSBWEB) -d../lua/    .lua .net *.nw

clean:
	rm -f `$(DSBWEB) -list -d../c/      -underscore .c .h .cc .hh *.nw`
	rm -f `$(DSBWEB) -list -d../matlab/ -underscore .m *.nw`
	rm -f `$(DSBWEB) -list -d../lua/    .lua .net *.nw`

# -----------------------------------------------------------------
NOWEAVE=noweave
WEAVEOPTS=-delay -x

tex:
	for nwfile in *.nw; do \
		texfile=`echo $$nwfile | sed "s/[.]nw/.tex/"` ; \
		$(NOWEAVE) $(WEAVEOPTS) $$nwfile > $$texfile ; \
		mv $$texfile ../tex ; \
	done

clean-tex:
	for nwfile in *.nw; do \
		texfile=`echo $$nwfile | sed "s/[.]nw/.tex/"` ; \
		rm -f ../tex/$$texfile ; \
	done

