#ifndef GETLINE_H
#define GETLINE_H

#include <stddef.h>
#include <stdio.h>

/**
 * Getline function adapted from POSIX.
 *
 * See https://linux.die.net/man/3/getline for documentation.
 */
long getline(char **lineptr, size_t *n, FILE *file);

#endif // GETLINE_H
