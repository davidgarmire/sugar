# 
# Makefile constants for SUGAR 3.0
#

CXX       = g++
CXXFLAGS  = -I. -g -O2

CC        =  gcc
CFLAGS    = -I. -g -O2

MEX       = /cygdrive/c/matlab6p5/bin/win32/mex.bat

ARCH      = /cygdrive/c/mingw/bin/ar
ARCHFLAGS = rc
RANLIB    = ranlib 

LIBS          = -lm 
READLINE_LIBS = -lreadline
BLAS_LIBS     = 
LAPACK_LIBS   = 
FLIBS         =  -Lc:/mingw/bin/../lib/gcc-lib/mingw32/3.2 -Lc:/mingw/bin/../lib/gcc-lib -L/mingw/lib/gcc-lib/mingw32/3.2 -Lc:/mingw/bin/../lib/gcc-lib/mingw32/3.2/../../../../mingw32/lib -L/mingw/lib/gcc-lib/mingw32/3.2/../../../../mingw32/lib -Lc:/mingw/bin/../lib/gcc-lib/mingw32/3.2/../../.. -L/mingw/lib/gcc-lib/mingw32/3.2/../../.. -lfrtbegin -lg2c -lmingw32 -lmoldname -lmingwex -lmsvcrt -luser32 -lkernel32 -ladvapi32 -lshell32

X_CFLAGS  = 
X_LIBS    = 

SUPERLU_DIR = 
SUPERLU_LIB = 

FLTK_CONFIG   = 
FLTK_CXXFLAGS = 
FLTK_LIBS     = 

DEFAULT_PROGRAMS = sugar-lua  mex

#XDRDEF= -DHAS_XDR
#XDRLIB= -lnsl

