%{

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "parse.h"
#include "lexer.h"

extern int yywrap();
extern int yylex();

static code_t* netcode;
static code_t* precode;

%}

%union {
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
};

%token IF ELSE
%token OR AND EQ NEQ GT LT GEQ LEQ NOT
%token CALL SUBNET PARAM PROCESS FOR
%token ID
%token INT
%token FLOAT
%token STRING

%left OR
%left AND
%left EQ NEQ GT LT GEQ LEQ
%left '-' '+'
%left '*' '/'
%left UMINUS NOT
%left '^'

%type<id> ID
%type<ival> INT
%type<dval> FLOAT
%type<sval> STRING
%type<code> expr exprs optexprs callNode
%type<code> def defs param paramList subnetParam subnetParams
%type<code> elementHead elementLine
%type<code> subnet subnetHead
%type<code> name names
%type<code> forLine forHead ifLine
%type<code> codeBlock codeLine codeLines netlistDef netlistDefs 
%type<code> paramLine processLine 

%%

netlist: 
      netlistDefs 
       { netcode = cat_nodes(precode, $1); } ;

netlistDefs: 
      netlistDefs netlistDef
       { $$ = cat_nodes($1, $2); }
    | netlistDef ;

netlistDef: 
      paramLine
    | processLine
    | subnet
    | codeLine ;

paramLine: 
      PARAM paramList 
       { $$ = $2; } ;

paramList: 
      param 
    | paramList ',' param 
       { $$ = cat_nodes($1, $3); } ;

param: 
      def
       { $$ = $1;
         $$->tag = PARAM_NODE; }
    | ID 
       { $$ = def_node($1, NULL);
         $$->tag = PARAM_NODE; } ;

processLine: 
      PROCESS ID '=' blockStart defs blockEnd
       { $$ = process_node($2, $5, NULL); }
    | PROCESS ID ':' ID '=' blockStart defs blockEnd
       { $$ = process_node($2, $7, $4); }

subnet: 
      subnetHead codeBlock
       { $$ = subnet_body($1, $2);
         end_subnet();  };

subnetHead:    
      subnetKey ID '[' names ']' '[' subnetParams ']'
       { $$ = subnet_node($2, $4, $7); } ;

subnetKey:
      SUBNET 
       { start_subnet(); } ;

subnetParams: 
      subnetParams subnetParam
       { $$ = cat_nodes($1, $2); }
    | 
       { $$ = NULL; } ;

subnetParam: 
      def
    | ID '=' '*'
       { $$ = def_node($1, NULL); } 
    | ID '=' '?'
       { $$ = def_node($1, undef_node()); } ;

codeLines: 
      codeLines codeLine
       { $$ = cat_nodes($1, $2); }
    | 
       { $$ = NULL } ;

codeLine: 
      codeBlock
    | def
    | elementLine
    | forLine 
    | ifLine 
    | callNode;

codeBlock: 
      blockStart codeLines blockEnd
      { $$ = block_node($2); } ;

blockStart:
      '[' { push_level(); }

blockEnd:
      ']' { pop_level(); }

elementLine: 
      elementHead '[' names ']' blockStart defs blockEnd
       { $$ = elt_node_finish($1, $3, $6); } ;

elementHead:
      ID '(' exprs ')' ID ID
       { $$ = elt_node_head($1, $3, $5, lookup_process($6)); }
    | ID ID ID
       { $$ = elt_node_head($1, NULL, $2, lookup_process($3)); }
    | ID '(' exprs ')' ID '*'
       { $$ = elt_node_head($1, $3, $5, null_process); }
    | ID ID '*'
       { $$ = elt_node_head($1, NULL, $2, null_process); }
    | ID ID
       { $$ = elt_node_head(NULL, NULL, $1, lookup_process($2)); }
    | ID '*'
       { $$ = elt_node_head(NULL, NULL, $1, null_process); } ;

forLine: 
      forHead codeBlock 
       { $$ = for_node_body($1, $2); } ;

forHead:
      FOR ID '=' expr ':' expr
       { $$ = for_node($2, $4, $6); } ;

ifLine:
      IF expr codeLine
       { $$ = if_node($2, $3, NULL); }
    | IF expr codeLine ELSE codeLine
       { $$ = if_node($2, $3, $5); }

def: 
      ID '=' expr 
       { $$ = def_node($1, $3); } ;

defs: 
      defs def
       { $$ = cat_nodes($1, $2); }
    | 
       { $$ = NULL; } ;

names: 
      names name
       { $$ = cat_nodes($1, $2); }
    | 
       { $$ = NULL; } ;

name: 
      ID
       { $$ = name_node($1, NULL); }
    | ID '(' exprs ')'
       { $$ = name_node($1, $3); } ;

expr: 
      expr '+' expr
       { $$ = call_node("+", cat_nodes($1, $3)); }
    | expr '-' expr
       { $$ = call_node("-", cat_nodes($1, $3)); }
    | expr '*' expr
       { $$ = call_node("*", cat_nodes($1, $3)); }
    | expr '/' expr
       { $$ = call_node("/", cat_nodes($1, $3)); }
    | expr '^' expr
       { $$ = call_node("^", cat_nodes($1, $3)); }
    | '-' expr %prec UMINUS
       { $$ = call_node("unary-", $2); }
    | NOT expr
       { $$ = call_node("!", $2); }
    | expr AND expr 
       { $$ = call_node("&", cat_nodes($1, $3)); }
    | expr OR expr 
       { $$ = call_node("|", cat_nodes($1, $3)); }
    | expr EQ expr
       { $$ = call_node("==", cat_nodes($1, $3)); }
    | expr NEQ expr
       { $$ = call_node("!=", cat_nodes($1, $3)); }
    | expr GT expr
       { $$ = call_node(">", cat_nodes($1, $3)); }
    | expr LT expr
       { $$ = call_node("<", cat_nodes($1, $3)); }
    | expr GEQ expr
       { $$ = call_node(">=", cat_nodes($1, $3)); }
    | expr LEQ expr
       { $$ = call_node("<=", cat_nodes($1, $3)); }
    | '(' expr ')'
       { $$ = $2; }
    | INT
       { $$ = float_node($1.val, $1.tok); }
    | FLOAT
       { $$ = float_node($1.val, $1.tok); }
    | STRING
       { $$ = string_node($1); }
    | ID
       { $$ = var_node($1); }
    | ID '(' optexprs ')' 
       { $$ = call_node($1, $3); }

callNode: 
    CALL ID '(' optexprs ')' 
       { $$ = call_node($2, $4); }

exprs: 
      exprs ',' expr
       { $$ = cat_nodes($1, $3); }
    | expr ;

optexprs:
      exprs 
    |  
       { $$ = NULL; };

%%

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
