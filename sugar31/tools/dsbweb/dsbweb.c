/*
 * Simplified .nw file tangle program.  This code may not be as robust
 * as notangle, but it can be run without the (problematic) noweb install.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Structure to hold a linked list of named text fragments.
 */
typedef struct fragment {
    char* tag;
    char* data;
    struct fragment* next;
} fragment_t;


/* ---- Prototypes ---- */

void add_entry(fragment_t** head, char* tag, char* data);
void print_fragment_list(fragment_t* entries);

void expand_fragment_data(char* data, int indent, 
                       fragment_t* entries, FILE* outfp);
void expand_fragment(char* name, int indent, 
                  fragment_t* entries, FILE* outfp);


/* ---- Functions ---- */

/*
 * Add the tagged data to the entry list starting with *head.
 * If the tag matches one already in the list, concatenate the data
 * onto the existing entry; otherwise, create a new entry.
 */
void add_entry(fragment_t** head, char* tag, char* data)
{
    fragment_t** current = head;
    fragment_t*  new_entry;

    while (*current) {
        if (strcmp((*current)->tag, tag) == 0)
            break;
        current = &((*current)->next);
    }

    if (*current) {

        fragment_t* old_entry = *current;
        char* new_data = malloc(strlen(old_entry->data) + strlen(data) + 1);

        strcpy(new_data, old_entry->data);
        strcat(new_data, data);

        free(old_entry->data);
        free(tag);

        old_entry->data = new_data;
        
    } else {

        new_entry = malloc(sizeof(fragment_t));
        new_entry->tag  = tag;
        new_entry->data = strdup(data);
        new_entry->next = NULL;

        *current = new_entry;

    }
}


/*
 * Build a list of all fragment definitions in a web input file.
 * Fragment definitions have the form
 *
 *   <<tag>>=
 *   data in macro body
 *   @
 *
 * CAVEAT: Fragment sections longer than 32K, or tags longer than 250
 *   characters, will cause buffer overflows.
 */
fragment_t* build_fragment_list(FILE* fp)
{
    static char data_buf[32768];
    char buf[256];
    fragment_t*  head = NULL;

    while (fgets(buf, 256, fp)) {
        if (buf[0] == '<' && buf[1] == '<') {
            char* tag_end = strstr(buf, ">>=");
            if (tag_end) {
                char* tag;

                *tag_end = '\0';
                tag = strdup(buf + 2);

                *data_buf = '\0';
                while (fgets(buf, 256, fp) && *buf != '@') {
                    strcat(data_buf, buf);
                }

                add_entry(&head, tag, data_buf);
            }
        }
    }

    return head;
}


/*
 * Deallocate an existing fragment list
 */
void delete_fragment_list(fragment_t* entries)
{
    while (entries) {

        fragment_t* dead_entry = entries;
        entries = entries->next;

        free(dead_entry->data);
        free(dead_entry->tag);
        free(dead_entry);
    }
}


/*
 * Print all fragments from the list.  Useful purely for diagnostic purposes.
 */
void print_fragment_list(fragment_t* entries)
{
    while (entries) {
        printf("-------------------------------------------------------\n");
        printf("<<%s>>=\n%s", entries->tag, entries->data);
        entries = entries->next;
    }
    printf("-------------------------------------------------------\n");
}


/*
 * Expand macro definitions in the specified text data and print
 * the result to outfp.  Macro uses have the form <<tag>>.
 * After the first line, multi-line macro text is indented to match
 * the indentation level at which the macro was invoked.
 */
void expand_fragment_data(char* data, int indent, fragment_t* entries,
                          FILE* outfp)
{
    char* eol = strstr(data, "\n");
    int first_line = 1;

    while (eol) {

        char* macro_begin = strstr(data, "<<");
        char* macro_end   = strstr(data, ">>");
        int i;

        *eol        = '\0';
        macro_begin = strstr(data, "<<");
        macro_end   = strstr(data, ">>");

        if (first_line) {
            first_line = 0;
        } else {
            for (i = 0; i < indent; ++i) 
                fprintf(outfp, " ");
        }

        if (macro_begin && macro_end) {
            char  macro_tag[128];

            *macro_begin = '\0';
            fprintf(outfp, "%s", data);
            *macro_begin = '<';

            *macro_end = '\0';
            strcpy(macro_tag, macro_begin + 2);
            *macro_end = '>';

            expand_fragment(macro_tag, indent + (macro_begin - data), 
                            entries, outfp); 
            data = macro_end + 2;
        } 

        fprintf(outfp, "%s", data);
        if (eol[1])
            fprintf(outfp, "\n");

        *eol = '\n';
        data = eol + 1;
        eol  = strstr(data, "\n");
    }
}


/*
 * Expand the text of the named macro definition and print to outfp.
 * Indent any lines after the first at the specified indentation level.
 */
void expand_fragment(char* name, int indent, fragment_t* entries,
                     FILE* outfp)
{
    fragment_t* current_entry = entries;
    while (current_entry) {
        if (strcmp(current_entry->tag, name) == 0)
            expand_fragment_data(current_entry->data, indent, entries, outfp);
        current_entry = current_entry->next;
    }
}


