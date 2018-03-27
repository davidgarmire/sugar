#include <stdio.h>
#include "dynarray.h"

int main()
{
    int i;
    dynarray_t array;
    array = dynarray_create(sizeof(int), 4);
    dynarray_destroy(array);
    array = dynarray_create(sizeof(int), 4);
    for (i = 0; i < 8; ++i)
        dynarray_append(array, &i);

    printf("After appending eight times:");
    for (i = 0; i < dynarray_count(array); ++i)
        printf(" %d", *(int*) dynarray_get(array, i));
    printf("\n");
    i = 16;
    dynarray_set(array, i, &i);

    printf("After setting element sixteen:");
    for (i = 0; i < dynarray_count(array); ++i)
        printf(" %d", *(int*) dynarray_get(array, i));
    printf("\n");
    dynarray_set_count(array, 5);

    printf("After decreasing to five elements:");
    for (i = 0; i < dynarray_count(array); ++i)
        printf(" %d", *(int*) dynarray_get(array, i));
    printf("\n");
    dynarray_set_count(array, 16);

    printf("After increasing to 16 elements:");
    for (i = 0; i < dynarray_count(array); ++i)
        printf(" %d", *(int*) dynarray_get(array, i));
    printf("\n");
    for (i = 0; dynarray_get(array, i) != NULL; ++i);
    printf("Ran into a NULL get at index %d\n", i);
    *(int*) dynarray_set(array, 31, NULL) = 31;

    printf("After setting element 31:");
    for (i = 0; i < dynarray_count(array); ++i)
        printf(" %d", *(int*) dynarray_get(array, i));
    printf("\n");
    dynarray_destroy(array);
    return 0;
}
