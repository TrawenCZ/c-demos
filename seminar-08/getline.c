#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Do *NOT* use ‹readline()› in this exercise! That would be insane.
// #include "readline.h"

#define UNUSED(VAR) \
    ((void) (VAR))

long getline(char **lineptr, size_t *n, FILE *file)
{
    if (lineptr == NULL || n == NULL)
        return -1;

    // TODO: implement the function
    UNUSED(file);

    return -1;
}
