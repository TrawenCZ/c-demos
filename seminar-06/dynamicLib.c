//
// Created by Wermington on 15.2.16.
//

#include "dynamicLib.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define UNUSED(a) ((void)(a))

#define NOT_IMPLEMENTED()                                         \
    do {                                                        \
        fprintf(stderr, "%s is not implemented!\n", __func__);  \
        abort();                                                \
    } while(0)

int str_size(const char *str) {
    int length = 0;
    while (str[length] != '\0') {
        length += 1; 
    }
    return length;
}

char *dyn_strcpy(const char *str)
{   
    if (str == NULL) return NULL;
    int length = str_size(str);
    char* new_str = malloc(length*sizeof(char));
    if (new_str == NULL) return NULL;

    strcpy(new_str, str);
    return new_str;
}

char *dyn_strjoin(const char *pre, const char *post)
{
    if (pre == NULL || post == NULL) return NULL;
    int length1 = strlen(pre), length2 = strlen(post);
    char* new_str = malloc((length1 + length2+1) * sizeof(char));
    if (new_str == NULL) return NULL;

    strcpy(new_str, pre); strcpy(new_str + length1, post);
    return new_str;
}

char *read_line()
{
    // TODO
    NOT_IMPLEMENTED();
    return NULL;
}

void **dyn_alloc2d(size_t rows, size_t colSizes[rows])
{
    //TODO
    UNUSED(rows);
    UNUSED(colSizes);
    NOT_IMPLEMENTED();
    return NULL;
}

int dyn_free2d(void **memory, size_t rows)
{
    // TODO
    UNUSED(rows);
    UNUSED(memory);
    NOT_IMPLEMENTED();
    return 1;
}

/**
 * BONUSES
 */

unsigned **pascal_triangle(size_t depth)
{
    UNUSED(depth);
    NOT_IMPLEMENTED();
    return NULL;
}

char **dyn_str_split_sub(const char *orig, const char *splitter, size_t *size)
{
    // TODO
    UNUSED(orig);
    UNUSED(splitter);
    UNUSED(size);
    NOT_IMPLEMENTED();
    return NULL;
}
