#include <assert.h>
#include <stdio.h>

#include "list.h"
#include "list_utils.h"

//-----------------------------------------------------------------------------
//  Convenience macros
//-----------------------------------------------------------------------------

// --<----<----<----<----<----<----<----<----<----<----<----<----<----<----<--
// REMOVE THESE MACROS WHEN DONE
// --<----<----<----<----<----<----<----<----<----<----<----<----<----<----<--

// See ⟨list.c⟩ for explanation on what these macros do.

#define UNUSED(VARIABLE)                                                    \
    ((void) (VARIABLE))

#define NOT_IMPLEMENTED()                                                   \
    do {                                                                    \
        fprintf(stderr, "Unimplemented function '%s' called.\n", __func__); \
        abort();                                                            \
    } while (0)

//-----------------------------------------------------------------------------
//  Convenience macros
//-----------------------------------------------------------------------------

bool list_any(const struct list *list, predicate p)
{
    /* TODO: Do not forget to add ⟨assert()⟩ checks to other functions. */
    assert(list != NULL);
    assert(p != NULL);

    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ UNUSED(p);
    /* ! */ NOT_IMPLEMENTED();
}

bool list_all(const struct list *list, predicate p)
{
    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ UNUSED(p);
    /* ! */ NOT_IMPLEMENTED();
}

bool list_insert(struct list *list, size_t index, void *data)
{
    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ UNUSED(index);
    /* ! */ UNUSED(data);
    /* ! */ NOT_IMPLEMENTED();
}

const void *list_get(const struct list *list, size_t index)
{
    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ UNUSED(index);
    /* ! */ NOT_IMPLEMENTED();
}

int list_get_index(const struct list *list, const void *data)
{
    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ UNUSED(data);
    /* ! */ NOT_IMPLEMENTED();
}

bool list_remove(struct list *list, size_t index, void *data)
{
    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ UNUSED(index);
    /* ! */ UNUSED(data);
    /* ! */ NOT_IMPLEMENTED();
}

const void* list_find_first(const struct list *list, predicate p)
{
    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ UNUSED(p);
    /* ! */ NOT_IMPLEMENTED();
}

const void* list_find_last(const struct list *list, predicate p)
{
    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ UNUSED(p);
    /* ! */ NOT_IMPLEMENTED();
}

void list_for_each(struct list *list, action f, void *context)
{
    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ UNUSED(f);
    /* ! */ UNUSED(context);
    /* ! */ NOT_IMPLEMENTED();
}

void list_for_each_reverse(struct list *list, action f, void *context)
{
    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ UNUSED(f);
    /* ! */ UNUSED(context);
    /* ! */ NOT_IMPLEMENTED();
}

void list_sort(struct list *list, comparator cmp)
{
    /* TODO: Remove the following lines and implement the function. */
    /* ! */ UNUSED(list);
    /* ! */ UNUSED(cmp);
    /* ! */ NOT_IMPLEMENTED();
}
