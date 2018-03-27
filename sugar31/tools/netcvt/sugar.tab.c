
/*  A Bison parser, made from sugar.y
    by GNU Bison version 1.28  */

#define YYBISON 1  /* Identify Bison output.  */

#define	IF	257
#define	ELSE	258
#define	OR	259
#define	AND	260
#define	EQ	261
#define	NEQ	262
#define	GT	263
#define	LT	264
#define	GEQ	265
#define	LEQ	266
#define	NOT	267
#define	CALL	268
#define	SUBNET	269
#define	PARAM	270
#define	PROCESS	271
#define	FOR	272
#define	ID	273
#define	INT	274
#define	FLOAT	275
#define	STRING	276
#define	UMINUS	277

#line 1 "sugar.y"


#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parse.h"
#include "lexer.h"

extern int yywrap();
extern int yylex();

static code_t* netcode;
static code_t* precode;


#line 18 "sugar.y"
typedef union {
    char* id;
    struct {
        int val;
        char* tok;
    } ival;
    struct {
        double val;
        char* tok;
    } dval;
    char* sval;
    struct code_struct* code;
} YYSTYPE;
#include <stdio.h>

#ifndef __cplusplus
#ifndef __STDC__
#define const
#endif
#endif



#define	YYFINAL		147
#define	YYFLAG		-32768
#define	YYNTBASE	37

#define YYTRANSLATE(x) ((unsigned)(x) <= 277 ? yytranslate[x] : 67)

static const char yytranslate[] = {     0,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,    35,
    36,    25,    24,    29,    23,     2,    26,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,    31,     2,     2,
    30,     2,    34,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
    32,     2,    33,    28,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
     2,     2,     2,     2,     2,     1,     3,     4,     5,     6,
     7,     8,     9,    10,    11,    12,    13,    14,    15,    16,
    17,    18,    19,    20,    21,    22,    27
};

#if YYDEBUG != 0
static const short yyprhs[] = {     0,
     0,     2,     5,     7,     9,    11,    13,    15,    18,    20,
    24,    26,    28,    35,    44,    47,    56,    58,    61,    62,
    64,    68,    72,    75,    76,    78,    80,    82,    84,    86,
    88,    92,    94,    96,   104,   111,   115,   122,   126,   129,
   132,   135,   142,   146,   152,   156,   159,   160,   163,   164,
   166,   171,   175,   179,   183,   187,   191,   194,   197,   201,
   205,   209,   213,   217,   221,   225,   229,   233,   235,   237,
   239,   241,   246,   252,   256,   258,   260
};

static const short yyrhs[] = {    38,
     0,    38,    39,     0,    39,     0,    40,     0,    43,     0,
    44,     0,    50,     0,    16,    41,     0,    42,     0,    41,
    29,    42,     0,    59,     0,    19,     0,    17,    19,    30,
    52,    60,    53,     0,    17,    19,    31,    19,    30,    52,
    60,    53,     0,    45,    51,     0,    46,    19,    32,    61,
    33,    32,    47,    33,     0,    15,     0,    47,    48,     0,
     0,    59,     0,    19,    30,    25,     0,    19,    30,    34,
     0,    49,    50,     0,     0,    51,     0,    59,     0,    54,
     0,    56,     0,    58,     0,    64,     0,    52,    49,    53,
     0,    32,     0,    33,     0,    55,    32,    61,    33,    52,
    60,    53,     0,    19,    35,    65,    36,    19,    19,     0,
    19,    19,    19,     0,    19,    35,    65,    36,    19,    25,
     0,    19,    19,    25,     0,    19,    19,     0,    19,    25,
     0,    57,    51,     0,    18,    19,    30,    63,    31,    63,
     0,     3,    63,    50,     0,     3,    63,    50,     4,    50,
     0,    19,    30,    63,     0,    60,    59,     0,     0,    61,
    62,     0,     0,    19,     0,    19,    35,    65,    36,     0,
    63,    24,    63,     0,    63,    23,    63,     0,    63,    25,
    63,     0,    63,    26,    63,     0,    63,    28,    63,     0,
    23,    63,     0,    13,    63,     0,    63,     6,    63,     0,
    63,     5,    63,     0,    63,     7,    63,     0,    63,     8,
    63,     0,    63,     9,    63,     0,    63,    10,    63,     0,
    63,    11,    63,     0,    63,    12,    63,     0,    35,    63,
    36,     0,    20,     0,    21,     0,    22,     0,    19,     0,
    19,    35,    66,    36,     0,    14,    19,    35,    66,    36,
     0,    65,    29,    63,     0,    63,     0,    65,     0,     0
};

#endif

#if YYDEBUG != 0
static const short yyrline[] = { 0,
    63,    67,    70,    72,    74,    75,    76,    78,    82,    84,
    87,    91,    95,    98,   101,   106,   110,   114,   117,   120,
   122,   124,   127,   130,   133,   135,   136,   137,   138,   139,
   141,   145,   148,   151,   155,   158,   160,   162,   164,   166,
   169,   173,   177,   180,   183,   187,   190,   193,   196,   199,
   202,   205,   208,   210,   212,   214,   216,   218,   220,   222,
   224,   226,   228,   230,   232,   234,   236,   238,   240,   242,
   244,   246,   249,   253,   256,   258,   260
};
#endif


#if YYDEBUG != 0 || defined (YYERROR_VERBOSE)

