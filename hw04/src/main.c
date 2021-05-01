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

typedef struct linked_list_item 
{
    char *name;
    struct linked_list_item *next;
} linked_list_item;

typedef struct linked_list 
{
    linked_list_item *top;
    linked_list_item *bottom;
    int size;
} linked_list;

static void usage(FILE *output, const char *program)
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

    fprintf(output, help, program, program);
}

void ignore_line(FILE *input) 
{
    char loaded;
    while ((loaded = fgetc(input)) != EOF && loaded != '\n') {
        continue;
    }
}

bool sectionize(FILE *input, FILE *output, char *prefix) 
{
    char loaded;
    char *err_msg = "Section not in correct format!\n";
    fprintf(output, "[%s", prefix);
    while ((loaded = fgetc(input)) != EOF && loaded != '\n' && loaded != ']') {
        if (isspace(loaded)) continue;
        if (strchr(ALLOWED_CHARS, loaded) == NULL) return false;
        fprintf(output, "%c", loaded);
    }
    if (loaded != ']') {
        fprintf(stderr, "%s", err_msg);
        return false;
    }
    while ((loaded = fgetc(input)) != EOF && loaded != '\n') {
        if (isspace(loaded)) continue;
        fprintf(stderr, "%s", err_msg);
        return false;
    }
    fprintf(output, "]\n");
    return true;
}

bool key_bind(FILE *input, FILE *output) 
{   
    char loaded;
    while ((loaded = fgetc(input)) != '=') {
        if (isspace(loaded)) continue;
        if (strchr(ALLOWED_CHARS, loaded) == NULL) {
            fprintf(stderr, "Key characters not in allowed range! Allowed characters: %s\n", ALLOWED_CHARS);
            return false;
        }
        fprintf(output, "%c", loaded);
    }
    fprintf(output, " = ");
    int loaded_long;
    while ((loaded_long = fgetc(input)) != EOF && loaded_long != '\n') {
        fprintf(output, "%c", loaded_long);
    }
    fprintf(output, "\n");
    return true;
}

bool add_to_paths(linked_list_item *new_path, linked_list *paths) 
{
    if (paths->top == NULL) {
        paths->top = new_path;
        paths->bottom = new_path;
    } else {
        linked_list_item *pPath = paths->top;
        while (pPath != NULL) {
            if (pPath->name == new_path->name) return false;
            pPath = pPath->next;
        }
        new_path->next = NULL;
        paths->bottom->next = new_path;
        paths->bottom = new_path;
    }
    paths->size++;
    return true;
}

bool reallocate(linked_list_item *new_path, int *size_limit)
{
    *size_limit += 255;
    void *holder = realloc(new_path->name, sizeof(char)*(*size_limit));
    if (holder == NULL) {
        fprintf(stderr, "Could not allocate enough memory.\n");
        return false;
    }
    free(new_path->name);
    new_path->name = holder;
    return true;
}

bool is_include(FILE* input, linked_list_item *new_path) 
{
    char *pattern = "include ";
    for (int i = 0; i < (int) strlen(pattern); i++) {
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
        new_path->name[size_counter-1] = loaded;
    }
    new_path->name[size_counter] = '\0';
    return true;
}

int to_prefix(char* path, linked_list_item *new_prefix)
{
    int size_counter = 0;
    int size_limit = 254;
    int depth_counter = 0;
    bool previous_is_slash = false;
    for (int i = 0; i < (int) strlen(path); i++) {
        if (path[i] == '/' || path[i] == '\\') {
            if (previous_is_slash) continue;
            depth_counter++;
            new_prefix->name[size_counter] = ':';
            size_counter++;
            previous_is_slash = true;
        } else if (path[i] == '.') {
            depth_counter--;
            continue;
        } else if (strchr(ALLOWED_CHARS, path[i]) != NULL) {
            previous_is_slash = false;
            new_prefix->name[size_counter] = path[i];
            size_counter++;
        } else {
            new_prefix->name[size_counter] = '?';
            size_counter++;
        }
        if (size_counter == size_limit) {
            if (!reallocate(new_prefix, &size_limit)) return 0;
        }
    }
    new_prefix->name[size_counter-3] = '\0';
    return depth_counter;
}

