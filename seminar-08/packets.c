#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * Text file packet structure.
 */
struct packet
{
    uint16_t id;        ///< sequence number of the packet
    uint16_t length;    ///< length of the actual data stored in 'data'
    char     data[28];  ///< at most 28 characters of data
};

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "usage: %s FILE\n", argv[0]);
        return EXIT_FAILURE;
    }

    // TODO: open file in binary mode

    // TODO: read packets from the file one by one
    //       and print data

    // TODO: close file

    return EXIT_SUCCESS;
}
