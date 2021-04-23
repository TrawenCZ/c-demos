#ifndef LIST_H
#define LIST_H

/**
 * \file   list.h
 * \author ...
 */

#include <stdlib.h>
#include <stdbool.h>

//-----------------------------------------------------------------------------
//  Features
//-----------------------------------------------------------------------------

// Uncomment the following line to enable doubly-linked implementation.
//#define LLIST_ENABLE_TWO_LINKS

// Uncomment the following line if you want to use nodes with
// Flexible Array Member (FAM)
//#define LLIST_ENABLE_FAM

// Uncomment the following line if you want to enable tests for
// all ends.
//#define LLIST_ENABLE_ALL_ENDS

//-----------------------------------------------------------------------------
//  Types
//-----------------------------------------------------------------------------

/**
 * \brief Doubly linked list node.
 */
struct node {
    struct node *next; /**< next node */
#if defined(LLIST_ENABLE_TWO_LINKS)
    struct node *prev; /**< previous node */
#endif

#if defined(LLIST_ENABLE_FAM)
    char data[]; /**< flexible array member */
#else
    void *data; /**< pointer to node data */
#endif
};

/**
 * \brief Doubly linked list.
 */
struct list {
    struct node *head; /**< list's first node */
    struct node *tail; /**< list's last node  */

    size_t elem_size; /**< size of stored elements */
};

//-----------------------------------------------------------------------------
//  Consistency requirements
//-----------------------------------------------------------------------------

/*
 * Let ⟦P⟧ be a property that a list has if and only if it satisfies
 * the following requirements:
 *
 * A) ⟨list->head⟩ is ⟨NULL⟩ if and only if the ⟨list->tail⟩ is ⟨NULL⟩,
 * B) if ⟨list->head⟩ is not ⟨NULL⟩, then ⟨head->prev⟩ is ⟨NULL⟩,
 * C) if ⟨list->tail⟩ is not ⟨NULL⟩, then ⟨tail->next⟩ is ⟨NULL⟩,
 *
 * Each of the following functions must satisfy this condition:
 *
 *     If the function's argument is a list with property ⟦P⟧, then
 *     the list also has the property ⟦P⟧ after the function ends.
 */

//-----------------------------------------------------------------------------
//  Note
//-----------------------------------------------------------------------------

/*
 * Unless stated otherwise, ⟨NULL⟩ value passed as a parameter to any of
 * the following functions causes undefined behaviour. This is not tested
 * by the included tests.
 *
 * Use ⟨assert()⟩ to check for these conditions.
 */

//-----------------------------------------------------------------------------
//  Task 1: List initialization
//-----------------------------------------------------------------------------

/**
 * \brief Initializes the list structure.
 *
 * The result of this operation is an empty list.
 *
 * \note The behaviour of this function is undefined if \p element_size is zero.
 *
 * \param element_size  Size (in bytes) of stored elements.
 */
void list_init(struct list *list, size_t element_size);

//-----------------------------------------------------------------------------
//  Task 2: Adding elements to the end of the list
//-----------------------------------------------------------------------------

/**
 * \brief Inserts a new element after the last element in the list.
 *
 * The element will contain a copy of the memory pointed to by the
 * \p data parameter.
 *
 * \param data      Pointer to data to be stored.
 * \returns \c true if the operation was successful, \c false otherwise.
 */
bool list_push_back(struct list *list, const void *data);

//-----------------------------------------------------------------------------
//  Task 3: Basic queries
//-----------------------------------------------------------------------------

/**
 * \brief Returns the number of elements stored in the list.
 */
size_t list_size(const struct list *list);

/**
 * \brief Predicate that tells wheter the \a list is empty or not.
 *
 * \returns \c true if the \a list is empty, \c false otherwise.
 */
bool list_is_empty(const struct list *list);

//-----------------------------------------------------------------------------
//  Task 4: Element retrieval
//-----------------------------------------------------------------------------

/**
 * \brief Removes and returns the first element stored in the list.
 *
 * If \p data is not \c NULL, contents of the node data will be copied
 * to the memory pointed to by \p data pointer.
 *
 * \note    If \p list is empty, the operation fails.
 * \note    If the list is empty, the function must not change the memory
 *          pointed to by \p data.
 *
 * \returns \c true if the operation was successful, \c false otherwise.
 */
bool list_pop_front(struct list *list, void *data);

//-----------------------------------------------------------------------------
//  Task 5: Destruction and mayhem
//-----------------------------------------------------------------------------

/**
 * \brief Releases all resources associated with the \p list.
 */
void list_destroy(struct list *list);

//-----------------------------------------------------------------------------
//  Bonus 1: Operations at both ends
//-----------------------------------------------------------------------------

/**  NOTE  **
 *
 * You ‹MUST› uncomment the LLIST_ENABLE_ALL_ENDS macro at the beginning
 * of this file to enable tests and implementation of these functions.
 */

#if defined(LLIST_ENABLE_ALL_ENDS)

/**
 * \brief Inserts a new element before the first element in the list.
 *
 * The element will contain a copy of the memory pointed to by the
 * \p data parameter.
 *
 * \param data      Pointer to data to be stored.
 * \return \c true if the operation was successful, \c false otherwise.
 */
bool list_push_front(struct list *list, const void *data);

/**
 * \brief Removes and returns the last element stored in the list.
 *
 * If \p data is not \c NULL, contents of the node data will be copied
 * to the memory pointed to by \p data pointer.
 *
 * \note    If \p data is set to \c NULL, the element is simply discarded.
 * \note    If the list is empty, the function must not change the memory
 *          pointed to by \p data.
 *
 * \returns \c true if the operation was successful, \c false otherwise.
 */
bool list_pop_back(struct list *list, void *data);

#endif // LLIST_ENABLE_ALL_ENDS

#endif // LIST_H
