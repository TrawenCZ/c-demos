#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <fcntl.h>
#include <pwd.h>
#include <grp.h>
#include <utime.h>

/*
                                     _ _ _  __        ___    ____  _   _ ___ _   _  ____   _ _ _ 
                                    | | | | \ \      / / \  |  _ \| \ | |_ _| \ | |/ ___| | | | |
                                    | | | |  \ \ /\ / / _ \ | |_) |  \| || ||  \| | |  _  | | | |
                                    |_|_|_|   \ V  V / ___ \|  _ <| |\  || || |\  | |_| | |_|_|_|
                                    (_|_|_)    \_/\_/_/   \_\_| \_\_| \_|___|_| \_|\____| (_|_|_)
 
  ____            _                 _                             _                             _        _          _               
 | __ ) _ __ __ _(_)_ __         __| | __ _ _ __ ___   __ _  __ _(_)_ __   __ _    ___ ___   __| | ___  | |__   ___| | _____      __
 |  _ \| '__/ _` | | '_ \ _____ / _` |/ _` | '_ ` _ \ / _` |/ _` | | '_ \ / _` |  / __/ _ \ / _` |/ _ \ | '_ \ / _ \ |/ _ \ \ /\ / /
 | |_) | | | (_| | | | | |_____| (_| | (_| | | | | | | (_| | (_| | | | | | (_| | | (_| (_) | (_| |  __/ | |_) |  __/ | (_) \ V  V / 
 |____/|_|  \__,_|_|_| |_|      \__,_|\__,_|_| |_| |_|\__,_|\__, |_|_| |_|\__, |  \___\___/ \__,_|\___| |_.__/ \___|_|\___/ \_/\_/  
                                                            |___/         |___/                                                     
*/

#define POINTER_SIZE 8


typedef struct dynamic_list
{
    char **data;
    int size;
    int limit_size;
} dynamic_list;

