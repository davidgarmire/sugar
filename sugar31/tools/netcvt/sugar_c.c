#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "lexer.h"
#include "parse.h"

void print_call_string(FILE* fp, code_t* code);
void print_code_item(FILE* fp, char* tab, code_t* code, int nodefs);
void print_code_list(FILE* fp, char* tab, code_t* firstcode, 
                     char* delim, int nodefs);

void print_call_string(FILE* fp, code_t* code)
{
    fprintf(fp, "(");
    if (strcmp(code->v.call.name, "+") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, "+");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, "-") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, "-");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, "*") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, "*");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, "/") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, "/");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, "^") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, "^");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, "unary-") == 0) {
        fprintf(fp, "-");
        print_code_item(fp, "", code->v.call.args, 1);
    } else if (strcmp(code->v.call.name, "!") == 0) {
        fprintf(fp, "not ");
        print_code_item(fp, "", code->v.call.args, 1);
    } else if (strcmp(code->v.call.name, "&") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, " and ");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, "|") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, " or ");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, "==") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, " == ");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, "!=") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, " ~= ");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, ">") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, " > ");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, "<") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, " < ");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, ">=") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, " >= ");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, "<=") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, " <= ");
        print_code_item(fp, "", code->v.call.args->next, 1);
    } else if (strcmp(code->v.call.name, "cond") == 0) {
        print_code_item(fp, "", code->v.call.args, 1);
        fprintf(fp, " and ");
        print_code_item(fp, "", code->v.call.args->next, 1);
        fprintf(fp, " or ");
        print_code_item(fp, "", code->v.call.args->next->next, 1);
    } else {
        fprintf(fp, "%s(", code->v.call.name);
        if (code->v.call.args)
            print_code_list(fp, "", code->v.call.args, ", ", 1);
        fprintf(fp, ")");
    }
    fprintf(fp, ")");
}

