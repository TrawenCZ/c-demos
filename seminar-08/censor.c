#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readline.h"

#define UNUSED(VAR) \
    ((void) (VAR))

/**
 * Searches for \p search in each line of \p in and replaces it with
 * stars. The result is written into \p out.
 *
 * \param   search      string to censor
 * \param   in          input file
 * \param   out         output file
 * \return  0 on success, an arbitrary error code on failure
 */
int censor(const char *search, FILE *in, FILE *out)
{
    char* line;
    char* loader;
    int censor_length = strlen(search);
    char replacement[censor_length];

    for (int i = 0; i < censor_length; i++) {
        replacement[i] = '*';
    }
    
    while ((line = readline(in)) != NULL) {
        if ((loader = strstr(line, search)) != NULL) {
            strncpy(loader, replacement, censor_length);
        }
        fprintf(out, "%s", line);
        free(line);
    }

    return 1;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "usage: %s STRING SOURCE DESTINATION\n", argv[0]);
        return EXIT_FAILURE;
    }

    char* search = argv[1];
    FILE* in = fopen(argv[2], "r");
    FILE* out = fopen(argv[3], "w");

    censor(search, in, out);

    fclose(in);
    fclose(out);

    return EXIT_SUCCESS;
}
