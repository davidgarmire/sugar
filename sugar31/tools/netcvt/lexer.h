#ifndef __LEXER_H
#define __LEXER_H

/*
 * Functions related to managing the lexer:
 *
 *  lex_init  -- Initialize the lexer internals
 *  lex_clear -- Close any remaining files open during lexing
 *               (Particularly useful if a parse error stops things)
 *
 *  which_file -- Search the path (MATLAB_PATH) for the given file
 *  print_files_used -- Write to file a list of "uses"
 *  
 *  get_parse_position -- Get file and line number for use in later
 *                        error reporting
 *  yyerror -- Standard lex/yacc error reporting routine
 *  add_error -- Report an error in post-parsing phase; additional arguments
 *               tell where to report the error.
 *  add_error_stack -- Print a stack trace line during an execution-phase
 *               error
 *  no_errors -- Returns true iff there were no errors during translation
 */
 
#include <stdio.h>
#include "mempool.h"

void lex_init(mempool_t pool);
void lex_clear();
FILE* which_file(char* filename); 
void print_files_used(FILE* fp);
void get_parse_position(int* lineno_out, int* fileno_out);
int yyerror(char* errmsg);
int add_error(int fileno, int lineno, char* token, char* errmsg, 
              int is_warning);
int add_error_stack(int parse_fileno, int parse_lineno, char* stack_name);
int no_errors();

#endif /* __LEXER_H */