static const char * const yytname[] = {   "$","error","$undefined.","IF","ELSE",
"OR","AND","EQ","NEQ","GT","LT","GEQ","LEQ","NOT","CALL","SUBNET","PARAM","PROCESS",
"FOR","ID","INT","FLOAT","STRING","'-'","'+'","'*'","'/'","UMINUS","'^'","','",
"'='","':'","'['","']'","'?'","'('","')'","netlist","netlistDefs","netlistDef",
"paramLine","paramList","param","processLine","subnet","subnetHead","subnetKey",
"subnetParams","subnetParam","codeLines","codeLine","codeBlock","blockStart",
"blockEnd","elementLine","elementHead","forLine","forHead","ifLine","def","defs",
"names","name","expr","callNode","exprs","optexprs", NULL
};
#endif

static const short yyr1[] = {     0,
    37,    38,    38,    39,    39,    39,    39,    40,    41,    41,
    42,    42,    43,    43,    44,    45,    46,    47,    47,    48,
    48,    48,    49,    49,    50,    50,    50,    50,    50,    50,
    51,    52,    53,    54,    55,    55,    55,    55,    55,    55,
    56,    57,    58,    58,    59,    60,    60,    61,    61,    62,
    62,    63,    63,    63,    63,    63,    63,    63,    63,    63,
    63,    63,    63,    63,    63,    63,    63,    63,    63,    63,
    63,    63,    64,    65,    65,    66,    66
};

static const short yyr2[] = {     0,
     1,     2,     1,     1,     1,     1,     1,     2,     1,     3,
     1,     1,     6,     8,     2,     8,     1,     2,     0,     1,
     3,     3,     2,     0,     1,     1,     1,     1,     1,     1,
     3,     1,     1,     7,     6,     3,     6,     3,     2,     2,
     2,     6,     3,     5,     3,     2,     0,     2,     0,     1,
     4,     3,     3,     3,     3,     3,     2,     2,     3,     3,
     3,     3,     3,     3,     3,     3,     3,     1,     1,     1,
     1,     4,     5,     3,     1,     1,     0
};

static const short yydefact[] = {     0,
     0,     0,    17,     0,     0,     0,     0,    32,     1,     3,
     4,     5,     6,     0,     0,     7,    25,    24,    27,     0,
    28,     0,    29,    26,    30,     0,    71,    68,    69,    70,
     0,     0,     0,     0,    12,     8,     9,    11,     0,     0,
    39,    40,     0,     0,     2,    15,     0,     0,    49,    41,
    58,    77,    57,     0,     0,     0,     0,     0,     0,     0,
     0,     0,     0,     0,     0,     0,     0,    43,    77,     0,
     0,     0,     0,    36,    38,    45,    75,     0,    49,    33,
    23,    31,     0,    76,     0,    67,    60,    59,    61,    62,
    63,    64,    65,    66,    53,    52,    54,    55,    56,     0,
     0,    10,    47,     0,     0,     0,     0,     0,    50,     0,
    48,    72,    44,    73,     0,     0,     0,    74,     0,     0,
     0,    47,     0,    13,    46,    47,    42,    35,    37,    19,
     0,     0,     0,     0,    51,    34,    14,     0,    16,    18,
    20,     0,    21,    22,     0,     0,     0
};

static const short yydefgoto[] = {   145,
     9,    10,    11,    36,    37,    12,    13,    14,    15,   134,
   140,    48,    16,    17,    18,    82,    19,    20,    21,    22,
    23,    24,   115,    83,   111,    77,    25,    84,    85
};

static const short yypact[] = {    74,
   205,    20,-32768,    21,    33,    52,   -13,-32768,    74,-32768,
-32768,-32768,-32768,    43,    57,-32768,-32768,-32768,-32768,    46,
-32768,    43,-32768,-32768,-32768,   205,    59,-32768,-32768,-32768,
   205,   205,   114,    60,    66,    68,-32768,-32768,   -20,    72,
   -12,-32768,   205,   205,-32768,-32768,    76,     2,-32768,-32768,
    79,   205,    79,    75,   205,   205,   205,   205,   205,   205,
   205,   205,   205,   205,   205,   205,   205,   100,   205,    21,
    43,    90,   205,-32768,-32768,   151,   151,     9,-32768,-32768,
-32768,-32768,    -5,    81,    77,-32768,   174,   222,   164,   164,
   164,   164,   164,   164,    25,    25,    79,    79,-32768,     5,
    78,-32768,-32768,    85,   142,   205,    99,    -4,    92,    43,
-32768,-32768,-32768,-32768,    13,    43,   205,   151,    49,   102,
   205,-32768,    66,-32768,-32768,-32768,   151,-32768,-32768,-32768,
    12,    13,    13,    14,-32768,-32768,-32768,   105,-32768,-32768,
-32768,   188,-32768,-32768,   136,   143,-32768
};

static const short yypgoto[] = {-32768,
-32768,   135,-32768,-32768,    94,-32768,-32768,-32768,-32768,-32768,
-32768,-32768,   -30,    22,   -67,  -106,-32768,-32768,-32768,-32768,
-32768,    -3,   -53,    93,-32768,    -1,-32768,   -42,    86
};


#define	YYLAST		250