struct metadata
{
    char *path_to_file;
    int mode;
    int file_size;
    int last_accessed;
    char type_of_file;
};

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
        char **holder = realloc(list->data, POINTER_SIZE * list->limit_size);
        if (holder == NULL) {
            fprintf(stderr, "%s", err_msg);
            return false;
        }
        list->data = holder;
    }

    if (strlen(prefix) + strlen(element) + 1 > 254) {
        return true; }
          // + 1 is for '/' character between prefix and element
    
    list->data[list->size] = malloc(sizeof(char)*255);
    list->data[list->size][0] = '\0';
    if (list->data[list->size] == NULL) {
        fprintf(stderr, "%s", err_msg);
        return false;
    }

    strcat(list->data[list->size], prefix);
    if (strcmp(element, "") != 0) {
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

            if (todo_list->data[i][strlen(todo_list->data[i]) - 1] != '/') {
                if (strlen(todo_list->data[i]) > 253) {
                    fprintf(stderr, "Path '%s/' is too large to store!\n", todo_list->data[i]);
                    continue;
                } else {
                    strcat(todo_list->data[i], "/");
                }
            }

            int new_start_index = todo_list->size;
            DIR *directory = opendir(todo_list->data[i]);
            if (directory == NULL) continue;
            struct dirent *dir_content;
            while ((dir_content = readdir(directory)) != NULL) {
                if (strcmp(dir_content->d_name, "..") == 0 || strcmp(dir_content->d_name, ".") == 0) continue;
                if (!append(todo_list, todo_list->data[i], dir_content->d_name)) { free(directory); return false; }
            }
            closedir(directory);
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

void split_to_prefix(char *input, char *prefix, char **file_name)
{
    int pointer = strlen(input) - 100;
    while (input[pointer] != '/' && input[pointer] == '\0') {
        pointer++;
    }
    if (input[pointer] == '\0') {
        strncpy(prefix, input, 155);
        prefix[pointer+1] = '\0';
        *file_name = "";
        return;
    }
    strncpy(prefix, input, 155);
    prefix[pointer+1] = '\0';
    *file_name += pointer + 2;
}

char* to_octal(int num, int size_of_output, bool control_count, char **to_free)
{
    int pointer;
    char *output = malloc(size_of_output);
    *to_free = output;
    if (control_count) {
        output[size_of_output - 1] = ' ';
        output[size_of_output - 2] = '\0';
        pointer = size_of_output - 3;
    } else {
        output[size_of_output - 1] = '\0';
        pointer = size_of_output - 2;
    }
    
    while (num > 0) {
        output[pointer] = num % 8 + 48;
        num /= 8;
        pointer--;
    }
    while (pointer >= 0) {
        output[pointer] = '0';
        pointer--;
    }
    return output;

}

bool load_info(dynamic_list *list, bool should_print, int output)
{   
    struct stat path_stat;
    bool return_value = true;
    int file;
    
    for (int i = 0; i < list->size; i++) {
        if (should_print) {
            if (is_regular_directory(list->data[i])) {
                char no_slash[255];
                strcpy(no_slash, list->data[i]);
                no_slash[strlen(no_slash) - 1] = '\0';
                fprintf(stderr, "%s\n", no_slash);
            } else fprintf(stderr, "%s\n", list->data[i]);
        }
        
        if (is_regular_directory(list->data[i])) {
            DIR *folder_check;
            if ((folder_check = opendir(list->data[i])) == NULL) {
                fprintf(stderr, "Permission denied to folder '%s'!\n", list->data[i]);
                return_value = false;
            } else {
                closedir(folder_check);
            }
        } else {
            if ((file = open(list->data[i], O_RDONLY)) == -1) {
                fprintf(stderr, "Permission denied to file '%s'!\n", list->data[i]);
                return_value = false;
                continue;
            }
        }

        if (stat(list->data[i], &path_stat) == -1) {
            fprintf(stderr, "Could not access info about path '%s'\n", list->data[i]);
            return_value = false;
            continue;
        }

        char *new_header = calloc(sizeof(char), 512);
        char *to_free[6];
        
        char *prefix = malloc(155);
        prefix[0] = '\0';
        char *file_name = list->data[i];
        if (strlen(list->data[i]) > 99) {
            split_to_prefix(list->data[i], prefix, &file_name);
        }

        strcpy(&new_header[0], file_name);
        strcpy(&new_header[100], to_octal(path_stat.st_mode & 511, 8, false, &to_free[0]));
        strcpy(&new_header[108], to_octal(path_stat.st_uid, 8, false, &to_free[1]));
        strcpy(&new_header[116], to_octal(path_stat.st_gid, 8, false, &to_free[2]));
        strcpy(&new_header[124], to_octal(path_stat.st_size, 12, false, &to_free[3]));
        if (is_regular_directory(list->data[i])) {
            strcpy(&new_header[124], "00000000000");
        }
        strcpy(&new_header[136], to_octal(path_stat.st_mtim.tv_sec, 12, false, &to_free[4]));
        for (int i = 148; i < 156; i++) {
            new_header[i] = ' ';
        }
        if (is_regular_directory(list->data[i])) new_header[156] = '5';
        else new_header[156] = '0';
        strcpy(&new_header[257], "ustar");
        new_header[263] = '0'; new_header[264] = '0';
        strcpy(&new_header[265], getpwuid(path_stat.st_uid)->pw_name);
        strcpy(&new_header[297], getgrgid(path_stat.st_gid)->gr_name);
        strcpy(&new_header[329], "0000000");
        strcpy(&new_header[337], "0000000");
        strcpy(&new_header[345], prefix);
        free(prefix);

        unsigned int control_count = 0;
        for (int i = 0; i < 512; i++) {
            control_count += (unsigned int) new_header[i];
        }
        strcpy(&new_header[148], to_octal(control_count, 8, true, &to_free[5]));

        new_header[155] = ' ';

        write(output, new_header, 512);

        for (int i = 0; i < 6; i++) {
            free(to_free[i]);
        }
        free(new_header);
        
        if (is_regular_directory(list->data[i])) continue;

        char *buffer;
        int size = path_stat.st_size;
        while (size > 0) {
            buffer = calloc(1, 512);
            read(file, buffer, 512);
            write(output, buffer, 512);
            free(buffer);
            size -= 512;
        }
        close(file);
    }
    return return_value;
}

char* from_prefix(char buffer[512])
{
    char *path_to_file = malloc(255);
    path_to_file[0] = '\0';
    if (buffer[345] != '\0') {
        strcat(path_to_file, &buffer[345]);
        strcat(path_to_file, "/");
    }
    
    strcat(path_to_file, &buffer[0]);
    return path_to_file;

}

unsigned int control_sum(char *buffer)
{
    unsigned int sum = 8*32; // size of 8 spaces
    for (int i = 0; i < 512; i++) {
        if (i >= 148 && i < 156) continue;
        sum += (unsigned int) buffer[i];
    }
    return sum;
}

void check_existence_of_dirs(char *path)
{
    char buffer[255];
    for (int i = 0; i < (int) strlen(path) - 1; i++) {
        if (path[i] == '/') {
            buffer[i] = '\0';
            DIR *check = opendir(buffer);
            if (check != NULL) {
                closedir(check);
                continue;
            }
            mkdir(buffer, 0777);
        }
        buffer[i] = path[i];
    }
}

struct metadata* extract_metadata(char *buffer)
{
    struct metadata *data = malloc(sizeof(struct metadata));
    data->path_to_file = from_prefix(buffer);
    data->mode = (int) strtoll(&buffer[100], NULL, 8);
    data->file_size = (int) strtoll(&buffer[124], NULL, 8);
    data->last_accessed = (int) strtoll(&buffer[136], NULL, 8);
    data->type_of_file = buffer[156];
    return data;
}


bool load_files(struct stat input_stats, int input, bool should_print)
{
    int byte_counter = 0;
    char *header_buffer = calloc(sizeof(char), 512);
    bool return_val = true;
    while (byte_counter < input_stats.st_size - 1024) {
        read(input, header_buffer, 512);
        byte_counter += 512;

        struct metadata *data = extract_metadata(header_buffer);
        if (should_print) fprintf(stderr, "%s\n", data->path_to_file);

        if (control_sum(header_buffer) != (unsigned int) strtoll(&header_buffer[148], NULL, 8)) {
            fprintf(stderr, "Control sum from one of the headers doesn't match!\n");

            free(header_buffer);
            free(data->path_to_file);
            free(data);
            return false;
        }

        char *locater;
        if ((locater = strchr(data->path_to_file, '/')) != NULL && locater != &data->path_to_file[strlen(data->path_to_file) - 1]) {
            check_existence_of_dirs(data->path_to_file);
        }

        if (data->type_of_file == '5') {
            DIR *check = opendir(data->path_to_file);
            if (check != NULL) {
                free(data->path_to_file);
                free(data);
                closedir(check);
                continue;
            } else {
                mkdir(data->path_to_file, data->mode);
            }
        } else if (data->type_of_file == '0') {
            int check = open(data->path_to_file, O_RDONLY); 
            if (check != -1) {
                fprintf(stderr, "File '%s' already exists!\n", data->path_to_file);
                return_val = false;
                int to_seek = data->file_size + (512 - data->file_size % 512);
                if (data->file_size == 0) {
                    to_seek = 0;
                }
                lseek(input, to_seek, SEEK_CUR);
                byte_counter += to_seek;
                free(data->path_to_file);
                free(data);
                close(check);
                continue;
            }
            int created = creat(data->path_to_file, data->mode);
            close(created);
        } else {
            fprintf(stderr, "Unrecognized type of file!\n");
            free(header_buffer);
            free(data->path_to_file);
            free(data);
            return false;
        }

        if (data->type_of_file == '0' && data->file_size > 0) {
            int output_file = open(data->path_to_file, O_WRONLY);
            char *file_content = malloc(data->file_size);
            int spacer = data->file_size % 512;
            if (spacer == 0) {
                spacer = 512;
            }
            read(input, file_content, data->file_size);
            write(output_file, file_content, data->file_size);
            free(file_content);
            close(output_file);
            lseek(input, 512 - spacer, SEEK_CUR);
            byte_counter += data->file_size + (512 - spacer);
        }

        //fprintf(stderr, "Setting time for %s to %d\n", data->path_to_file, data->last_accessed);

        struct utimbuf ubuf = {(time_t) data->last_accessed, (time_t) data->last_accessed };
        utime(data->path_to_file, &ubuf);
        
        free(data->path_to_file);
        free(data);

    }
    free(header_buffer);
    return return_val;
}


int create_failed(dynamic_list *list, int file)
{
    if (file != -1) close(file);
    clear(list);
    return false;
}

int cmpstr(void const *pStr1, void const *pStr2) { 
    return strcmp(*(char const **) pStr1, *(char const **) pStr2);
}

bool create_processer(int argc, char **input_files, bool should_print)
{
    dynamic_list *todo_list = malloc(sizeof(dynamic_list));
    todo_list->size = 0;
    todo_list->limit_size = 20;
    todo_list->data = malloc(POINTER_SIZE * 20);

    for (int i = 3; i < argc; i++) {
        if (!append(todo_list, input_files[i], "")) return create_failed(todo_list, -1);
    }
    if (!list_processer(todo_list, 0)) return create_failed(todo_list, -1);

    qsort(todo_list->data, todo_list->size, POINTER_SIZE, cmpstr);

    /*
    for (int i = 0; i < todo_list->size; i++) {
        puts(todo_list->data[i]);
    }
    */

    int check_existence;
    if ((check_existence = open(input_files[2], O_RDONLY, 0666)) != -1) {
        fprintf(stderr, "Output file already exists!\n");
        return create_failed(todo_list, check_existence);
    }

    int output = creat(input_files[2], 0666);
    if (output == -1) {
        fprintf(stderr, "Cannot write to given output!\n");
        return create_failed(todo_list, output);
    }

    bool return_val = true;
    struct stat path_stat;
    if (!load_info(todo_list, should_print, output)) {
        if ((check_existence = stat(input_files[2], &path_stat)) != -1 && path_stat.st_size == 0) {
            fprintf(stderr, "Trying to create empty .tar!\n");
            close(output);
            remove(input_files[2]);
            return create_failed(todo_list, -1);
        }
        return_val = false;
    }

    
    if (stat(input_files[2], &path_stat) && path_stat.st_size == 0) {
        fprintf(stderr, "Trying to create empty .tar!\n");
        remove(input_files[2]);
        return create_failed(todo_list, output);
    }

    char *ender = calloc(sizeof(char), 1024);
    if (ender == NULL) {
        fprintf(stderr, "Could not allocate enough memory for ending zero sequence.\n");
        return create_failed(todo_list, output);
    }
    write(output, ender, 1024);
    free(ender);
    close(output);

    clear(todo_list);
    return return_val;
}

bool extract_processer(char *path_to_input, bool should_print)
{
    int file = open(path_to_input, O_RDONLY);
    if (file == -1) {
        fprintf(stderr, "Could not open input tar file for extraction!\n");
        return false;
    }
    struct stat path_stat;
    if (stat(path_to_input, &path_stat) == -1) {
        fprintf(stderr, "Could not access metadata of input tar file!\n");
        close(file);
        return false;
    } else if (path_stat.st_size % 512 != 0) {
        fprintf(stderr, "Given tar doesn't have valid size!\n");
        close(file);
        return false;
    }

    if (!load_files(path_stat, file, should_print)) { close(file); return false; }
    close(file);
    return true;

}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Not enough arguments given!\n");
        return EXIT_FAILURE;
    }

    bool create = false, extract = false, print = false;
    for (int i = 0; i < (int) strlen(argv[1]); i++) {
        if (argv[1][i] == 'c') {
            if (create) {
                fprintf(stderr, "Duplicit 'c' switch!\n");
                return EXIT_FAILURE;
            }
            create = true;
        } else if (argv[1][i] == 'x') {
            if (extract) {
                fprintf(stderr, "Duplicit 'x' switch!\n");
                return EXIT_FAILURE;
            }
            extract = true;
        } else if (argv[1][i] == 'v') {
            if (print) {
                fprintf(stderr, "Duplicit 'v' switch!\n");
                return EXIT_FAILURE;
            }
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

    if (create) {
        if (!create_processer(argc, argv, print)) return EXIT_FAILURE;
    } else if (extract) {
        if (argc != 3) {
            fprintf(stderr, "Invalid number of arguments given for extraction! (Given '%d', but should be 3)\n", argc);
            return EXIT_FAILURE;
        }
        if (!extract_processer(argv[2], print)) return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
