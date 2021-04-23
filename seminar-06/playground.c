#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dynamicLib.h"

void play_dyn_strcpy()
{
    const char *orig = "Ahoj svet !";
    char *copy = dyn_strcpy(orig);
    printf("Original: %s\n", orig);
    printf("Copy: %s\n", copy);
    free(copy);
}

void play_dyn_strjoin()
{
    const char *ahoj = "";
    const char *svet = "";
    char *result = dyn_strjoin(ahoj, svet);
    printf("Joined strings: \"%s\"\n", result);
    free(result);
}

void play_read_line()
{
    printf("Write something: ");
    char *line = read_line();
    printf("You have typed: \"%s\"\n", line);
    free(line);
}

static void str_printer(char **array, size_t size)
{
    for (int i = 0; i < (int)size; i++) {
        printf("%d - %s\n", i + 1, array[i]);
    }
}

void play_split()
{
    const char *orig = "carrot, bannana, orange, kebab, mouse";
    size_t size;
    char **split = dyn_str_split_sub(orig, ", ", &size);
    printf("Original string: %s\n", orig);
    printf("Splited:\n");
    str_printer(split, size);
    for (size_t i = 0; i < size + 1; i++) {
        free(split[i]);
    }
    free(split);
}

int main()
{
    play_dyn_strcpy();
    play_dyn_strjoin();
    play_split();

    return 0;
}