static const short yytable[] = {    33,
    38,    78,    68,   103,     1,    41,    74,     1,   124,    71,
    72,    42,    75,   109,   109,     2,    43,    81,     2,     6,
     7,    44,     6,     7,    51,   136,   137,   110,   120,    53,
    54,   123,   138,     8,    80,    46,     8,   106,    34,    35,
   106,    76,   122,    50,   107,    80,   139,   135,   126,    65,
    66,    39,    67,    87,    88,    89,    90,    91,    92,    93,
    94,    95,    96,    97,    98,    99,    38,   128,   132,   113,
    40,   105,   133,   129,     8,    47,     1,    49,   131,    55,
    56,    57,    58,    59,    60,    61,    62,     2,     3,     4,
     5,     6,     7,    52,    69,    43,    70,    63,    64,    65,
    66,    73,    67,   100,   118,     8,    67,    79,   104,   106,
    86,   125,   112,   114,   116,   127,     1,   119,    55,    56,
    57,    58,    59,    60,    61,    62,   121,     2,   125,   125,
   141,     6,     7,   130,   142,   146,    63,    64,    65,    66,
    76,    67,   147,    45,     0,     8,    55,    56,    57,    58,
    59,    60,    61,    62,   101,    55,    56,    57,    58,    59,
    60,    61,    62,   102,    63,    64,    65,    66,     0,    67,
     0,   108,   117,    63,    64,    65,    66,     0,    67,    56,
    57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
     0,    67,     0,     0,     0,     0,    63,    64,    65,    66,
    26,    67,     0,     0,     0,     0,    27,    28,    29,    30,
    31,     0,   143,     0,     0,     0,     0,    26,     0,     0,
     0,   144,    32,    27,    28,    29,    30,    31,    57,    58,
    59,    60,    61,    62,     0,     0,     0,     0,     0,    32,
     0,     0,     0,     0,    63,    64,    65,    66,     0,    67
};

static const short yycheck[] = {     1,
     4,    44,    33,    71,     3,    19,    19,     3,   115,    30,
    31,    25,    25,    19,    19,    14,    30,    48,    14,    18,
    19,    35,    18,    19,    26,   132,   133,    33,    33,    31,
    32,    19,    19,    32,    33,    14,    32,    29,    19,    19,
    29,    43,   110,    22,    36,    33,    33,    36,   116,    25,
    26,    19,    28,    55,    56,    57,    58,    59,    60,    61,
    62,    63,    64,    65,    66,    67,    70,    19,   122,   100,
    19,    73,   126,    25,    32,    19,     3,    32,   121,     5,
     6,     7,     8,     9,    10,    11,    12,    14,    15,    16,
    17,    18,    19,    35,    35,    30,    29,    23,    24,    25,
    26,    30,    28,     4,   106,    32,    28,    32,    19,    29,
    36,   115,    36,    36,    30,   117,     3,    19,     5,     6,
     7,     8,     9,    10,    11,    12,    35,    14,   132,   133,
   134,    18,    19,    32,    30,     0,    23,    24,    25,    26,
   142,    28,     0,     9,    -1,    32,     5,     6,     7,     8,
     9,    10,    11,    12,    69,     5,     6,     7,     8,     9,
    10,    11,    12,    70,    23,    24,    25,    26,    -1,    28,
    -1,    79,    31,    23,    24,    25,    26,    -1,    28,     6,
     7,     8,     9,    10,    11,    12,    23,    24,    25,    26,
    -1,    28,    -1,    -1,    -1,    -1,    23,    24,    25,    26,
    13,    28,    -1,    -1,    -1,    -1,    19,    20,    21,    22,
    23,    -1,    25,    -1,    -1,    -1,    -1,    13,    -1,    -1,
    -1,    34,    35,    19,    20,    21,    22,    23,     7,     8,
     9,    10,    11,    12,    -1,    -1,    -1,    -1,    -1,    35,
    -1,    -1,    -1,    -1,    23,    24,    25,    26,    -1,    28
};
/* -*-C-*-  Note some compilers choke on comments on `#line' lines.  */
#line 3 "/usr/lib/bison.simple"
/* This file comes from bison-1.28.  */

/* Skeleton output parser for bison,
   Copyright (C) 1984, 1989, 1990 Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

/* This is the parser code that is written into each bison parser
  when the %semantic_parser declaration is not specified in the grammar.
  It was written by Richard Stallman by simplifying the hairy parser
  used when %semantic_parser is specified.  */

#ifndef YYSTACK_USE_ALLOCA
#ifdef alloca
#define YYSTACK_USE_ALLOCA
#else /* alloca not defined */
#ifdef __GNUC__
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#else /* not GNU C.  */
#if (!defined (__STDC__) && defined (sparc)) || defined (__sparc__) || defined (__sparc) || defined (__sgi) || (defined (__sun) && defined (__i386))
#define YYSTACK_USE_ALLOCA
#include <alloca.h>
#else /* not sparc */
/* We think this test detects Watcom and Microsoft C.  */
/* This used to test MSDOS, but that is a bad idea
   since that symbol is in the user namespace.  */
#if (defined (_MSDOS) || defined (_MSDOS_)) && !defined (__TURBOC__)
#if 0 /* No need for malloc.h, which pollutes the namespace;
	 instead, just don't use alloca.  */
#include <malloc.h>
#endif
#else /* not MSDOS, or __TURBOC__ */
#if defined(_AIX)
/* I don't know what this was needed for, but it pollutes the namespace.
   So I turned it off.   rms, 2 May 1997.  */
