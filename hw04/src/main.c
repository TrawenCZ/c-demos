#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <limits.h>

#define ALLOWED_CHARS "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!-.:?_"
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

typedef struct linked_list_item 
{
    char *name;
    struct linked_list_item *next;
    int path_movement_for_files;
} linked_list_item;

typedef struct linked_list 
{
    linked_list_item *top;
    linked_list_item *bottom;
    int size;
} linked_list;


bool convert(FILE *input, FILE *output, bool guard, bool report_cycles, bool comments_and_empty_lines, int max_depth, 
             int curr_file_depth, char *prefix, linked_list *sections, linked_list *used_files);

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

void helper_printer(linked_list *list)
{
    linked_list_item *pNode = list->top;
    while (pNode != NULL) {
        printf("%s\n", pNode->name);
        pNode = pNode->next;
    }
}

void clear(linked_list *list)
{
    linked_list_item *pNode = list->top;
    while (pNode != NULL) {
        linked_list_item *pNextNode = pNode->next;
        free(pNode->name);
        free(pNode);
        pNode = pNextNode;
    }
    free(list);
}

bool reallocate(linked_list_item *new_item, int *size_limit)
{
    *size_limit += 255;
    void *holder = realloc(new_item->name, sizeof(char)*(*size_limit));
    if (holder == NULL) {
        free(new_item->name);
        fprintf(stderr, "Could not allocate enough memory.\n");
        return false;
    }
    new_item->name = holder;
    return true;
}

bool push_to_linked_list(linked_list_item *item, linked_list *list, bool check_path_depths) 
{
    if (list->top == NULL) {
        list->top = item;
        list->bottom = item;
        item->next = NULL;
    } else {
        linked_list_item *pPath = list->top;
        while (pPath != NULL) {
            if (strcmp(pPath->name, item->name) == 0) {
                if (check_path_depths && pPath->path_movement_for_files != item->path_movement_for_files) {
                    continue;
                }
                free(item->name);
                free(item);
                return false;
            }
            pPath = pPath->next;
        }
        item->next = NULL;
        list->bottom->next = item;
        list->bottom = item;
    }
    list->size++;
    return true;
}

bool sectionize(FILE *input, FILE *output, char *prefix, linked_list *sections) 
{
    char loaded;
    linked_list_item *new_section = malloc(sizeof(linked_list_item));
    new_section->name = malloc(sizeof(char)*255);
    int size_counter = 0;
    int size_limit = 253;
    char *err_msg = "Section not in correct format!\n";
    char *mmr_msg = "Could not allocate enought memory for one of the sections!";
    fprintf(output, "[%s", prefix);
    while ((loaded = fgetc(input)) != ']') {
        if (isspace(loaded)) continue;
        if (strchr(ALLOWED_CHARS, loaded) == NULL) {
            fprintf(stderr, "Unallowed characters in section name! Allowed characters: %s\n", ALLOWED_CHARS);
            return false;
        }
        if (size_counter == size_limit) {
            if (!reallocate(new_section, &size_limit)) {
                fprintf(stderr, "%s\n", mmr_msg);
                return false;
            }
        }
        new_section->name[size_counter] = loaded;
        size_counter++;
    }
    new_section->name[size_counter] = '\0';
    if (loaded != ']') {
        fprintf(stderr, "%s", err_msg);
        return false;
    }
    while ((loaded = fgetc(input)) != EOF && loaded != '\n') {
        if (isspace(loaded)) continue;
        fprintf(stderr, "%s", err_msg);
        return false;
    }
    if (!push_to_linked_list(new_section, sections, false)) {
        fprintf(stderr, "Duplicit sections detected! Or maybe cycle in .include.\n");
        return false;
    }
    fprintf(output, "%s", new_section->name);
    fprintf(output, "]\n");
    return true;
}

