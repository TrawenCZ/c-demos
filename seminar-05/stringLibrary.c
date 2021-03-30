#include <stdio.h>
#include "stringLibrary.h"

#define UNUSED(param) ((void)(param))

size_t string_length(const char *str)
{
    int length = 0;
    char* pointer = str;
    while (*pointer != '\0') {
        pointer += 1;
        length += 1;
    }
    return length;
}

char *string_copy(char *result, const char *origin)
{
    char* pointer = origin;
    if (pointer == NULL) return NULL;
    while (*pointer != '\0') {
        *result = *pointer;
        pointer += 1; result += 1;
    }
    *result = *pointer;
    return *pointer;
}

size_t string_count_char(const char *string, char ch)
{
    // TODO
    UNUSED(string);
    UNUSED(ch);
    return 0;
}

size_t string_count_substr(const char *original, const char *substring)
{
    // TODO
    UNUSED(original);
    UNUSED(substring);
    return 0;
}

size_t string_count_string_in_array(const char **array, const char *string)
{
    //TODO
    UNUSED(array);
    UNUSED(string);
    return 0;
}

void string_split(const char *original, char result[50][256], int *size, char delim)
{
    // TODO
    UNUSED(original);
    UNUSED(result);
    UNUSED(size);
    UNUSED(delim);
}