/* #include <malloc.h>  */
 #pragma alloca
#define YYSTACK_USE_ALLOCA
#else /* not MSDOS, or __TURBOC__, or _AIX */
#if 0
#ifdef __hpux /* haible@ilog.fr says this works for HPUX 9.05 and up,
		 and on HPUX 10.  Eventually we can turn this on.  */
#define YYSTACK_USE_ALLOCA
#define alloca __builtin_alloca
#endif /* __hpux */
#endif
#endif /* not _AIX */
#endif /* not MSDOS, or __TURBOC__ */
#endif /* not sparc */
#endif /* not GNU C */
#endif /* alloca not defined */
#endif /* YYSTACK_USE_ALLOCA not defined */

#ifdef YYSTACK_USE_ALLOCA
#define YYSTACK_ALLOC alloca
#else
#define YYSTACK_ALLOC malloc
#endif

/* Note: there must be only one dollar sign in this file.
   It is replaced by the list of actions, each action
   as one case of the switch.  */

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		-2
#define YYEOF		0
#define YYACCEPT	goto yyacceptlab
#define YYABORT 	goto yyabortlab
#define YYERROR		goto yyerrlab1
/* Like YYERROR except do call yyerror.
   This remains here temporarily to ease the
   transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */
#define YYFAIL		goto yyerrlab
#define YYRECOVERING()  (!!yyerrstatus)
#define YYBACKUP(token, value) \
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    { yychar = (token), yylval = (value);			\
      yychar1 = YYTRANSLATE (yychar);				\
      YYPOPSTACK;						\
      goto yybackup;						\
    }								\
  else								\
    { yyerror ("syntax error: cannot back up"); YYERROR; }	\
while (0)

#define YYTERROR	1
#define YYERRCODE	256

#ifndef YYPURE
#define YYLEX		yylex()
#endif

#ifdef YYPURE
#ifdef YYLSP_NEEDED
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, &yylloc, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval, &yylloc)
#endif
#else /* not YYLSP_NEEDED */
#ifdef YYLEX_PARAM
#define YYLEX		yylex(&yylval, YYLEX_PARAM)
#else
#define YYLEX		yylex(&yylval)
#endif
#endif /* not YYLSP_NEEDED */
#endif

/* If nonreentrant, generate the variables here */

#ifndef YYPURE

int	yychar;			/*  the lookahead symbol		*/
YYSTYPE	yylval;			/*  the semantic value of the		*/
				/*  lookahead symbol			*/

#ifdef YYLSP_NEEDED
YYLTYPE yylloc;			/*  location data for the lookahead	*/
				/*  symbol				*/
#endif

int yynerrs;			/*  number of parse errors so far       */
#endif  /* not YYPURE */

#if YYDEBUG != 0
int yydebug;			/*  nonzero means print parse trace	*/
/* Since this is uninitialized, it does not stop multiple parsers
   from coexisting.  */
#endif

/*  YYINITDEPTH indicates the initial size of the parser's stacks	*/

#ifndef	YYINITDEPTH
#define YYINITDEPTH 200
#endif

/*  YYMAXDEPTH is the maximum size the stacks can grow to
    (effective only if the built-in stack extension method is used).  */

#if YYMAXDEPTH == 0
#undef YYMAXDEPTH
#endif

#ifndef YYMAXDEPTH
#define YYMAXDEPTH 10000
#endif

/* Define __yy_memcpy.  Note that the size argument
   should be passed with type unsigned int, because that is what the non-GCC
   definitions require.  With GCC, __builtin_memcpy takes an arg
   of type size_t, but it can handle unsigned int.  */

