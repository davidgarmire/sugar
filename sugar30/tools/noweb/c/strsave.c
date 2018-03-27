#line 6 "strsave.nw"
static char rcsid[] = "$Id: strsave.c,v 1.3 2002/04/05 09:49:35 strive Exp $";
static char rcsname[] = "$Name:  $";
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "strsave.h"
#include "errors.h"

char *strsave (char *s) {
    char *t = malloc (strlen(s)+1);
    checkptr(t);
    strcpy(t,s);
    return t;
}
