#ifndef USES_MANAGER_H
#define USES_MANAGER_H

#include <stdio.h>
#include "mempool.h"

typedef struct uses_manager_struct* uses_manager_t;

uses_manager_t uses_manager_create(mempool_t pool);

void  uses_manager_addpath(uses_manager_t self, const char* path);
void  uses_manager_envpath(uses_manager_t self, const char* envname);
char* uses_manager_search (uses_manager_t self, char* namebuf, int buflen);

void  uses_manager_use         (uses_manager_t self, const char* name);
int   uses_manager_already_used(uses_manager_t self, const char* name);

#endif /* USES_MANAGER_H */
