#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stringLibrary.h"

void play_split()
{
    const char *original = "Ahoj svet 000 Query";
    char result[50][256];
    int size;
    char delim = ' ';

    string_split(original, result, &size, delim);
    printf("SPLIT: \nSize: %d\nParts: \n", size);
    for (int i = 0; i < size; i++) {
        printf("   %d. - %s\n", i + 1, result[i]);
    }
}

void tests() {
    char str[] = "Popokatepetl";
    size_t length = string_length(str);
    char buffer[length + 1];
    printf("Size of string is: %d\n", length);
    if (string_copy(buffer, str) == 0) {
        printf("Copied string is: %s\n", buffer);
    } else { printf("Input is NULL"); }
}

int main(void)
{
    //play_split();
    tests();

    return 0;
}