/* ---- Main routines ---- */


/* 
 * Print program help
 */
void print_help()
{
    fprintf(stderr,
            "Syntax:\n"
            "\n"
            "  dsbweb -Rroot nwfile\n"
            "\n"
            "    Noweb compatibility mode.  Expand listed root fragments\n"
            "    to stdout from the specified nwfile.  Only one file can\n"
            "    be handled at a time.\n"
            "\n"
            "  dsbweb [-ddir] [-underscore] [-list] .ext nwfiles\n"
            "\n"
            "    Process all fragments with the given extension and write\n"
            "    them to appropriately named files in a given output dir.\n"
            "    If the -underscore option is specified, all dash chars are\n"
            "    replaced by underscores.\n"
            "\n"
            "    If -list is specified, the (pathed) names of the files\n"
            "    that would be generated are printed to the standard output.\n"
            "    In this case, the files themselves are not specified.\n"
            "\n");
}


/*
 * Main routine for noweb emulation mode (root fragments specified with -R).
 */
int noweb_main(int argc, char** argv)
{
    FILE* fp;
    fragment_t* entries;

    char* nw_file_name = NULL;
    int i;

    /* Get input file name */
    for (i = 1; i < argc; ++i) {
        if (argv[i][0] != '-') {
            if (nw_file_name) {
                fprintf(stderr, "Can only process one file at a time.\n");
                return -1;
            }
            nw_file_name = argv[i];
        }
    }

    if (nw_file_name == NULL) {
        fprintf(stderr, "Could not find file name\n");
        return -1;
    }
   
    /* Process the .nw file */
    fp = fopen(nw_file_name, "r");
    if (fp == NULL) {
        fprintf(stderr, "Could not find file %s\n", nw_file_name);
        return -1;
    }
    entries = build_fragment_list(fp);
    fclose(fp);

    /* Expand root fragments */
    for (i = 1; i < argc; ++i) {
        if (argv[i][0] == '-' && argv[i][1] == 'R') {
            expand_fragment(argv[i]+2, 0, entries, stdout);
        }
        fprintf(stdout, "\n");
    }

    delete_fragment_list(entries);
    return 0;
}


/*
 * Generate files with specified extension from a noweb input file.
 */
int dsbweb_process(char* nw_name, char* ext_name, char* dir_name, 
                   int underscore_flag, int list_flag)
{
    FILE*        in_fp;
    fragment_t*  entries;

    fragment_t*  entry;
    char         buf[256];

    in_fp = fopen(nw_name, "r");
    if (in_fp == NULL) {
        fprintf(stderr, "Could not open %s for input\n", nw_name);
        return -1;
    }
    entries = build_fragment_list(in_fp);
    fclose(in_fp);

    entry = entries;
    while (entry) {
        char* ext = strrchr(entry->tag, '.');
        if (ext && strcmp(ext+1, ext_name) == 0) {
            char* file_name_part;

            /* Form full file name
             */
            strcpy(buf, dir_name);
            file_name_part = buf + strlen(buf);
            strcpy(file_name_part, entry->tag);
            if (file_name_part) {
                while (*file_name_part) {
                    if (*file_name_part == '-')
                        *file_name_part = '_';
                    ++file_name_part;
                }
            }

            /* Print or expand file
             */
            if (list_flag)
                printf("%s\n", buf);
            else {
                FILE* out_fp = fopen(buf, "w");
                if (out_fp == NULL) {
                    fprintf(stderr, "Error opening %s for output\n", buf);
                    return -1;
                }
                expand_fragment(entry->tag, 0, entries, out_fp);
                fprintf(out_fp, "\n");
                fclose(out_fp);
            }

        }
        entry = entry->next;
    }

    delete_fragment_list(entries);
}


/*
 * Process dsbweb options and fenerate files with specified extension 
 * from a list of noweb files.
 */
int dsbweb_main(int argc, char** argv)
{
    char* dir_name        = "";
    int   underscore_flag = 0;
    int   list_flag       = 0;

    int i, j;

    /* Process flags */
    for (i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            if (argv[i][1] == 'd')
                dir_name = argv[i] + 2;
            else if (strcmp(argv[i] + 1, "underscore") == 0)
                underscore_flag = 1;
            else if (strcmp(argv[i] + 1, "list") == 0)
                list_flag = 1;
        }
    }

    /* Process all .ext / web file pairs */
    for (i = 1; i < argc; ++i) {
        if (argv[i][0] == '.') {
            char* ext_name = argv[i] + 1;
            for (j = 1; j < argc; ++j) {
                if (argv[j][0] != '-' && argv[j][0] != '.') {
                    if (dsbweb_process(argv[j], ext_name, dir_name,
                                       underscore_flag, list_flag) < 0)
                        return -1;
                }
            }
        }
    }

    return 0;
}


int main(int argc, char** argv)
{
    int i;

    if (argc < 2) {
        print_help();
        return -1;
    }

    for (i = 1; i < argc; ++i)
        if (argv[i][0] == '-' && argv[i][1] == 'R')
            return noweb_main(argc, argv);

    return dsbweb_main(argc, argv);
}