bool is_valid_path(linked_list_item *new_path)
{
    if (strcmp(new_path->name[(int) strlen(new_path->name) - 4], ".ini") != 0) {
        fprintf(stderr, "File in .include is not a valid .ini file!\n");
        return false;
    } else if (new_path->name[0] == '/' || new_path->name[0] == '\\' || new_path->name[0] == '~' ||
               strstr(new_path->name, ":\\") != NULL || strstr(new_path->name, ":/") != NULL) {
                   fprintf(stderr, "Path in .include is not a valid relative path!\n");
                   return false;
               }
    return true;
}



bool convert(FILE *input, FILE *output, bool guard, bool report_cycles, int max_depth, int curr_file_depth, char *prefix, linked_list *used_files) 
{
    char loaded;
    bool section_is_set = false;
    while ((loaded = fgetc(input)) != EOF) {
        if (isspace(loaded) || loaded == '\n') continue;
        if (loaded == ';') {ignore_line(input); continue; }
        if (loaded == '[') {
            if (!sectionize(input, output, prefix)) return false;
            section_is_set = true;
            continue;
        }
        if (loaded == '.') {
            linked_list_item *new_path = malloc(sizeof(linked_list_item));
            new_path->name = malloc(sizeof(char)*255);

            if (!is_include(input, new_path) || used_files->size > max_depth) {
                return false;
            }
            if (!add_to_paths(new_path, used_files) && (guard || report_cycles)) {
                fprintf(stderr, "Cycle in files detected!\n");
                return false;
            } else if (!is_valid_path(new_path)) return false;

            FILE* new_input = fopen(new_path->name, "r");
            if (new_input == NULL) {
                fprintf(stderr, "Cannot open file at path after '.include' statement in file.\n");
                return false;
            }
            linked_list_item *new_prefix = malloc(sizeof(linked_list_item));
            new_prefix->name = malloc(sizeof(char)*255);
            int depth_movement = to_prefix(new_path->name, new_prefix);
            if (curr_file_depth + depth_movement < 0) {
                fprintf(stderr, "Cannot go higher in filesystem than the level on which has program been launched.\n");
                return false;
            }
            if (!convert(new_input, output, guard, report_cycles, max_depth, curr_file_depth + depth_movement, new_prefix->name, used_files)) {
                free(new_prefix->name);
                free(new_prefix);
                return false;
            }
            free(new_prefix->name);
            free(new_prefix);
        }
        if (strchr(ALLOWED_CHARS, loaded) != NULL) {
            if (!section_is_set) {
                fprintf(stderr, "Section is not set for some keys!\n");
                return false;
            }
            fprintf(output, "%c", loaded);
            if (!key_bind(input, output)) return false;
            continue;
        } else return false;
    }
    return true;
}


int main(int argc, char **argv)
{
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            usage(stdout, argv[0]);
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
        } else if (argv[i][0] == '-') {
            fprintf(stderr, "Ivalid operator in function arguments!\n");
            usage(stderr, argv[0]);
            return EXIT_FAILURE;
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
    } else if (output == NULL || strcmp(argv[(int) strlen(argv[i]) - 4], ".ini") != 0) {
        fprintf(stderr, "Invalid output path given. %s", help_msg);
        return EXIT_FAILURE;
    }

    if (max_depth < 0) {
        max_depth = INT_MAX;
    }
    linked_list *used_files = malloc(sizeof(linked_list));
    linked_list *prefixes = malloc(sizeof(linked_list));
    used_files->size = 0;
    prefixes->size = 0;
    if (convert(input, output, guard, report_cycles, max_depth, 0, "", used_files)) {
        return EXIT_SUCCESS;
    }

    fclose(input);
    fclose(output);

    return EXIT_FAILURE;

}
