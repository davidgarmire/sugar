# -- Makefile for sugar converter 

MAKE=make

CC=gcc
CFLAGS=-g -Wall -DMEMPOOL_CLEAR_DEBUG -Wall -pedantic
CLIBS= -lm

# -----------------------------------------------------------------

OBJS = sugar_c.o \
       mempool.o symbol.o \
       sugar.tab.o lex.yy.o \
       parse.o 

# -----------------------------------------------------------------

standalone:
	$(MAKE) netcvt CMPL="$(CC)" CMPLFLAGS="$(CFLAGS)" LIBS="$(CLIBS)"

# -----------------------------------------------------------------

.SUFFIXES : .c

.c.o:
	$(CMPL) -c $(CMPLFLAGS) $<

netcvt: $(OBJS)
	$(CMPL) -o netcvt $(OBJS) $(LIBS)

sugar.tab.c: sugar.y
	bison -v -d sugar.y

lex.yy.c: sugar.lex
	flex sugar.lex
	cat lex.yy.c | awk '!/unistd.h/' > lex.yy-win.c

clean:
	rm -f *.o

clean-all:
	rm -f *.o sugar.tab.c sugar.tab.h sugar.output lex.yy.c \
              lex.yy-win.c netcvt 

sugar_c.o: sugar_c.c
mempool.o: mempool.c
parse.o: parse.c parse.h symbol.h
symbol.o: symbol.c symbol.h
sugar.tab.o: sugar.tab.c parse.h
lex.yy.o: lex.yy.c sugar.tab.c
lex.yy-win.o: lex.yy-win.c sugar.tab.c

