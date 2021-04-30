#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>

#define ALLOWED_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz!-.:?_\0"
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

typedef struct file_path 
{
    char *path_to_file;
    struct file_path *next;
    int size;
} file_path;

typedef struct used_paths {
    file_path *top;
    file_path *bottom;
    int depth;
} used_paths;

static void usage(const char *program)
{
    const char *help =
            "Usage: %s OPTIONS\n"
            "\n"
            "This program loads a .ini file and converts him with following options:\n"
            "\n"
            "\t-h\t--help\t\t\tShow this helpful message (and ends program)\n"
            "\t-d N\t--max-depth N\t\tSets maximum depth immersion limit [N]. ([N=0] = no immersion, negative N = no limit)\n"
            "\t-g\t--include-guard\t\tPrint loaded sudoku without formatting\n"
            "\t-r\t--report-cycles\t\tPrint loaded sudoku in ASCII grid\n"
            "\n";

    printf(help, program, program);
}

void ignore_line(input) 
{
    char loaded;
    while ((loaded = fgetc(input)) != EOF && loaded != '\n') {
        continue;
    }
}

bool sectionize(FILE *input, FILE *output, char *prefix) 
{
    char loaded;
    fprintf(output, "[%s", prefix);
    while ((loaded = fgetc(input)) != EOF && loaded != '\n' && loaded != ']') {
        if (isspace(loaded)) continue;
        if (strchr(ALLOWED_CHARS, loaded) == NULL) return false;
        fprintf(output, "%c", loaded);
    }
    fprintf(output, "]\n");
    return true;
}

bool key_bind(input, output) 
{

}

bool add_to_paths(file_path *new_path, used_paths *paths) 
{
    if (paths->top == NULL) {
        paths->top = new_path;
        paths->bottom = new_path;
    } else {
        file_path *pPath = paths->top;
        while (pPath != NULL) {
            if (pPath->path_to_file == new_path->path_to_file) return false;
            pPath = pPath->next;
        }
        path->next = NULL;
        paths->bottom->next = new_path;
        paths->bottom = new_path;
    }
    return true;
}

bool reallocate(file_path *new_path, int *size_limit)
{
    *size_limit += 255;
    void *holder = realloc(new_path->path_to_file, sizeof(char)*(*size_limit))
    if (holder == NULL) {
        fprintf(stderr, "Could not allocate enough memory.\n");
        return false;
    }
    free(new_path->path_to_file);
    new_path->path_to_file = holder;
    return true;
}

bool is_include(FILE* input, file_path *new_path) 
{
    char *pattern = "include ";
    for (int i = 0; i < strlen(pattern); i++0) {
        if (fgetc(input) != pattern[i]) return false;
    }
    char loaded;
    int size_counter = 0;
    int size_limit = 254;
    while ((loaded = fgetc(input)) != '\n' && loaded != EOF) {
        size_counter++;
        if (size_counter > size_limit) {
            if (!reallocate(new_path, &size_limit)) return false;
        }
        new_path->path_to_file[size_counter-1] = loaded;
    }
    new_path->path_to_file[size_counter] = '\0';
    return true;
}



bool convert(FILE *input, FILE *output, bool guard, bool report_cycles, int max_depth, char *prefix, used_paths used_files) 
{
    char loaded;
    while ((loaded = fgetc(input)) != EOF) {
        if (isspace(loaded) || loaded == '\n') continue;
        if (loaded == ';') {ignore_line(input); continue; }
        if (loaded == '[') {
            if (!sectionize(input, output, prefix)) return false;
            continue;
        }
        if (loaded == '.') {
            file_path *new_path;
            new_path->path_to_file = malloc(sizeof(char)*255);
            if (!is_include(input, new_path) || used_files->depth > max_depth) {
                return false;
            }
            if (!add_to_paths(new_path, used_files) && (guard || report_cycles) {
                fprintf(stderr, "Cycle in files detected!\n")
                return false;
            }
            FILE* new_input = fopen(buffer, "r");
            if (new_input == NULL) {
                fprintf(stderr, "Invalid input path after .include in file.\n");
                return false;
            }
            if (!convert(new_input, output, guard, report_cycles, max_depth, to_prefix(new_path->path_to_file), used_files)) {
                return false;
            }
        }
        if (strchr(ALLOWED_CHARS, loaded) != NULL) {
            if (!key_bind(input, output)) return false;
            continue;
        } else return false;
    }
}



int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(argv[0]);
            return EXIT_SUCCESS;
        }
    }

    char *help_msg = "Try --help for more information.\n";
    int i = 1;
    bool change = true, guard = false, report_cycles = false;
    int max_depth = 10;
    

    while (i < argc && change) {
        change = false;
        if (strcmp(argv[i], "--max-depth") == 0 || strcmp(argv[i], "-d") == 0) {
            int *max_depth_helper = malloc(sizeof(int));
            if (i+1 >= argc || sscanf(argv[i+1], "%d", max_depth_helper) == 0) {
                free(max_depth_helper);
                fprintf(stderr, "--max-depth and -d take a number as a next parameter! %s", help_msg);
                return EXIT_FAILURE;
            }
            max_depth = *max_depth_helper;
            free(max_depth_helper);
            change = true;
            i++;
        } else if (strcmp(argv[i], "--include-guard") == 0 || strcmp(argv[i], "-g") == 0) {
            change = true;
            guard = true;
        } else if (strcmp(argv[i], "--report-cycles") == 0 || strcmp(argv[i], "-r") == 0) {
            change = true;
            report_cycles = true;
        }
        i++;
    }

    if (i >= argc) {
        fprintf(stderr, "Input and output path not specified correctly! %s", help_msg);
        return EXIT_FAILURE;
    }
    FILE* input = fopen(argv[i-1], "r");
    FILE* output = fopen(argv[i], "w");

    if (input == NULL) {
        fprintf(stderr, "Invalid input path given. %s", help_msg);
        return EXIT_FAILURE;
    } else if (output == NULL || strchr(argv[i], ".ini") == NULL) {
        fprintf(stderr, "Invalid output path given. %s", help_msg);
        return EXIT_FAILURE;
    }

    if (max_depth < 0) {
        max_depth = INT_MAX;
    }
    used_paths *used_files;
    used_files->depth = 0;
    if (convert(input, output, guard, report_cycles, max_depth, "\0")) {
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;

}
