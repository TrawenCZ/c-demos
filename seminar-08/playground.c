#include <stdio.h>
#include <stdlib.h>

#include "getline.h"

int main(void)
{
    char *lineptr = NULL;
    size_t size = 0;

    while (getline(&lineptr, &size, stdin) > 0) {
        printf("lineptr=%p, size=%zu\n", (void*) lineptr, size);
        printf("%s", lineptr);
    }

    free(lineptr);
    return 0;
}
