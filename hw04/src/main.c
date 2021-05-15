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
    struct linked_list_item *prev;
    int path_movement_for_files;
} linked_list_item;

typedef struct linked_list 
{
    linked_list_item *top;
    linked_list_item *bottom;
    int size;
} linked_list;

typedef struct buffer {
    char *text;
    int size;
    int size_limit;
} buffer;


bool convert(FILE *input, buffer *output, bool guard, bool report_cycles, bool with_comments, int max_depth, 
             linked_list_item *new_path, int size_limit, char *prefix, linked_list *sections, linked_list *used_files);

static void usage(FILE *output, const char *program)
{
    const char *help =
            "Usage: %s OPTIONS\n"
            "\n"
            "This program loads a .ini file and converts him with following options:\n"
            "\n"
            "\t-h\t--help\t\t\tShow this helpful message (and ends program)\n"
            "\t-d N\t--max-depth N\t\tSets maximum depth immersion limit [N]. ([N=0] = no immersion, negative N = no limit)\n"
            "\t-g\t--include-guard\t\tProtects againts duplicate file copy.\n"
            "\t-r\t--report-cycles\t\tReports and ends function if cycle in .include statements detected.\n"
            "\t-c\t--with-comments\t\tKeeps blank linkes and comments in output file.\n"
            "\n Arguments should be passed like on this pattern: ./program [options] [path to input file] [path to output file]\n"
            "\n";

    fprintf(output, help, program, program);
}

bool buffer_realloc(buffer *buffer_to_realloc)
{
    buffer_to_realloc->size_limit += 255;
    char *holder = realloc(buffer_to_realloc->text, sizeof(char)*buffer_to_realloc->size_limit);
    if (holder == NULL) {
        fprintf(stderr, "Could not allocate enough memory for storing .ini file(s) content.\n");
        return false;
    }
    buffer_to_realloc->text = holder;
    return true;
}

bool bcprintf(buffer *output, char char_to_print)
{
    if (output->size + 2 >= output->size_limit && !buffer_realloc(output)) return false;
    output->text[output->size] = char_to_print;
    output->size++;
    output->text[output->size] = '\0';
    return true;
}

bool bsprintf(buffer *output, char *string)
{
    while ((int) (output->size + strlen(string) + 1) >= output->size_limit) {
        if (!buffer_realloc(output)) return false;
    }
    strcpy(output->text + output->size, string);
    output->size += strlen(string);
    return true;
}

bool malloc_failed(void) {
    fprintf(stderr, "Could not allocate enough memory!\n");
    return false;
}

