#define CUT
#include "cut.h"

#include "test_utils.h"

#include <stdio.h>

void terminate(const char *str)
{
    fprintf(stderr, "%s\n", str);
    abort();
}

void list_check_integrity(struct list *list)
{
    CHECK(list != NULL);

    // The head is NULL if and only if the tail is ⟨NULL⟩.
    CHECK(list->head == NULL || list->tail != NULL);
    CHECK(list->tail == NULL || list->head != NULL);

    // Empty list is trivially correct.
    if (list->head == NULL)
        return;

    // Check ⟨head->prev⟩ and ⟨tail->next⟩.
#if defined(LLIST_ENABLE_TWO_LINKS)
    CHECK(list->head->prev == NULL);
#endif
    CHECK(list->tail->next == NULL);

    // The list may only contain one node.
    if (list->head == list->tail)
        return;

    // The list now contains at least two nodes, gotta test'em all!
    for (struct node *node = list->head; node != NULL; node = node->next) {
        // ⟨NULL⟩ values are not allowed.
        CHECK(node->data != NULL);

        // Check identities.
#if defined(LLIST_ENABLE_TWO_LINKS)
        if (node != list->head) {
            CHECK(node->prev != NULL);
            CHECK(node->prev->next == node);
        }
#endif

        if (node != list->tail) {
            CHECK(node->next != NULL);
#if defined(LLIST_ENABLE_TWO_LINKS)
            CHECK(node->next->prev == node);
#endif
        }
    }

    // OK, we are done.
}

void list_check_contents(struct list *list, size_t size, int *data)
{
    struct node *node = list->head;

    if (size == 0U) {
        // The list should be empty.
        CHECK(node == NULL);
        return;
    }

    for (size_t i = 0U; i < size; ++i) {
        CHECK(node != NULL);
        CHECK(node->data != NULL);

        // Check i-th value.
        int value = *(int*)(node->data);
        CHECK(value == data[i]);

        node = node->next;
    }

    // If this node is not ⟨NULL⟩, there are more values in the list, not cool.
    CHECK(node == NULL);
}

void free_nodes(struct node *head)
{
    while (head != NULL) {
        struct node *next = head->next;

#if !defined(LLIST_ENABLE_FAM)
        free(head->data);
#endif

        free(head);
        head = next;
    }
}
