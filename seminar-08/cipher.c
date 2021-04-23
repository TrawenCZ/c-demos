#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Do *NOT* use ‹readline()› in this exercise! It will not work as you expect.
// #include "readline.h"

#define UNUSED(VAR) \
    ((void) (VAR))

/**
 * Reads 512 binary bytes from \p key and uses it as key to "encrypt"
 * each block of 512 from \p in. Resulting blocks are written to \p out.
 *
 * @param   key         the file to read key from
 * @param   in          input file
 * @param   out         output file
 * @return  0 on success, an arbitrary error code on failure
 */
int xorcrypt(FILE *key, FILE *in, FILE *out)
{
    char buffer_in[512];
    char buffer_key[512];
    int length;
    fread(buffer_in, 1, 512, in);
    fread(buffer_key, 1, 512, key);

    while ((length = strlen(buffer_in)) != 0) {
        for (int i = 0; i < length; i++) {
            fwrite(buffer_in[i] ^ buffer_key[i], 1, 1, out);
        }
        fread(buffer_in, 1, 512, in);
    }

    return 1;
}

int main(int argc, char *argv[])
{
    if (argc != 4) {
        fprintf(stderr, "usage: %s KEY INPUT OUTPUT\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE* key = fopen(argv[1], "rb");
    FILE* in = fopen(argv[2], "rb");
    FILE* out = fopen(argv[3], "wb");

    xorcrypt(key, in, out);

    fclose(key); fclose(in); fclose(out);

    return EXIT_SUCCESS;
}
