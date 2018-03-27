%{
#include "lexer.h"
#include "sugar.tab.h"
#include <string.h>
#include <stdlib.h>

static int parse_lineno;
static int parse_fileno;
static mempool_t parse_pool;
static int uses_stack_count;

static void push_use_file(char* filename);
static void pop_use_file();
static double suffix_val(char c);

%}

%x COMMENT
%x INCL

inttok      [0-9]+
floattok    [0-9]+(\.[0-9]+)?(e[-+]?[0-9]+)?
numext      [afpnumcdhkMGTPE]
strtok1     \"[^\"\n]*[\"\n]
strtok2     \'[^\'\n]*[\'\n]

%%

\%              { BEGIN COMMENT;        }
<COMMENT>\n     { ++parse_lineno; BEGIN 0;    }
<COMMENT>.      ;


"call"          { return CALL;    }
"subnet"        { return SUBNET;  }
"param"         { return PARAM;   }
"process"       { return PROCESS; }
"for"           { return FOR;     }
"if"            { return IF;      }
"else"          { return ELSE;    }

"!"            { return NOT;      }
"~"            { return NOT;      }
"|"            { return OR;       }
"&"            { return AND;      }
"=="           { return EQ;       }
"!="           { return NEQ;      }
"~="           { return NEQ;      }
"<="           { return LEQ;      }
">="           { return GEQ;      }
"<"            { return LT;       }
">"            { return GT;       }

"uses"          { BEGIN INCL;           }
<INCL>[ \r\t]*  ;
<INCL>\n        { ++parse_lineno; }
<INCL>[^ \t\r\n]+ { 
    push_use_file(yytext);
    BEGIN INITIAL;
}

<<EOF>> {
    if (uses_stack_count <= 0)
        yyterminate();
    else {
        --uses_stack_count;
        pop_use_file();
    }
}


[a-zA-Z][a-zA-Z0-9_]*    { yylval.id = mempool_strdup(parse_pool, yytext);
                           return ID;      }
{inttok}            { yylval.ival.val = atoi(yytext);
                      yylval.ival.tok = mempool_strdup(parse_pool, yytext);
                      return INT;     }
{floattok}          { yylval.dval.val = atof(yytext);
                      yylval.dval.tok = mempool_strdup(parse_pool, yytext);
                      return FLOAT;   }
{floattok}{numext}  { yylval.dval.val = atof(yytext) *
                          suffix_val(yytext[strlen(yytext)-1]);
                      yylval.dval.tok = mempool_strdup(parse_pool, yytext);
                      return FLOAT;   }
{strtok1}           { /* Skip open quote */
                      yylval.sval = mempool_strdup(parse_pool, yytext+1); 
                      if (yylval.sval[yyleng-2] != '"') {
                          yytext[yyleng-1] = '\0';
                          yyerror("Unterminated character string");
                      } else
                          yylval.sval[yyleng-2] = '\0';
                      return STRING;   }
{strtok2}           { /* Skip open quote */
                      yylval.sval = mempool_strdup(parse_pool, yytext+1); 
                      if (yylval.sval[yyleng-2] != '\'') {
                          yytext[yyleng-1] = '\0';
                          yyerror("Unterminated character string");
                      } else
                          yylval.sval[yyleng-2] = '\0';
                      return STRING;   }


\n            { ++parse_lineno; }
[ \t\r]       ;
.             { return yytext[0]; }

%%

/* Count of errors encountered in processing this netlist
 */
#define MAX_REPORTED_ERRORS 10
static int error_count;
static int has_fatal_errors;

/* List of files already used (prevents multiple uses) 
 */
#define MAX_USES 40
static char* uses_list[MAX_USES];
static int total_uses;


/* Stack of currently included files 
 */
#define MAX_INCLUDE_DEPTH 10
struct uses_stack_node {
    int parse_lineno;
    int parse_fileno;
    YY_BUFFER_STATE buf;
};

static struct uses_stack_node uses_stack[MAX_INCLUDE_DEPTH];
static struct uses_stack_node* uses_stack_ptr = uses_stack;


/* Initialize local scanner state 
 */
void lex_init(mempool_t pool)
{
    total_uses = 0;
    uses_stack_count = 0;
    uses_stack_ptr = uses_stack;

    error_count = 0;
    has_fatal_errors = 0;
    parse_lineno = 1;
    parse_fileno = 0;

    parse_pool = pool;
}

/* Clear local scanner state
 */
void lex_clear()
{
    while (uses_stack_count > 0) {
        --uses_stack_count;
        pop_use_file();
    }
}

/* Search Matlab path for files
 */
FILE* which_file(char* filename)
{
    FILE* fp = fopen(filename, "r");
    return fp;
}

/* Request to use file filename, if it has not already been used. 
 */
