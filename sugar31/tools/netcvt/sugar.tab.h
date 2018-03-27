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


extern YYSTYPE yylval;