bool key_bind(FILE *input, FILE *output, linked_list *keys, char first_char) 
{   
    helper_printer(keys);
    linked_list_item *new_key = malloc(sizeof(linked_list_item));
    new_key->name = malloc(sizeof(char)*255);
    new_key->name[0] = first_char;
    int size_counter = 1;
    int size_limit = 253;
    char loaded;
    while ((loaded = fgetc(input)) != '=') {
        if (isspace(loaded)) continue;
        if (strchr(ALLOWED_CHARS, loaded) == NULL) {
            fprintf(stderr, "Key characters not in allowed range! Allowed characters: %s\n", ALLOWED_CHARS);
            free(new_key->name);
            free(new_key);
            return false;
        }
        if (size_counter == size_limit) {
            if (!reallocate(new_key, &size_limit)) {
                fprintf(stderr, "Could not allocate enough memory for one of the keys.\n");
                return false;
            }
        }
        new_key->name[size_counter] = loaded;
        size_counter++;
    }

    if (!push_to_linked_list(new_key, keys, false)) {
        fprintf(stderr, "Key already in section!\n");
        return false;
    }
    new_key->name[size_counter] = '\0';
    fprintf(output, "%s = ", new_key->name + 1);
    int loaded_long;
    while ((loaded_long = fgetc(input)) != EOF && loaded_long != '\n') {
        fprintf(output, "%c", loaded_long);
    }
    fprintf(output, "\n");
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
            previous_is_slash = false;
            new_prefix->name[size_counter] = '?';
            size_counter++;
        }
        if (size_counter == size_limit) {
            if (!reallocate(new_prefix, &size_limit)) return 0;
        }
    }
    new_prefix->name[size_counter - 3] = ':';
    new_prefix->name[size_counter - 2] = '\0';
    return depth_counter + 1;       // + 1 because of decreasing by 1 in .ini 
}

bool is_valid_path(linked_list_item *new_path)
{
    if (strcmp(new_path->name + (strlen(new_path->name) - 4), ".ini") != 0) {
        fprintf(stderr, "File in .include is not a valid .ini file!\n");
        return false;
    } else if (new_path->name[0] == '/' || new_path->name[0] == '\\' || new_path->name[0] == '~' ||
               strstr(new_path->name, ":\\") != NULL || strstr(new_path->name, ":/") != NULL) {
                   fprintf(stderr, "Path in .include is not a valid relative path!\n");
                   return false;
               }
    return true;
}

bool include_processer(FILE *input, FILE *output, bool guard, bool report_cycles, bool with_comments, int max_depth,
                       int curr_file_depth, linked_list *sections, linked_list *used_files)
{
    linked_list_item *new_path = malloc(sizeof(linked_list_item));
    new_path->name = malloc(sizeof(char)*255);

    if (!is_include(input, new_path) || used_files->size > max_depth) {
        if (used_files->size > max_depth) {
            fprintf(stderr, "Max depth exceeded! Try setting it higher for bigger tolerance.\n");
        } else { fprintf(stderr, "Invalid .include statement in file.\n"); }
        free(new_path->name);
        free(new_path);
        return false;
    }

    if (!is_valid_path(new_path)) {
        free(new_path->name);
        free(new_path);
        return false;
    }

    FILE* new_input = fopen(new_path->name, "r");
    if (new_input == NULL) {
        fprintf(stderr, "Cannot open file at path after '.include' statement in file.\n");
        free(new_path->name);
        free(new_path);
        return false;
    }
    
    linked_list_item *new_prefix = malloc(sizeof(linked_list_item));
    new_prefix->name = malloc(sizeof(char)*255);
    new_prefix->path_movement_for_files = to_prefix(new_path->name, new_prefix);
    free(new_path->name);
    free(new_path);

    if (!push_to_linked_list(new_prefix, used_files, true) && (guard || report_cycles)) {
        fprintf(stderr, "Cycle in .include statements detected!\n");
        fclose(new_input);
        return false;
    } else if (curr_file_depth + new_prefix->path_movement_for_files < 0) {
        fprintf(stderr, "Cannot go higher in filesystem than the level on which has program been launched.\n");
        fclose(new_input);
        return false;
    }
    if (!convert(new_input, output, guard, report_cycles, with_comments, max_depth, curr_file_depth + new_prefix->path_movement_for_files, new_prefix->name, sections, used_files)) {
        fclose(new_input);
        return false;
    }
    fclose(new_input);
    return true;
}

void copy_line(FILE* input, FILE* output) 
{
    int loaded;
    fprintf(output, ";");
    while ((loaded = fgetc(input)) != '\n' && loaded != EOF) {
        fprintf(output, "%c", loaded);
    }
    fprintf(output, "\n");
}