void ignore_line(FILE *input) 
{
    char loaded;
    while ((loaded = fgetc(input)) != EOF && loaded != '\n') {
        continue;
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

bool path_creator(char *raw_path, linked_list_item *output_path, int size_limit) {
    int i = strlen(raw_path) - 1;
    if (i == -1) {
        output_path->name[0] = '\0';
        return true;
    }

    while (i >= 0 && raw_path[i] != '/' && raw_path[i] != '\\') {
        i--;
    }
    while ((int) strlen(raw_path) > size_limit) {
        if (!reallocate(output_path, &size_limit)) return false;
    }
    strncpy(output_path->name, raw_path, i+1);
    output_path->name[i+1] = '\0';
    return true;
}

bool push_to_linked_list(linked_list_item *item, linked_list *list, bool should_check) 
{
    if (list->top == NULL) {
        list->top = item;
        list->bottom = item;
        item->next = NULL;
        item->prev = NULL;
        list->size++;
        return true;
    } else if (should_check) {
        linked_list_item *pPath = list->top;
        while (pPath != NULL) {
            if (strcmp(pPath->name, item->name) == 0) {
                free(item->name);
                free(item);
                return false;
            }
            pPath = pPath->next;
        }
    }
    item->next = NULL;
    item->prev = list->bottom;
    list->bottom->next = item;
    list->bottom = item;
    list->size++;
    return true;
}

bool sectionize(FILE *input, buffer *output, char *prefix, linked_list *sections) 
{
    char loaded;
    char *err_msg = "Section not in correct format!\n";
    if (!bcprintf(output, '[')) return false;

    while (isspace(loaded = fgetc(input)) && loaded != '\n' && loaded != EOF) {
        continue;
    }

    if (loaded == '\n' || loaded == EOF) {
        fprintf(stderr, "%s", err_msg);
        return false;
    }

    linked_list_item *new_section = malloc(sizeof(linked_list_item));
    if (new_section == NULL) return malloc_failed();
    new_section->name = malloc(sizeof(char)*255);
    if (new_section->name == NULL) {free(new_section); return malloc_failed(); }

    int size_counter = 0;
    int size_limit = 253;
    bool space_found = false;
    char *mmr_msg = "Could not allocate enought memory for one of the sections!";

    while ((int) strlen(prefix) > size_limit) {
        if (!reallocate(new_section, &size_limit)) {
            fprintf(stderr, "%s", mmr_msg);
        }
    }

    strcpy(new_section->name, prefix);
    size_counter += strlen(prefix);
    new_section->name[size_counter] = loaded;
    size_counter++;

    while ((loaded = fgetc(input)) != ']') {
         if (isspace(loaded) && loaded != '\n' && loaded != EOF) {
            space_found = true;
            continue;
        }
        if (strchr(ALLOWED_CHARS, loaded) == NULL) {
            fprintf(stderr, "Unallowed characters in section name! Allowed characters: %s\n", ALLOWED_CHARS);
            free(new_section->name);
            free(new_section);
            return false;
        }
        if (size_counter == size_limit) {
            if (!reallocate(new_section, &size_limit)) {
                fprintf(stderr, "%s\n", mmr_msg);
                free(new_section->name);
                free(new_section);
                return false;
            }
        }
        if (space_found) {
            fprintf(stderr, "Found white space in section name!\n");
            free(new_section->name);
            free(new_section);
            return false;
        }
        new_section->name[size_counter] = loaded;
        size_counter++;
    }
    new_section->name[size_counter] = '\0';
    if (loaded != ']' || size_counter == 0) {
        fprintf(stderr, "%s", err_msg);
        free(new_section->name);
        free(new_section);
        return false;
    }
    while ((loaded = fgetc(input)) != EOF && loaded != '\n') {
        if (isspace(loaded)) continue;
        fprintf(stderr, "%s", err_msg);
        free(new_section->name);
        free(new_section);
        return false;
    }
    if (!push_to_linked_list(new_section, sections, true)) {
        fprintf(stderr, "Duplicit sections detected! Or maybe cycle in .include statements.\n");
        return false;
    }
    if (!bsprintf(output, new_section->name)) return false;
    if (!bsprintf(output, "]\n")) return false;
    return true;
}

bool key_bind(FILE *input, buffer *output, linked_list *keys, char first_char) 
{   
    linked_list_item *new_key = malloc(sizeof(linked_list_item));
    if (new_key == NULL) return malloc_failed();
    new_key->name = malloc(sizeof(char)*255);
    if (new_key->name == NULL) {free(new_key); return malloc_failed(); }
    new_key->name[0] = first_char;
    bool space_found = false;
    int size_counter = 1;
    int size_limit = 253;
    char loaded;    
    while ((loaded = fgetc(input)) != '=' && loaded != '\n' && loaded != EOF) {
        if (isspace(loaded)) {
            space_found = true;
            continue;
        }
        if (strchr(ALLOWED_CHARS, loaded) == NULL) {
            fprintf(stderr, "Key characters not in allowed range! Allowed characters: %s\n", ALLOWED_CHARS);
            free(new_key->name);
            free(new_key);
            return false;
        }
        if (size_counter == size_limit) {
            if (!reallocate(new_key, &size_limit)) {
                fprintf(stderr, "Could not allocate enough memory for one of the keys.\n");
                free(new_key->name);
                free(new_key);
                return false;
            }
        }
        if (space_found) {
            fprintf(stderr, "Detected white space inside of key!\n");
            free(new_key->name);
            free(new_key);
            return false;
        }
        new_key->name[size_counter] = loaded;
        size_counter++;
    }
    if (loaded == '\n' || loaded == EOF) {
        fprintf(stderr, "Key value not specified correctly!\n");
        free(new_key->name);
        free(new_key);
        return false;
    }

    if (!push_to_linked_list(new_key, keys, true)) {
        fprintf(stderr, "Key already in section!\n");
        return false;
    }
    
    new_key->name[size_counter] = '\0';
    if (!bsprintf(output, new_key->name + 1)) return false;
    if (!bsprintf(output, " = ")) return false;

    while (isspace(loaded = fgetc(input)) && loaded != '\n') {
        continue;
    }
    if (!bcprintf(output, loaded)) return false;
    if (loaded == '\n') return true;

    int num_of_spaces = 0;
    while ((loaded = fgetc(input)) != EOF && loaded != '\n') {
        if (isspace(loaded)) {
            num_of_spaces++;
        } else {
            while (num_of_spaces != 0) {
                if (!bcprintf(output, ' ')) return false;
                num_of_spaces--;
            }
            if (!bcprintf(output, loaded)) return false;
        }
    }
    if (!bcprintf(output, '\n')) return false;
    return true;
}

bool is_include(FILE* input, linked_list_item *new_path, int init_write_index) 
{
    char *pattern = "include ";
    for (int i = 0; i < (int) strlen(pattern); i++) {
        if (fgetc(input) != pattern[i]) return false;
    }
    char loaded;
    while (isspace(loaded = fgetc(input)) && loaded != '\n' && loaded != EOF) continue;
    if (loaded == '\n' || loaded == EOF) {
        fprintf(stderr, "Path after .include statement is empty!\n");
        return false;
    }
    new_path->name[init_write_index] = loaded;
    int size_counter = 1;
    int size_limit = 254;
    while ((loaded = fgetc(input)) != '\n' && loaded != EOF) {
        size_counter++;
        init_write_index++;
        if (size_counter == size_limit) {
            if (!reallocate(new_path, &size_limit)) return false;
        }
        new_path->name[init_write_index] = loaded;
    }
    new_path->name[init_write_index + 1] = '\0';
    return true;
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

void pop_from_linked_list(linked_list *list)
{
    if (list->size <= 0) {
        list->size--;
        return;
    }
    linked_list_item *pNewBottom = list->bottom->prev;
    if (pNewBottom == NULL) {
        list->top = NULL;
        free(list->bottom->name);
        free(list->bottom);
        list->bottom = NULL;
    } else {
        free(pNewBottom->next->name);
        free(pNewBottom->next);
        pNewBottom->next = NULL;
        list->bottom = pNewBottom;
    }
    list->size--;
}

bool to_prefix_rec(char *path, int index, linked_list *prefixed_path, int depth_limit, bool should_control)
{
    bool only_dot = true;
    int dot_count = 0;
    linked_list_item *prefixed_path_item = malloc(sizeof(linked_list_item));
    if (prefixed_path_item == NULL) return malloc_failed();
    int size_limit = 253;
    int size_counter = 0;
    prefixed_path_item->name = malloc(sizeof(char)*255);
    if (prefixed_path_item == NULL) {free(prefixed_path_item); return malloc_failed(); }

    while (path[index] != '/' && path[index] != '\\' && path[index] != '\0') {
        if (size_counter == size_limit) {
            if (!reallocate(prefixed_path_item, &size_limit)) {
                fprintf(stderr, "Could not allocate enough memory for prefix!\n");
                free(prefixed_path_item);
                return false;
            }
        }
        if (path[index] == '.') {
            dot_count++;
            prefixed_path_item->name[size_counter] = path[index];  
        } else {
            only_dot = false;
            if (strchr(ALLOWED_CHARS, path[index]) != NULL) {
                prefixed_path_item->name[size_counter] = path[index];
            } else {
                prefixed_path_item->name[size_counter] = '?';
            }
        }
    size_counter++;
    index++;
    }

    if (only_dot && dot_count == 2) {
        pop_from_linked_list(prefixed_path);
        free(prefixed_path_item->name);
        free(prefixed_path_item);
        if (prefixed_path->size < depth_limit && should_control) {
            fprintf(stderr, "Detected step above root directory! (Root directory is where input file is.)\n");   
            return false;
        } 
    } else if (only_dot && dot_count == 1) {
        free(prefixed_path_item->name);
        free(prefixed_path_item);
    } else {
        prefixed_path_item->name[size_counter] = ':';
        prefixed_path_item->name[size_counter + 1] = '\0';
        push_to_linked_list(prefixed_path_item, prefixed_path, false);

    }


    if (path[index] != '\0') {
        while (path[index] == '/' || path[index] == '\\') { index++; }
        return to_prefix_rec(path, index, prefixed_path, depth_limit, should_control);
    }

    prefixed_path_item->name[size_counter - 4] = ':';
    prefixed_path_item->name[size_counter - 3] = '\0';
    return true;
}

bool to_prefix(char* path, linked_list_item *new_prefix, int *new_size, int depth_limit, bool should_control)
{
    linked_list *prefixed_path = malloc(sizeof(linked_list));
    int size_limit = 254;
    int size_counter = 0;
    prefixed_path->size = 0;
    prefixed_path->top = NULL;

    if (!to_prefix_rec(path, 0, prefixed_path, depth_limit, should_control)) {clear(prefixed_path); return false; }
    
    linked_list_item *pItem = prefixed_path->top;
    while (pItem != NULL) {
        while ((int) (size_counter + strlen(pItem->name)) >= size_limit) {
            if (!reallocate(new_prefix, &size_limit)) {
                fprintf(stderr, "Could not allocate enough memory for prefix!\n");
                return false;
            }
        }
        strcpy(new_prefix->name + size_counter, pItem->name);
        size_counter += strlen(pItem->name);
        pItem = pItem->next;
    }
    if (new_size != NULL) {
        *new_size = prefixed_path->size;
    }

    clear(prefixed_path);
    return true;

}


bool include_processer(FILE *input, buffer *output, bool guard, bool report_cycles, bool with_comments, int max_depth,
                       linked_list_item *path_prefix, int depth_limit, linked_list *sections, linked_list *used_files)
{
    linked_list_item *new_path = malloc(sizeof(linked_list_item));
    if (new_path == NULL) return malloc_failed();
    new_path->name = malloc(sizeof(char)*255 + sizeof(char)*strlen(path_prefix->name));
    if (new_path->name == NULL) {free(new_path); return malloc_failed(); }
    strcpy(new_path->name, path_prefix->name);

    if (!is_include(input, new_path, strlen(path_prefix->name)) || used_files->size > max_depth) {
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
    if (new_prefix == NULL) return malloc_failed();
    new_prefix->name = malloc(sizeof(char)*255);
    if (new_prefix->name == NULL) {free(new_prefix); return malloc_failed(); }
    
    if (!to_prefix(new_path->name, new_prefix, NULL, depth_limit, true)) {
        free(new_path->name);
        free(new_path);
        free(new_prefix->name);
        free(new_prefix);
        fclose(new_input);
        return false;
    } else {
        to_prefix(new_path->name + strlen(path_prefix->name), new_prefix, NULL, depth_limit, true);   //Im really sorry but at this point Im desperate at midnight :(
    }

    linked_list_item *new_path_prefix = malloc(sizeof(linked_list_item));
    if (new_path_prefix == NULL) { free(new_path->name); free(new_path); return malloc_failed();}
    new_path_prefix->name = malloc(sizeof(char)*255);
    if (new_path_prefix->name == NULL) { free(new_path->name); free(new_path); free(new_path_prefix); return malloc_failed();}
    path_creator(new_path->name, new_path_prefix, 254);

    free(new_path->name);
    free(new_path);

    if (!push_to_linked_list(new_prefix, used_files, (guard || report_cycles))) {
        if (report_cycles) {
            fprintf(stderr, "Cycle in .include statements detected!\n");
            fclose(new_input);
            free(new_path_prefix->name);
            free(new_path_prefix);
            return false;
        } else if (guard) {
            fclose(new_input);
            free(new_path_prefix->name);
            free(new_path_prefix);
            return true;
        }
    }
    if (!convert(new_input, output, guard, report_cycles, with_comments, max_depth, new_path_prefix, depth_limit, new_prefix->name, sections, used_files)) {
        fclose(new_input);
        free(new_path_prefix->name);
        free(new_path_prefix);
        return false;
    }
    if (!guard) {
        pop_from_linked_list(used_files);
    }
    free(new_path_prefix->name);
    free(new_path_prefix);
    fclose(new_input);
    return true;
}

bool copy_line(FILE* input, buffer *output) 
{
    int loaded;
    if (!bcprintf(output, ';')) return false;
    while ((loaded = fgetc(input)) != '\n' && loaded != EOF) {
        if (!bcprintf(output, loaded)) return false;
    }
    if (!bcprintf(output, '\n')) return false;
    return true;
}


bool convert(FILE *input, buffer *output, bool guard, bool report_cycles, bool with_comments, int max_depth, 
             linked_list_item *new_path, int depth_limit, char *prefix, linked_list *sections, linked_list *used_files)
{
    linked_list *keys = malloc(sizeof(linked_list));
    if (keys == NULL) return malloc_failed();
    keys->top = NULL;
    char loaded;
    bool section_is_set = false;
    while ((loaded = fgetc(input)) != EOF) {
        if (loaded == '\n') {
            if (with_comments) bcprintf(output, '\n');
            continue;
        }
        if (isspace(loaded)) continue;
        
        if (loaded == ';') {
            if (with_comments && !copy_line(input, output)) return false;
            else ignore_line(input);
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
            if (keys == NULL) return malloc_failed();
            keys->top = NULL;
            continue;
        }
        if (loaded == '.') {
            if (!include_processer(input, output, guard, report_cycles, with_comments, 
                                   max_depth, new_path, depth_limit, sections, used_files)) {
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
            if (!bcprintf(output, loaded)) return false;
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
            if (i+1 >= argc || sscanf(argv[i+1], "%d", &max_depth) == 0) {
                fprintf(stderr, "--max-depth and -d take a number as a next parameter! %s", help_msg);
                return EXIT_FAILURE;
            }
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

    linked_list *used_files = malloc(sizeof(linked_list));
    linked_list *sections = malloc(sizeof(linked_list));
    if (used_files == NULL || sections == NULL) {
        fprintf(stderr, "Could not allocate enough memory for storing file paths or sections.\n");
        if (used_files != NULL) free(used_files);
        return EXIT_FAILURE;
    };
    used_files->size = 0;
    used_files->top = NULL;
    sections->size = 0;
    sections->top = NULL;

    linked_list_item *new_prefix = malloc(sizeof(linked_list_item));
    new_prefix->name = malloc(sizeof(char)*255);
    if (new_prefix == NULL || new_prefix->name == NULL) {
        fprintf(stderr, "Could not allocate enough memory for file prefix.\n");
        free(used_files);
        free(sections);
        if (new_prefix != NULL) free(new_prefix);
        return EXIT_FAILURE;
    };

    int *new_size = malloc(sizeof(int));
    to_prefix(argv[i-1], new_prefix, new_size, 0, false);
    push_to_linked_list(new_prefix, used_files, true);
    *new_size = *new_size - 1;

    linked_list_item *new_path = malloc(sizeof(linked_list_item));
    new_path->name = malloc(sizeof(char)*255);
    path_creator(argv[i-1], new_path, 254);


    buffer *tmp_output = malloc(sizeof(buffer));
    tmp_output->text = malloc(sizeof(char)*255);
    tmp_output->size = 0;
    tmp_output->size_limit = 254;


    int return_val = EXIT_FAILURE;
    if (convert(input, tmp_output, guard, report_cycles, with_comments, max_depth, 
                new_path, *new_size, "", sections, used_files)) {
        FILE* output = fopen(argv[i], "w");
        if (output == NULL) {
            fprintf(stderr, "Could not open output file!\n");
        } else {
            fprintf(output, "%s", tmp_output->text);
            fclose(output);
            return_val = EXIT_SUCCESS;
        }
    }

    free(tmp_output->text);
    free(tmp_output);
    free(new_path->name);
    free(new_path);
    free(new_size);
    clear(used_files);
    clear(sections);
    fclose(input);

    return return_val;

}
