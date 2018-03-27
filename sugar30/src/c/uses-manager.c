#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string.h>

#include "uses-manager.h"

typedef struct name_list_t {
    const char* name;
    struct name_list_t* next;
} name_list_t;

struct uses_manager_struct {
    mempool_t pool;
    name_list_t* paths;
    name_list_t* files;    
};

uses_manager_t uses_manager_create(mempool_t pool)
{
    uses_manager_t self = (uses_manager_t) 
        mempool_get(pool, sizeof(struct uses_manager_struct));

    self->pool = pool;
    self->paths = NULL;
    self->files = NULL;
    return self;
}

void uses_manager_addpath(uses_manager_t self, const char* name)
{
    name_list_t* link = (name_list_t*)
        mempool_get(self->pool, sizeof(struct name_list_t));

    link->name = mempool_strdup(self->pool, name);
    link->next = self->paths;
    self->paths = link;
}

void uses_manager_envpath(uses_manager_t self, const char* envname)
{
    char* names;
    char* name;

    if ((names = getenv(envname)) == NULL)
        return;

    names = mempool_strdup(self->pool, names);

    while ((name = strtok(names, ":")) != NULL) {
        name_list_t* link = (name_list_t*)
            mempool_get(self->pool, sizeof(struct name_list_t));
        link->name = name;
        link->next = self->paths;
        self->paths = link;
        names = NULL;
    }
}

int uses_manager_access(const char* name)
{
    FILE* fp = fopen(name, "r");
    if (fp) {
        fclose(fp);
        return 0;
    }
    return -1;
}

char* uses_manager_search(uses_manager_t self, char* namebuf,
                          int buflen)
{
    name_list_t* current_path = self->paths;

    if (uses_manager_access(namebuf) == 0)
        return namebuf;
    while (current_path != NULL) {

        int tmpbuflen = strlen(current_path->name) +
                        strlen(namebuf) + 2;
        char* tmpbuf = (char*) mempool_geth(self->pool, tmpbuflen);

        strcpy(tmpbuf, current_path->name);
        strcat(tmpbuf, "/");
        strcat(tmpbuf, namebuf);
        if (uses_manager_access(tmpbuf) == 0) {
            if (strlen(tmpbuf) + 1 > buflen) {
                mempool_freeh(tmpbuf);
                return NULL;
            } else {
                strcpy(namebuf, tmpbuf);
                mempool_freeh(tmpbuf);
                return namebuf;
            }
        }
        
        mempool_freeh(tmpbuf);
        current_path = current_path->next;
    }

    return NULL;
}

void uses_manager_use(uses_manager_t self, const char* name)
{
    if (!uses_manager_already_used(self, name)) {
        name_list_t* new_link = (name_list_t*)
            mempool_get(self->pool, sizeof(name_list_t));

        new_link->name = mempool_strdup(self->pool, name);
        new_link->next = self->files;
        self->files = new_link;
    }
}

int uses_manager_already_used(uses_manager_t self, const char* name)
{
    name_list_t* link = self->files;
    while (link != NULL) {
        if (strcmp(link->name, name) == 0)
            return 1;
        link = link->next;
    }
    return 0;
}