#if __GNUC__ > 1		/* GNU C and GNU C++ define this.  */
#define __yy_memcpy(TO,FROM,COUNT)	__builtin_memcpy(TO,FROM,COUNT)
#else				/* not GNU C or C++ */
#ifndef __cplusplus

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (to, from, count)
     char *to;
     char *from;
     unsigned int count;
{
  register char *f = from;
  register char *t = to;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#else /* __cplusplus */

/* This is the most reliable way to avoid incompatibilities
   in available built-in functions on various systems.  */
static void
__yy_memcpy (char *to, char *from, unsigned int count)
{
  register char *t = to;
  register char *f = from;
  register int i = count;

  while (i-- > 0)
    *t++ = *f++;
}

#endif
#endif

#line 217 "/usr/lib/bison.simple"

/* The user can define YYPARSE_PARAM as the name of an argument to be passed
   into yyparse.  The argument should have type void *.
   It should actually point to an object.
   Grammar actions can access the variable by casting it
   to the proper pointer type.  */

#ifdef YYPARSE_PARAM
#ifdef __cplusplus
#define YYPARSE_PARAM_ARG void *YYPARSE_PARAM
#define YYPARSE_PARAM_DECL
#else /* not __cplusplus */
#define YYPARSE_PARAM_ARG YYPARSE_PARAM
#define YYPARSE_PARAM_DECL void *YYPARSE_PARAM;
#endif /* not __cplusplus */
#else /* not YYPARSE_PARAM */
#define YYPARSE_PARAM_ARG
#define YYPARSE_PARAM_DECL
#endif /* not YYPARSE_PARAM */

/* Prevent warning if -Wstrict-prototypes.  */
#ifdef __GNUC__
#ifdef YYPARSE_PARAM
int yyparse (void *);
#else
int yyparse (void);
#endif
#endif

int
yyparse(YYPARSE_PARAM_ARG)
     YYPARSE_PARAM_DECL
{
  register int yystate;
  register int yyn;
  register short *yyssp;
  register YYSTYPE *yyvsp;
  int yyerrstatus;	/*  number of tokens to shift before error messages enabled */
  int yychar1 = 0;		/*  lookahead token as an internal (translated) token number */

  short	yyssa[YYINITDEPTH];	/*  the state stack			*/
  YYSTYPE yyvsa[YYINITDEPTH];	/*  the semantic value stack		*/

  short *yyss = yyssa;		/*  refer to the stacks thru separate pointers */
  YYSTYPE *yyvs = yyvsa;	/*  to allow yyoverflow to reallocate them elsewhere */

#ifdef YYLSP_NEEDED
  YYLTYPE yylsa[YYINITDEPTH];	/*  the location stack			*/
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;

#define YYPOPSTACK   (yyvsp--, yyssp--, yylsp--)
#else
#define YYPOPSTACK   (yyvsp--, yyssp--)
#endif

  int yystacksize = YYINITDEPTH;
  int yyfree_stacks = 0;

#ifdef YYPURE
  int yychar;
  YYSTYPE yylval;
  int yynerrs;
#ifdef YYLSP_NEEDED
  YYLTYPE yylloc;
#endif
#endif

  YYSTYPE yyval;		/*  the variable used to return		*/
				/*  semantic values from the action	*/
				/*  routines				*/

  int yylen;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Starting parse\n");
#endif

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss - 1;
  yyvsp = yyvs;
#ifdef YYLSP_NEEDED
  yylsp = yyls;
#endif

/* Push a new state, which is found in  yystate  .  */
/* In all cases, when you get here, the value and location stacks
   have just been pushed. so pushing a state here evens the stacks.  */
yynewstate:

  *++yyssp = yystate;

  if (yyssp >= yyss + yystacksize - 1)
    {
      /* Give user a chance to reallocate the stack */
      /* Use copies of these so that the &'s don't force the real ones into memory. */
      YYSTYPE *yyvs1 = yyvs;
      short *yyss1 = yyss;
#ifdef YYLSP_NEEDED
      YYLTYPE *yyls1 = yyls;
#endif

      /* Get the current used size of the three stacks, in elements.  */
      int size = yyssp - yyss + 1;

#ifdef yyoverflow
      /* Each stack pointer address is followed by the size of
	 the data in use in that stack, in bytes.  */
#ifdef YYLSP_NEEDED
      /* This used to be a conditional around just the two extra args,
	 but that might be undefined if yyoverflow is a macro.  */
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yyls1, size * sizeof (*yylsp),
		 &yystacksize);
#else
      yyoverflow("parser stack overflow",
		 &yyss1, size * sizeof (*yyssp),
		 &yyvs1, size * sizeof (*yyvsp),
		 &yystacksize);
#endif

      yyss = yyss1; yyvs = yyvs1;
#ifdef YYLSP_NEEDED
      yyls = yyls1;
#endif
#else /* no yyoverflow */
      /* Extend the stack our own way.  */
      if (yystacksize >= YYMAXDEPTH)
	{
	  yyerror("parser stack overflow");
	  if (yyfree_stacks)
	    {
	      free (yyss);
	      free (yyvs);
#ifdef YYLSP_NEEDED
	      free (yyls);
#endif
	    }
	  return 2;
	}
      yystacksize *= 2;
      if (yystacksize > YYMAXDEPTH)
	yystacksize = YYMAXDEPTH;
#ifndef YYSTACK_USE_ALLOCA
      yyfree_stacks = 1;
#endif
      yyss = (short *) YYSTACK_ALLOC (yystacksize * sizeof (*yyssp));
      __yy_memcpy ((char *)yyss, (char *)yyss1,
		   size * (unsigned int) sizeof (*yyssp));
      yyvs = (YYSTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yyvsp));
      __yy_memcpy ((char *)yyvs, (char *)yyvs1,
		   size * (unsigned int) sizeof (*yyvsp));
#ifdef YYLSP_NEEDED
      yyls = (YYLTYPE *) YYSTACK_ALLOC (yystacksize * sizeof (*yylsp));
      __yy_memcpy ((char *)yyls, (char *)yyls1,
		   size * (unsigned int) sizeof (*yylsp));
#endif
#endif /* no yyoverflow */

      yyssp = yyss + size - 1;
      yyvsp = yyvs + size - 1;
#ifdef YYLSP_NEEDED
      yylsp = yyls + size - 1;
#endif

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Stack size increased to %d\n", yystacksize);
#endif

      if (yyssp >= yyss + yystacksize - 1)
	YYABORT;
    }

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Entering state %d\n", yystate);
#endif

  goto yybackup;
 yybackup:

/* Do appropriate processing given the current state.  */
/* Read a lookahead token if we need one and don't already have one.  */
/* yyresume: */

  /* First try to decide what to do without reference to lookahead token.  */

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* yychar is either YYEMPTY or YYEOF
     or a valid token in external form.  */

  if (yychar == YYEMPTY)
    {
#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Reading a token: ");
#endif
      yychar = YYLEX;
    }

  /* Convert token to internal form (in yychar1) for indexing tables with */

  if (yychar <= 0)		/* This means end of input. */
    {
      yychar1 = 0;
      yychar = YYEOF;		/* Don't call YYLEX any more */

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Now at end of input.\n");
#endif
    }
  else
    {
      yychar1 = YYTRANSLATE(yychar);

#if YYDEBUG != 0
      if (yydebug)
	{
	  fprintf (stderr, "Next token is %d (%s", yychar, yytname[yychar1]);
	  /* Give the individual parser a way to print the precise meaning
	     of a token, for further debugging info.  */
#ifdef YYPRINT
	  YYPRINT (stderr, yychar, yylval);
#endif
	  fprintf (stderr, ")\n");
	}
#endif
    }

  yyn += yychar1;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != yychar1)
    goto yydefault;

  yyn = yytable[yyn];

  /* yyn is what to do for this token type in this state.
     Negative => reduce, -yyn is rule number.
     Positive => shift, yyn is new state.
       New state is final state => don't bother to shift,
       just return success.
     0, or most negative number => error.  */

  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrlab;

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Shift the lookahead token.  */

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting token %d (%s), ", yychar, yytname[yychar1]);
#endif

  /* Discard the token being shifted unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  /* count tokens shifted since error; after three, turn off error status.  */
  if (yyerrstatus) yyerrstatus--;

  yystate = yyn;
  goto yynewstate;

/* Do the default action for the current state.  */
yydefault:

  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;

/* Do a reduction.  yyn is the number of a rule to reduce with.  */
yyreduce:
  yylen = yyr2[yyn];
  if (yylen > 0)
    yyval = yyvsp[1-yylen]; /* implement default value of the action */

#if YYDEBUG != 0
  if (yydebug)
    {
      int i;

      fprintf (stderr, "Reducing via rule %d (line %d), ",
	       yyn, yyrline[yyn]);

      /* Print the symbols being reduced, and their result.  */
      for (i = yyprhs[yyn]; yyrhs[i] > 0; i++)
	fprintf (stderr, "%s ", yytname[yyrhs[i]]);
      fprintf (stderr, " -> %s\n", yytname[yyr1[yyn]]);
    }
