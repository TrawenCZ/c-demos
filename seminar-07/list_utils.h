#ifndef LIST_UTILS_H
#define LIST_UTILS_H

/**
 * \file   list_utils.h
 * \author ...
 */

#include <stdbool.h>
#include "list.h"

//-----------------------------------------------------------------------------
//  Types
//-----------------------------------------------------------------------------

/**
 * \brief Predicate.
 */
typedef bool (*predicate)(const void *item);

/**
 * \brief Comparator.
 *
 * Let ⟦a₁ < a₂⟧ denote that the first argument should be before the second in
 * ordering implemented by this function; then the comparator returns number
 * ⟦r ∈ N₀⟧ such that
 *
 *   - ⟦n < 0⟧ if ⟦a₁ < a₂⟧
 *   - ⟦n > 0⟧ if ⟦a₂ > a₂⟧
 *   - ⟦n = 0⟧ otherwise
 */
typedef int (*comparator)(const void *pa, const void *pb);

/**
 * \brief Action commands, i.e. values returned from an action.
 */
enum action_command {
    ACTION_CONTINUE,    ///< Instruct the for-each loop to continue.
    ACTION_BREAK,       ///< Instruct the for-each loop to return immediately.
};

/**
 * \brief Action.
 */
typedef enum action_command (*action)(void *item, void *context);

//-----------------------------------------------------------------------------
//  List predicates
//-----------------------------------------------------------------------------

/**
 * \brief Returns \c true if at least one element satisfies the predicate \p p.
 *
 * \param p         The predicate.
 *
 * \note In empty list there is no element that satisfies \p p.
 */
bool list_any(const struct list *list, predicate p);

/**
 * \brief Returns \c false if at least one element does not satisfy
 * the predicate \p p.
 *
 * \param p         The predicate.
 *
 * \note In empty list there is no element that does not satisfy \p p.
 */
bool list_all(const struct list *list, predicate p);

//-----------------------------------------------------------------------------
//  Index-based functions
//-----------------------------------------------------------------------------

/**
 * \brief Inserts a new element at position specified by \p index from the head.
 *
 * The original element at the specified index will be moved after the new one.
 *
 * \param index     Distance from list's head.
 * \param data      Pointer to memory containing the element.
 *
 * \note If \p index is \c list_size(&list), then the new element
 * will be pushed back after the tail.
 *
 * \returns \c true if the operation was successful.
 */
bool list_insert(struct list *list, size_t index, void *data);

/**
 * \brief Returns an unmodifiable pointer to the element at the position
 * \p index or \c NULL if no such element exists.
 *
 * \param index     Distance from list's head.
 * \returns Element or \c NULL if not found.
 */
const void *list_get(const struct list *list, size_t index);

/**
 * \brief Finds the first index of the specified element in the list.
 *
 * \param data      The value to search in the list.
 * \returns Index in the list or -1 if not found.
 */
int list_get_index(const struct list *list, const void *data);

/**
 * \brief Removes the element at position \p index.
 *
 * If \p data is not null, value from the discarded element will be copied
 * to memory pointed to by \p data.
 *
 * \param index     Distance from list's head.
 * \returns \c true if an element was removed, \c false otherwise.
 */
bool list_remove(struct list *list, size_t index, void *data);

//-----------------------------------------------------------------------------
//  Search functions
//-----------------------------------------------------------------------------

/**
 * \brief Returns the the first element that satisfies predicate \p p
 * or \c NULL if no such element exists.
 *
 * \param p         The predicate.
 */
const void *list_find_first(const struct list *list, predicate p);

/**
 * \brief Returns the the last element that satisfies predicate \p p
 * or \c NULL if no such element exists.
 *
 * \param p         The predicate.
 */
const void *list_find_last(const struct list *list, predicate p);

//-----------------------------------------------------------------------------
//  Enumeration
//-----------------------------------------------------------------------------

/**
 * \brief Calls function \p f on each element from head to tail.
 *
 * \note    The loop stops when all nodes are processed or when the action
 *          returns a value other than ⟨ACTION_CONTINUE⟩.
 *
 * \param f         The function to call on data of each node.
 * \param context   User context passed to the action.
 */
void list_for_each(struct list *list, action f, void *context);

/**
 * \brief Calls function \p f on each element from tail to head.
 *
 * \note    The loop stops when all nodes are processed or when the action
 *          returns a value other than ⟨ACTION_CONTINUE⟩.
 *
 * \param f         The function to call on data of each node.
 * \param context   User context passed to the action.
 */
void list_for_each_reverse(struct list *list, action f, void *context);

//-----------------------------------------------------------------------------
//  Sorting
//-----------------------------------------------------------------------------

/**
 * \brief Sorts all elements in the list using \p cmp comparator.
 *
 * \param cmp   The comparator.
 */
void list_sort(struct list *list, comparator cmp);

#endif // LIST_UTILS_H