static void push_use_file(char* filename)
{
    int i;
    int already_used = 0;

    /* Look up the actual file name in Matlab's path */
    /* filename = which_file(filename); */

    /* Find whether we've seen this file in another uses statement */
    for (i = 0; i < total_uses && !already_used; ++i) {
        if (strcmp(filename, uses_list[i]) == 0)
            already_used = 1;
    }

    /* If not, go ahead and use it */
    if (!already_used) {
        FILE* used_file = which_file(filename); 
        if (!used_file) {
            yyerror("Could not open file for include");
        } else if (uses_stack_count >= MAX_INCLUDE_DEPTH) {
            yyerror("uses nested too deeply");
            fclose(used_file);
        } else {
            uses_stack_ptr->parse_lineno = parse_lineno;
            uses_stack_ptr->parse_fileno = parse_fileno;
            uses_stack_ptr->buf = YY_CURRENT_BUFFER;

            ++uses_stack_count;
            ++uses_stack_ptr;
            parse_lineno = 1;
            uses_list[total_uses++] = mempool_strdup(parse_pool, yytext);
            parse_fileno = total_uses;

            yyin = used_file;
            yy_switch_to_buffer(yy_create_buffer(yyin, YY_BUF_SIZE));
        }
    }
}

/* Pop a file off the uses stack and restore state of previous read
 */
static void pop_use_file()
{
    yy_delete_buffer(YY_CURRENT_BUFFER);
    fclose(yyin);

    --uses_stack_ptr;
    yy_switch_to_buffer(uses_stack_ptr->buf);
    parse_lineno = uses_stack_ptr->parse_lineno;
    parse_fileno = uses_stack_ptr->parse_fileno;
}

/* Print the list of files used
 */
void print_files_used(FILE* fp)
{
    int i;
    for (i = 0; i < total_uses; ++i) {
        fprintf(fp, "%s\n", uses_list[i]);
    }
}

/* Get the position of the current token
 */
void get_parse_position(int* parse_lineno_out, int* parse_fileno_out)
{
    *parse_lineno_out = parse_lineno;
    *parse_fileno_out = parse_fileno;
}

/* Report an error during parsing.
 */
int yyerror(char* errmsg) 
{
    extern char* yytext;
    return add_error(parse_fileno, parse_lineno, yytext, errmsg, 0);
}

/* Report up to MAX_REPORTED_ERRORS errors (then just indicate that there
 * were more errors)
 */
int add_error(int parse_fileno, int parse_lineno, char* token, char* errmsg,
              int is_warning)
{
    has_fatal_errors |= !is_warning;
    if (error_count++ <= MAX_REPORTED_ERRORS) {
        if (is_warning)
            printf("Warning ");
        else
            printf("Error ");
        printf("(");
        if (parse_fileno > 0)
            printf("%s, ", uses_list[parse_fileno-1]);
        printf("line %d", parse_lineno);
        if (token != NULL) 
            printf(" at %s", token);
        printf("): %s\n", errmsg);
    } 
    if (error_count == MAX_REPORTED_ERRORS+1)
        printf("Error: Stopped reporting after %d errors\n", 
               MAX_REPORTED_ERRORS);
    return 0;
}

/* Report up to MAX_REPORTED_ERRORS errors (then just indicate that there
 * were more errors)
 */
int add_error_stack(int parse_fileno, int parse_lineno, char* stack_name)
{
    if (error_count++ <= MAX_REPORTED_ERRORS) {
        printf("\tIn call to %s (", stack_name);
        if (parse_fileno > 0)
            printf("%s, ", uses_list[parse_fileno-1]);
        printf("line %d)\n", parse_lineno);
    } 
    return 0;
}

/* Return whether there have been any errors
 */
int no_errors()
{
    return (has_fatal_errors == 0);
}

/* Convert a metric suffix to the corresponding multiplier
 */
static double suffix_val(char c)
{
    switch (c) {
    case 'a': return 1e-18;     /* Atto     */
    case 'f': return 1e-15;     /* Femto    */
    case 'p': return 1e-12;     /* Pico     */
    case 'n': return 1e-9;      /* Nano     */
    case 'u': return 1e-6;      /* Micro    */
    case 'm': return 1e-3;      /* Milli    */
    case 'c': return 1e-2;      /* Centi    */
    case 'd': return 1e-1;      /* Deci     */
    case 'h': return 1e2;       /* Hecto    */
    case 'k': return 1e3;       /* Kilo     */
    case 'M': return 1e6;       /* Mega     */
    case 'G': return 1e9;       /* Giga     */
    case 'T': return 1e12;      /* Tera     */
    case 'P': return 1e15;      /* Peta     */
    case 'E': return 1e18;      /* Exa      */
    default:
       yyerror("Undefined value suffix");
       return 1e0;
    }
}
