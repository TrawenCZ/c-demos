#ifndef TEST_UTILS_H
#define TEST_UTILS_H

/**
 *  @file   test_utils.h
 *  @author Roman Lacko <xlacko1@fi.muni.cz>
 */

#include "cut.h"

#include <stddef.h>

#include "list.h"

// Convenience macros to imitate ⟨CHECK_*⟩.
#define LIST_CHECK_INTEGRITY(LIST)                                          \
    list_check_integrity((LIST))

#define LIST_CHECK_CONTENTS(LIST, SIZE, ARRAY)                              \
    list_check_contents((LIST), (SIZE), (ARRAY))

#define LIST_INITIALIZE(LIST, HEAD, TAIL, SIZE)                             \
    do {                                                                    \
        (LIST).head = (HEAD);                                               \
        (LIST).tail = (TAIL);                                               \
        (LIST).elem_size = (SIZE);                                          \
    } while (0)

// Why stop at CHECK_* macros?
#define DESTROY_LIST_RAW(NODES)                                             \
    free_nodes(NODES)

/**
 * \brief Prints the error and calls \c abort.
 *
 * \param str   error message to print
 */
void terminate(const char *str);

/**
 * \brief Checks that the list is sound.
 *
 * \param list  list to check
 */
void list_check_integrity(struct list *list);

/**
 * \brief Checks that the list contains the specified values.
 *
 * \note Call \c list_check_integrity before this function.
 *
 * \param list  list to check
 * \param size  number of expected integers
 * \param data  array of expected integers
 */
void list_check_contents(struct list *list, size_t size, int *data);

/**
 * \brief Deletes the chain of nodes.
 *
 * \param head  head of the list
 */
void free_nodes(struct node *head);

#endif // TEST_UTILS_H
