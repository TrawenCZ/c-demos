#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>

#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0

void print_stats(int file)
{
	struct stat *file_info = malloc(sizeof(struct stat));
	if (file_info == NULL) {
		fprintf(stderr, "Could not allocate enough memory for file info!\n");
		return;
	}

	fstat(file, file_info);

	printf("Velikost souboru: %ld bajtu\n", file_info->st_size);
	printf("Datum posledniho pristupu: %s", ctime(&(file_info->st_atime)));
	printf("UID majitele: %d\n", file_info->st_uid);
	printf("GID majitele: %d\n", file_info->st_gid);

	free(file_info);
}

void print_file(int file) 
{
	char buffer[1024] = {'\0'};
	ssize_t size = 0;

	while ((size = read(file, buffer, 1024)) > 0) {
		write(0, buffer, size);
	}
}


int main(int argc, char *argv[])
{
	bool prnt_file = false;
	bool prnt_stats = false;
	bool file_printed = false;

	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "-s") == 0) {
			if (file_printed) {
				perror(argv[0]);
				return EXIT_FAILURE;
			} else prnt_stats = true;
		} else if (strcmp(argv[i], "-p") == 0) {
			if (file_printed) {
				perror(argv[0]);
				return EXIT_FAILURE;
			} else prnt_file = true;
		} else {
			int file = open(argv[i], O_RDONLY);
			file_printed = true;
			if (file < 0) {
				fprintf(stderr, "File '%s' could not be opened!\n", argv[i]);
				continue;
			}

			if (prnt_file) {
				printf("Obsah souboru '%s':\n", argv[i]);
				print_file(file);
				putchar('\n');
			}
			if (prnt_stats) {
				printf("Informace o souboru '%s':\n", argv[i]);
				print_stats(file);
				putchar('\n');
			}
			close(file);
		}
	}
	return EXIT_SUCCESS;
}