#endif


  switch (yyn) {

case 1:
#line 65 "sugar.y"
{ netcode = cat_nodes(precode, yyvsp[0].code); ;
    break;}
case 2:
#line 69 "sugar.y"
{ yyval.code = cat_nodes(yyvsp[-1].code, yyvsp[0].code); ;
    break;}
case 8:
#line 80 "sugar.y"
{ yyval.code = yyvsp[0].code; ;
    break;}
case 10:
#line 85 "sugar.y"
{ yyval.code = cat_nodes(yyvsp[-2].code, yyvsp[0].code); ;
    break;}
case 11:
#line 89 "sugar.y"
{ yyval.code = yyvsp[0].code;
         yyval.code->tag = PARAM_NODE; ;
    break;}
case 12:
#line 92 "sugar.y"
{ yyval.code = def_node(yyvsp[0].id, NULL);
         yyval.code->tag = PARAM_NODE; ;
    break;}
case 13:
#line 97 "sugar.y"
{ yyval.code = process_node(yyvsp[-4].id, yyvsp[-1].code, NULL); ;
    break;}
case 14:
#line 99 "sugar.y"
{ yyval.code = process_node(yyvsp[-6].id, yyvsp[-1].code, yyvsp[-4].id); ;
    break;}
case 15:
#line 103 "sugar.y"
{ yyval.code = subnet_body(yyvsp[-1].code, yyvsp[0].code);
         end_subnet();  ;
    break;}
case 16:
#line 108 "sugar.y"
{ yyval.code = subnet_node(yyvsp[-6].id, yyvsp[-4].code, yyvsp[-1].code); ;
    break;}
case 17:
#line 112 "sugar.y"
{ start_subnet(); ;
    break;}
case 18:
#line 116 "sugar.y"
{ yyval.code = cat_nodes(yyvsp[-1].code, yyvsp[0].code); ;
    break;}
case 19:
#line 118 "sugar.y"
{ yyval.code = NULL; ;
    break;}
case 21:
#line 123 "sugar.y"
{ yyval.code = def_node(yyvsp[-2].id, NULL); ;
    break;}
case 22:
#line 125 "sugar.y"
{ yyval.code = def_node(yyvsp[-2].id, undef_node()); ;
    break;}
case 23:
#line 129 "sugar.y"
{ yyval.code = cat_nodes(yyvsp[-1].code, yyvsp[0].code); ;
    break;}
case 24:
#line 131 "sugar.y"
{ yyval.code = NULL ;
    break;}
case 31:
#line 143 "sugar.y"
{ yyval.code = block_node(yyvsp[-1].code); ;
    break;}
case 32:
#line 146 "sugar.y"
{ push_level(); ;
    break;}
case 33:
#line 149 "sugar.y"
{ pop_level(); ;
    break;}
case 34:
#line 153 "sugar.y"
{ yyval.code = elt_node_finish(yyvsp[-6].code, yyvsp[-4].code, yyvsp[-1].code); ;
    break;}
case 35:
#line 157 "sugar.y"
{ yyval.code = elt_node_head(yyvsp[-5].id, yyvsp[-3].code, yyvsp[-1].id, lookup_process(yyvsp[0].id)); ;
    break;}
case 36:
#line 159 "sugar.y"
{ yyval.code = elt_node_head(yyvsp[-2].id, NULL, yyvsp[-1].id, lookup_process(yyvsp[0].id)); ;
    break;}
case 37:
#line 161 "sugar.y"
{ yyval.code = elt_node_head(yyvsp[-5].id, yyvsp[-3].code, yyvsp[-1].id, null_process); ;
    break;}
case 38:
#line 163 "sugar.y"
{ yyval.code = elt_node_head(yyvsp[-2].id, NULL, yyvsp[-1].id, null_process); ;
    break;}
case 39:
#line 165 "sugar.y"
{ yyval.code = elt_node_head(NULL, NULL, yyvsp[-1].id, lookup_process(yyvsp[0].id)); ;
    break;}
case 40:
#line 167 "sugar.y"
{ yyval.code = elt_node_head(NULL, NULL, yyvsp[-1].id, null_process); ;
    break;}
case 41:
#line 171 "sugar.y"
{ yyval.code = for_node_body(yyvsp[-1].code, yyvsp[0].code); ;
    break;}
case 42:
#line 175 "sugar.y"
{ yyval.code = for_node(yyvsp[-4].id, yyvsp[-2].code, yyvsp[0].code); ;
    break;}
case 43:
#line 179 "sugar.y"
{ yyval.code = if_node(yyvsp[-1].code, yyvsp[0].code, NULL); ;
    break;}
case 44:
#line 181 "sugar.y"
{ yyval.code = if_node(yyvsp[-3].code, yyvsp[-2].code, yyvsp[0].code); ;
    break;}
case 45:
#line 185 "sugar.y"
{ yyval.code = def_node(yyvsp[-2].id, yyvsp[0].code); ;
    break;}
case 46:
#line 189 "sugar.y"
{ yyval.code = cat_nodes(yyvsp[-1].code, yyvsp[0].code); ;
    break;}
case 47:
#line 191 "sugar.y"
{ yyval.code = NULL; ;
    break;}
case 48:
#line 195 "sugar.y"
{ yyval.code = cat_nodes(yyvsp[-1].code, yyvsp[0].code); ;
    break;}
case 49:
#line 197 "sugar.y"
{ yyval.code = NULL; ;
    break;}
case 50:
#line 201 "sugar.y"
{ yyval.code = name_node(yyvsp[0].id, NULL); ;
    break;}
case 51:
#line 203 "sugar.y"
{ yyval.code = name_node(yyvsp[-3].id, yyvsp[-1].code); ;
    break;}
case 52:
#line 207 "sugar.y"
{ yyval.code = call_node("+", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 53:
#line 209 "sugar.y"
{ yyval.code = call_node("-", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 54:
#line 211 "sugar.y"
{ yyval.code = call_node("*", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 55:
#line 213 "sugar.y"
{ yyval.code = call_node("/", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 56:
#line 215 "sugar.y"
{ yyval.code = call_node("^", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 57:
#line 217 "sugar.y"
{ yyval.code = call_node("unary-", yyvsp[0].code); ;
    break;}
case 58:
#line 219 "sugar.y"
{ yyval.code = call_node("!", yyvsp[0].code); ;
    break;}
case 59:
#line 221 "sugar.y"
{ yyval.code = call_node("&", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 60:
#line 223 "sugar.y"
{ yyval.code = call_node("|", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 61:
#line 225 "sugar.y"
{ yyval.code = call_node("==", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 62:
#line 227 "sugar.y"
{ yyval.code = call_node("!=", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 63:
#line 229 "sugar.y"
{ yyval.code = call_node(">", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 64:
#line 231 "sugar.y"
{ yyval.code = call_node("<", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 65:
#line 233 "sugar.y"
{ yyval.code = call_node(">=", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 66:
#line 235 "sugar.y"
{ yyval.code = call_node("<=", cat_nodes(yyvsp[-2].code, yyvsp[0].code)); ;
    break;}
case 67:
#line 237 "sugar.y"
{ yyval.code = yyvsp[-1].code; ;
    break;}
case 68:
#line 239 "sugar.y"
{ yyval.code = float_node(yyvsp[0].ival.val, yyvsp[0].ival.tok); ;
    break;}
case 69:
#line 241 "sugar.y"
{ yyval.code = float_node(yyvsp[0].dval.val, yyvsp[0].dval.tok); ;
    break;}
case 70:
#line 243 "sugar.y"
{ yyval.code = string_node(yyvsp[0].sval); ;
    break;}
case 71:
#line 245 "sugar.y"
{ yyval.code = var_node(yyvsp[0].id); ;
    break;}
case 72:
#line 247 "sugar.y"
{ yyval.code = call_node(yyvsp[-3].id, yyvsp[-1].code); ;
    break;}
case 73:
#line 251 "sugar.y"
{ yyval.code = call_node(yyvsp[-3].id, yyvsp[-1].code); ;
    break;}
case 74:
#line 255 "sugar.y"
{ yyval.code = cat_nodes(yyvsp[-2].code, yyvsp[0].code); ;
    break;}
case 77:
#line 261 "sugar.y"
{ yyval.code = NULL; ;
    break;}
}
   /* the action file gets copied in in place of this dollarsign */
#line 543 "/usr/lib/bison.simple"

  yyvsp -= yylen;
  yyssp -= yylen;
#ifdef YYLSP_NEEDED
  yylsp -= yylen;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

  *++yyvsp = yyval;

#ifdef YYLSP_NEEDED
  yylsp++;
  if (yylen == 0)
    {
      yylsp->first_line = yylloc.first_line;
      yylsp->first_column = yylloc.first_column;
      yylsp->last_line = (yylsp-1)->last_line;
      yylsp->last_column = (yylsp-1)->last_column;
      yylsp->text = 0;
    }
  else
    {
      yylsp->last_line = (yylsp+yylen-1)->last_line;
      yylsp->last_column = (yylsp+yylen-1)->last_column;
    }
#endif

  /* Now "shift" the result of the reduction.
     Determine what state that goes to,
     based on the state we popped back to
     and the rule number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTBASE] + *yyssp;
  if (yystate >= 0 && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTBASE];

  goto yynewstate;

yyerrlab:   /* here on detecting error */

  if (! yyerrstatus)
    /* If not already recovering from an error, report this error.  */
    {
      ++yynerrs;

#ifdef YYERROR_VERBOSE
      yyn = yypact[yystate];

      if (yyn > YYFLAG && yyn < YYLAST)
	{
	  int size = 0;
	  char *msg;
	  int x, count;

	  count = 0;
	  /* Start X at -yyn if nec to avoid negative indexes in yycheck.  */
	  for (x = (yyn < 0 ? -yyn : 0);
	       x < (sizeof(yytname) / sizeof(char *)); x++)
	    if (yycheck[x + yyn] == x)
	      size += strlen(yytname[x]) + 15, count++;
	  msg = (char *) malloc(size + 15);
	  if (msg != 0)
	    {
	      strcpy(msg, "parse error");

	      if (count < 5)
		{
		  count = 0;
		  for (x = (yyn < 0 ? -yyn : 0);
		       x < (sizeof(yytname) / sizeof(char *)); x++)
		    if (yycheck[x + yyn] == x)
		      {
			strcat(msg, count == 0 ? ", expecting `" : " or `");
			strcat(msg, yytname[x]);
			strcat(msg, "'");
			count++;
		      }
		}
	      yyerror(msg);
	      free(msg);
	    }
	  else
	    yyerror ("parse error; also virtual memory exceeded");
	}
      else
#endif /* YYERROR_VERBOSE */
	yyerror("parse error");
    }

  goto yyerrlab1;
yyerrlab1:   /* here on error raised explicitly by an action */

  if (yyerrstatus == 3)
    {
      /* if just tried and failed to reuse lookahead token after an error, discard it.  */

      /* return failure if at end of input */
      if (yychar == YYEOF)
	YYABORT;

#if YYDEBUG != 0
      if (yydebug)
	fprintf(stderr, "Discarding token %d (%s).\n", yychar, yytname[yychar1]);
#endif

      yychar = YYEMPTY;
    }

  /* Else will try to reuse lookahead token
     after shifting the error token.  */

  yyerrstatus = 3;		/* Each real token shifted decrements this */

  goto yyerrhandle;

yyerrdefault:  /* current state does not do anything special for the error token. */

#if 0
  /* This is wrong; only states that explicitly want error tokens
     should shift them.  */
  yyn = yydefact[yystate];  /* If its default is to accept any token, ok.  Otherwise pop it.*/
  if (yyn) goto yydefault;
#endif

yyerrpop:   /* pop the current state because it cannot handle the error token */

  if (yyssp == yyss) YYABORT;
  yyvsp--;
  yystate = *--yyssp;
#ifdef YYLSP_NEEDED
  yylsp--;
#endif

#if YYDEBUG != 0
  if (yydebug)
    {
      short *ssp1 = yyss - 1;
      fprintf (stderr, "Error: state stack now");
      while (ssp1 != yyssp)
	fprintf (stderr, " %d", *++ssp1);
      fprintf (stderr, "\n");
    }
#endif

yyerrhandle:

  yyn = yypact[yystate];
  if (yyn == YYFLAG)
    goto yyerrdefault;

  yyn += YYTERROR;
  if (yyn < 0 || yyn > YYLAST || yycheck[yyn] != YYTERROR)
    goto yyerrdefault;

  yyn = yytable[yyn];
  if (yyn < 0)
    {
      if (yyn == YYFLAG)
	goto yyerrpop;
      yyn = -yyn;
      goto yyreduce;
    }
  else if (yyn == 0)
    goto yyerrpop;

  if (yyn == YYFINAL)
    YYACCEPT;

#if YYDEBUG != 0
  if (yydebug)
    fprintf(stderr, "Shifting error token, ");
#endif

  *++yyvsp = yylval;
#ifdef YYLSP_NEEDED
  *++yylsp = yylloc;
#endif

  yystate = yyn;
  goto yynewstate;

 yyacceptlab:
  /* YYACCEPT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 0;

 yyabortlab:
  /* YYABORT comes here.  */
  if (yyfree_stacks)
    {
      free (yyss);
      free (yyvs);
#ifdef YYLSP_NEEDED
      free (yyls);
#endif
    }
  return 1;
}
#line 263 "sugar.y"


code_t* sugar_parse(mempool_t pool)
{
    lex_init(pool);
    parse_init(pool);
    precode = get_precode();
    yyparse();
    lex_clear();
    if (no_errors())
        return netcode;
    else
        return NULL;
}

int yywrap()
{
    return 1;
}