bool convert(FILE *input, FILE *output, bool guard, bool report_cycles, bool with_comments, int max_depth, 
             int curr_file_depth, char *prefix, linked_list *sections, linked_list *used_files) 
{
    linked_list *keys = malloc(sizeof(linked_list));
    keys->top = NULL;
    char loaded;
    bool section_is_set = false;
    while ((loaded = fgetc(input)) != EOF) {
        if (loaded == '\n') {
            if (with_comments) fprintf(output, "\n");
            continue;
        }
        if (isspace(loaded)) continue;
        
        if (loaded == ';') {
            if (with_comments) {
                copy_line(input, output);
            } else ignore_line(input);
            continue;
        }
        if (loaded == '[') {
            if (!sectionize(input, output, prefix, sections)) {
                clear(keys);
                return false;
            }
            section_is_set = true;
            clear(keys);
            keys = malloc(sizeof(linked_list));
            keys->top = NULL;
            continue;
        }
        if (loaded == '.') {
            if (!include_processer(input, output, guard, report_cycles, with_comments, max_depth, curr_file_depth, sections, used_files)) {
                clear(keys);
                return false;
            }
            section_is_set = false;
            continue;
        }
        if (strchr(ALLOWED_CHARS, loaded) != NULL) {
            if (!section_is_set) {
                fprintf(stderr, "Section is not set for some keys!\n");
                clear(keys);
                return false;
            }
            fprintf(output, "%c", loaded);
            if (!key_bind(input, output, keys, loaded)) {
                clear(keys);
                return false;
            }
            continue;
        } else {
            fprintf(stderr, "Unsupported key values! Supported key values: %s\n", ALLOWED_CHARS);
            clear(keys);
            return false;
        }
    }
    clear(keys);
    return true;
}

char* create_tmp_output(FILE *tmp_file)
{
    fclose(tmp_file);
    char *prefix = malloc(sizeof(char)*255);
    char *ini_postfix = ".ini";
    strcpy(prefix, "tmp.ini");
    int size_counter = 7;
    int size_limit = 254;
    while ((tmp_file = fopen(prefix, "r")) != NULL) {
        if (size_counter == size_limit) {
            size_limit += 255;
            prefix = realloc(prefix, size_limit);   // I know this is not the best way to do it, but I rely on enough memory in this case
                                                    // and really low chance of existence of so many tmp files
        }
        fclose(tmp_file);
        prefix[size_counter-4] = '1';
        strcpy(prefix + (size_counter - 3), ini_postfix);
        size_counter++;
    }
    return prefix;
}

void file_copy(FILE* input, FILE* output) 
{
    int loaded;
    while ((loaded = fgetc(input)) != EOF) {
        fprintf(output, "%c", loaded);
    }
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
    bool change = true, guard = false, report_cycles = false, with_comments = false;
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
        } else if (strcmp(argv[i], "--with-comments") == 0 || strcmp(argv[i], "-c") == 0) {
            change = true;
            with_comments = true;
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

    if (input == NULL) {
        fprintf(stderr, "Invalid input path given. %s", help_msg);
        return EXIT_FAILURE;
    } else if (strcmp(argv[i] + (strlen(argv[i]) - 4), ".ini") != 0) {
        fprintf(stderr, "Output file is not a valid .ini file. %s", help_msg);
        return EXIT_FAILURE;
    }

    if (max_depth < 0) {
        max_depth = INT_MAX;
    }

    bool prefix_is_mallocated = false;
    char *tmp_output_prefix = "tmp.ini";
    FILE *tmp_output = fopen(tmp_output_prefix, "r");
    if (tmp_output != NULL) {
        tmp_output_prefix = create_tmp_output(tmp_output);
        tmp_output = fopen(tmp_output_prefix, "w");
        prefix_is_mallocated = true;
    } else {
        tmp_output = fopen(tmp_output_prefix, "w");
    }

    linked_list *used_files = malloc(sizeof(linked_list));
    linked_list *sections = malloc(sizeof(linked_list));
    used_files->size = 0;
    used_files->top = NULL;
    sections->size = 0;
    sections->top = NULL;

    linked_list_item *new_prefix = malloc(sizeof(linked_list_item));
    new_prefix->name = malloc(sizeof(char)*255);
    new_prefix->path_movement_for_files = to_prefix(argv[i-1], new_prefix);
    push_to_linked_list(new_prefix, used_files, false);

    int return_val = EXIT_FAILURE;
    if (convert(input, tmp_output, guard, report_cycles, with_comments, max_depth, 0, "", sections, used_files)) {
        FILE* output = fopen(argv[i], "w");
        if (output == NULL) {
            fprintf(stderr, "Could not open output file!\n");
        } else {
            fclose(tmp_output);
            tmp_output = fopen(tmp_output_prefix, "r");
            file_copy(tmp_output, output);
            fclose(output);
            return_val = EXIT_SUCCESS;
            printf("Successfuly converted!\n");
        }
    }

    clear(used_files);
    clear(sections);
    fclose(tmp_output);
    remove(tmp_output_prefix);
    if (prefix_is_mallocated) free(tmp_output_prefix);
    fclose(input);

    return return_val;

}
