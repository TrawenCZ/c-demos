#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "readline.h"
#include "list.h"
#include "list_utils.h"

#define UNUSED(VAR) \
    ((void) (VAR))

/**
 * Auxiliary structure to hold information about file lines.
 */
struct line_info
{
    int number; ///< line number
    char *text; ///< pointer to text inside line attribute
    char *line; ///< pointer to the beggining of line
                ///<    this pointer will be used to deallocate
                ///<    the line when calling free_line_info
};

/**
 * Compares two line_info structures according to their number.
 */
int line_info_cmp(const void *a, const void *b)
{
    UNUSED(a);
    UNUSED(b);

    // TODO:
    //   * typecast ‹a› and ‹b› to ‹const struct lineInfo›
    //   * implement the comparator that compares records
    //     by their ‹number› attribute

    return 0;
}

/**
 * Reads lines from \p in and stores them inside the list
 * as \c line_info structures.
 *
 * \param   list        pointer to the linked list
 * \param   in          input file
 * \return  0 on success, an arbitrary error code on failure
 */
int read_lines(struct list *list, FILE *in)
{
    UNUSED(list);
    UNUSED(in);

    // TODO: implement the function
    // Hints:
    //  - ‹readline()›
    //  - ‹strtok()›

    return 1;
}

/**
 * Writes lines from list to output file.
 * The lines in the list should already be sorted.
 *
 * \param   list        pointer to the linked list
 * \param   out         output file
 */
void write_lines(struct list *list, FILE *out)
{
    UNUSED(list);
    UNUSED(out);

    // TODO: implement the function
    // Hints:
    //  - you may want to use ‹list_for_each()›
}

int main(int argc, char *argv[])
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s INPUT OUTPUT\n", argv[0]);
        return EXIT_FAILURE;
    }

    // TODO: open files

    int status;
    struct list list;
    list_init(&list, sizeof(struct line_info));

    // TODO: replace ‹/* PLACEHOLDER */ NULL› with your filehandles
    if ((status = read_lines(&list, /* PLACEHOLDER */ NULL)) == 0) {
        list_sort(&list, &line_info_cmp);
        write_lines(&list, /* PLACEHOLDER */ NULL);
        status = 0;
    }

    // ‹list_destroy()› would leak memory on ‹struct list_info›, because
    // it contains pointer to another memory block.

    //  list_destroy(&list);

    // Instead we will implement our own loop:

    struct line_info item;
    while (list_pop_front(&list, &item)) {
        // TODO: deallocate line

        // Q: Do you have to do this?
        //    free(item);
    }

    // TODO: close files

    return status;
}