void print_code_item(FILE* fp, char* tab, code_t* code, int nodefs)
{
    char *newtab = (char *) malloc(100 * sizeof(char));
    code_t *newcode;
    sprintf(newtab, "%s  ", tab);
    switch (code->tag) {
    case INT_NODE:
        fprintf(fp, "%d", code->v.ival);
        break;

    case DOUBLE_NODE:
	fprintf(fp, "%s", code->token);
        break;

    case STRING_NODE:
        fprintf(fp, "\"%s\"", code->v.sval);
        break;

    case UNDEF_NODE:
        break;

    case CALL_NODE:
        print_call_string(fp, code);
        break;

    case VAR_NODE:
        fprintf(fp, "%s", code->v.var->v.def.name);
        break;

    case DEF_NODE:
        if (!nodefs && code->v.def.scope == 1)
            fprintf(fp, "%slocal ", tab);
        fprintf(fp, "%s%s = ", tab, code->v.def.name);
        print_code_list(fp, newtab, code->v.def.value, " ", 1);
        break;

    case PARAM_NODE:
        if (code->v.def.value) {
            fprintf(fp, "if not %s then %s = ", 
                    code->v.def.name, code->v.def.name);
            print_code_list(fp, newtab, code->v.def.value, ", ", 1);
            fprintf(fp, " end\n");
        }
        break;

    case PROCESS_NODE:
        fprintf(fp, "%s%s = material {\n", tab, code->v.process.name);
        if (code->v.process.parent)
            fprintf(fp, "%sparent = %s,\n",
                   newtab, code->v.process.parent->v.process.name);
        print_code_list(fp, newtab, code->v.process.defs, ",\n", 1);
        fprintf(fp, "%s\n}\n", tab);
        break;

    case NAME_NODE:
        fprintf(fp, "_n(\"%s\"", code->v.name.name);
	if (code->v.name.indices) {
	    fprintf(fp, " .. \"_\" .. (");
	    print_code_list(fp, tab, code->v.name.indices, 
                            ")..\"_\"..(", 1);
	    fprintf(fp, ")");
	}
	fprintf(fp, ")");
        break;

    case ELEMENT_NODE:
	fprintf(fp, "%smf%s {", tab, code->v.element.model);
	if (code->v.element.nodes)
	    print_code_list(fp, "", code->v.element.nodes, ", ", 1);
	fprintf(fp, "; ");
	if (code->v.element.process && 
                *code->v.element.process->v.process.name != '*')
	    fprintf(fp, "material = %s, ", 
                    code->v.element.process->v.process.name);
	print_code_list(fp, "", code->v.element.params, ", ", 1);
	fprintf(fp, "}");
	break;

    case FOR_NODE:
        fprintf(fp, "%sfor ", tab);
        print_code_list(fp, "", code->v.forloop.idx, "?", 1);
        print_code_list(fp, "", code->v.forloop.lower, "?", 1);
        fprintf(fp, " , ");
        print_code_list(fp, "", code->v.forloop.upper, "?", 1);
        fprintf(fp, " do\n");
        print_code_list(fp, newtab, code->v.forloop.body, "\n", 0);
        fprintf(fp, "\n%send", tab);
        break;

    case IF_NODE:
        fprintf(fp, "%sif ", tab);
        print_code_list(fp, newtab, code->v.ifstmt.cond, "?", 1);
        fprintf(fp, " then\n");
        print_code_list(fp, newtab, code->v.ifstmt.then_clause, "\n", 0);
        fprintf(fp, "%selse\n", tab);
        print_code_list(fp, newtab, code->v.ifstmt.else_clause, "\n", 0);
        fprintf(fp, "\n%send", tab);
        break;

    case SUBNET_NODE:
        if (code->v.subnet.model)
            fprintf(fp, "%ssubnet mf%s (", tab, code->v.subnet.model);
        if (code->v.subnet.nodes) {
	    newcode = code->v.subnet.nodes;
	    while (newcode) {
		fprintf(fp, "%s", newcode->v.name.name);
		newcode = newcode->next;
		if (newcode)
		    fprintf(fp, ", ");
	    }
            fprintf(fp, ", ");
        }

	fprintf(fp, "material");
	newcode = code->v.subnet.params;
	if (newcode)
	    fprintf(fp, ", ");

        while (newcode) {
	    fprintf(fp, "%s", newcode->v.def.name);
            newcode = newcode->next;
	    if (newcode)
		fprintf(fp, ", ");
        }
	fprintf(fp, ")\n");
	fprintf(fp, "%slocal parent = material\n", newtab);
	
	newcode = code->v.subnet.nodes;
	while (newcode) {
	    fprintf(fp, "%s_currnodes[\"%s\"] = %s\n", newtab,
		   newcode->v.name.name, newcode->v.name.name);
	    newcode = newcode->next;
	}

        if (code->v.subnet.body)
            print_code_list(fp, tab, code->v.subnet.body, "\n", 0);
        fprintf(fp, "\nend\n");
        break;

    case BLOCK_NODE:
        if (code->v.block)
            print_code_list(fp, newtab, code->v.block, "\n", 0);
        break;

    default:
        fprintf(fp, "Didn't understand\n");
        break;
    }
}

void print_code_list(FILE* fp, char *tab, code_t* firstitem, 
                     char *delim, int nodefs)
{
    code_t* item = firstitem;
    while (item) {
        if (item != firstitem)
            fprintf(fp, "%s", delim);
        print_code_item(fp, tab, item, nodefs);
        item = item->next;
    }
}


int main(int argc, char **argv)
{
    code_t* code;
    mempool_t parse_pool;
    extern FILE* yyin;
    FILE* fpout;

    yyin  = (argc > 1) ? fopen(argv[1], "r") : stdin;
    fpout = (argc > 2) ? fopen(argv[2], "w") : stdout;

    if (yyin == NULL) {
        printf("Cannot open %s\n", argv[1]);
        exit(1);
    }

    if (fpout == NULL) {
        printf("Cannot open %s\n", argv[2]);
        exit(1);
    }

    parse_pool = mempool_create(MEMPOOL_DEFAULT_SPAN);
    code = sugar_parse(parse_pool);

    fprintf(fpout, "use(\"std2cvt.net\")\n");
    fprintf(fpout, "\n");
    print_code_list(fpout, "", code, "\n", 0);
    fprintf(fpout, "\n");

    mempool_destroy(parse_pool);

    if (yyin != stdin)
        fclose(yyin);

    if (fpout != stdout)
        fclose(fpout);

    return 0;
}

