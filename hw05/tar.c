#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>

typedef struct dynamic_list
{
    char **data;
    int size;
    int limit_size;
} dynamic_list;

bool is_regular_directory(const char *path)
{
    struct stat path_stat;
    if (stat(path, &path_stat) == -1) return false;
    return S_ISDIR(path_stat.st_mode);
}

bool append(dynamic_list *list, char *prefix, char *element) {
    char *err_msg = "Could not allocate enough memory for storing names and paths!\n";

    if (list->size + 1 > list->limit_size) {
        list->limit_size += 20;
        char **holder = realloc(list->data, __SIZEOF_POINTER__ * list->limit_size);
        if (holder == NULL) {
            fprintf(stderr, "%s", err_msg);
            return false;
        }
        list->data = holder;
    }

    if (strlen(prefix) + strlen(element) + 1 > 254) return true;  // + 1 is for '/' character between prefix and element
    
    list->data[list->size] = malloc(sizeof(char)*255);
    list->data[list->size][0] = '\0';
    if (list->data[list->size] == NULL) {
        fprintf(stderr, "%s", err_msg);
        return false;
    }

    strcat(list->data[list->size], prefix);
    if (strcmp(element, "") != 0) {
        strcat(list->data[list->size], "/");
        strcat(list->data[list->size], element);
    }
    list->size++;
    return true;
}

bool list_processer(dynamic_list *todo_list, int start_index)
{
    int current_size_limit = todo_list->size;
    for (int i = start_index; i < current_size_limit; i++) {
        if (is_regular_directory(todo_list->data[i])) {
            int new_start_index = todo_list->size;
            DIR *directory = opendir(todo_list->data[i]);
            if (directory == NULL) continue;
            struct dirent *dir_content;
            while ((dir_content = readdir(directory)) != NULL) {
                if (strcmp(dir_content->d_name, "..") == 0 || strcmp(dir_content->d_name, ".") == 0) continue;
                if (!append(todo_list, todo_list->data[i], dir_content->d_name)) return false;
            }
            free(directory);
            if (!list_processer(todo_list, new_start_index)) return false;
        }
    }
    return true;
}

void clear(dynamic_list *list) {
    for (int i = 0; i < list->size; i++) {
        free(list->data[i]);
    }
    free(list->data);
    free(list);
}

void split_to_prefix(char *input, char *prefix, char *file)
{
    int pointer = strlen(input) - 100;
    while (input[pointer] != '/' && input[pointer] == '\0') {
        pointer++;
    }
    if (input[pointer] == '\0') {
        strncpy(prefix, input, 155);
        prefix[pointer - 1] = '\0';
        file = "";
        return;
    }
    strncpy(prefix, input, 155);
    prefix[pointer - 1] = '\0';
    file = input[pointer + 1];
}

bool load_info(dynamic_list *list, bool should_print, char *output)
{   
    struct stat path_stat;
    for (int i = 0; i < list->size; i++) {
        if (should_print) puts(list->data[i]);
        if (stat(list->data[i], &path_stat) == -1) {
            fprintf(stderr, "Could not access file '%s'\n", list->data[i]);
            continue;
        }
        int file = open(list->data[i], O_RDONLY);
        if (file == -1) {
            fprintf(stderr, "Could not open file '%s'\n", list->data[i]);
            continue;
        }

        char new_header[512] = {0};
        
        char *prefix = malloc(155);
        prefix[0] = '\0';
        char *file_name = list->data[i];
        if (strlen(list->data[i]) > 99) {
            split_to_prefix(list->data[i], prefix, file_name);
        }
        
        strcpy(new_header[0], file_name);


    }
}


int main_failed(dynamic_list *list)
{
    clear(list);
    return EXIT_FAILURE;
}

int cmpstr(void const *pStr1, void const *pStr2) { 
    return strcmp(*(char const **) pStr1, *(char const **) pStr2);
}

int main(int argc, char **argv)
{
    if (argc < 4) {
        fprintf(stderr, "Not enough arguments given!\n");
        return EXIT_FAILURE;
    }

    bool create = false, extract = false, print = false;
    for (int i = 0; i < (int) strlen(argv[1]); i++) {
        if (argv[1][i] == 'c') {
            create = true;
        } else if (argv[1][i] == 'x') {
            extract = true;
        } else if (argv[1][i] == 'v') {
            print = true;
        } else {
            fprintf(stderr, "Unallowed switches! Allowed: c, x, v\n");
            return EXIT_FAILURE;
        }
    }

    if (create && extract) {
        fprintf(stderr, "Cannot extract and create at the same time!\n");
        return EXIT_FAILURE;
    }

    struct stat path_stat;
    dynamic_list *todo_list = malloc(sizeof(dynamic_list));
    todo_list->size = 0;
    todo_list->limit_size = 20;
    todo_list->data = malloc(__SIZEOF_POINTER__ * 20);
    for (int i = 3; i < argc; i++) {
        if (!append(todo_list, argv[i], "")) return main_failed(todo_list);
    }
    if (!list_processer(todo_list, 0)) return main_failed(todo_list);

    qsort(todo_list->data, todo_list->size, __SIZEOF_POINTER__, cmpstr);

    /*
    for (int i = 0; i < todo_list->size; i++) {
        puts(todo_list->data[i]);
    }
    */

    int output = open(argv[2], O_WRONLY);
    if (output == -1) {
        fprintf(stderr, "Cannot open given output!\n");
        return main_failed(todo_list);
    }
    if (create && !load_info(todo_list, print, output)) return main_failed(todo_list); // argv[2] is output file

    clear(todo_list);
    return EXIT_SUCCESS;
}
