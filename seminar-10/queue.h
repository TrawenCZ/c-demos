/**
 * Queue library
 */

#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>

//=============================================================================

/**
 * Fixed-size element queue.
 *
 * If a queue is succesfully created with queue_create, then the following
 * statements are true about the members of @c 'struct queue' after each
 * operation except @c queue_destroy:
 *
 * - @c element and @c capacity never change
 * - @c index and @c size are initially 0
 * - the @c memory is able to store exactly @c capacity elements of size
 *   @c element
 * - the elements in @c memory are stored sequentially, as if it were
 *   an array of @c capacity cells of size @c element
 * - elements "wrap" after the end of the memory pointed to by @c memory;
 *   that is, if an element is stored at index 'capacity - 1', next element
 *   would be placed at index 0 again
 * - elements are copied, no pointers are stored
 * - @c index is always  between 0 and capacity - 1
 * - @c size is the number of elements currently stored
 */
struct queue
{
    size_t element;     ///< size of elements
    size_t capacity;    ///< queue capacity
    size_t index;       ///< current queue index
    size_t size;        ///< number of elements
    void   *memory;     ///< queue memory
};

//=============================================================================

/* ------------------------------  NOTE  --------------------------------------
 *
 * Except queue_create, all functions expect a pointer to a queue created
 * by queue_create as its first parameter. Their behaviour is undefined
 * otherwise.
 */

//--  Construction and Destruction  -------------------------------------------

/**
 * \brief Constructs the new queue.
 *
 * The queue shall be able to hold exactly \c capacity elements of the
 * size \c element_size bytes. If the queue constructor cannot guarantee
 * this requirement, it shall fail and return \c false.
 *
 * \note Queue of \c capacity equal to zero is a valid queue.
 * \note Queue of \c element_size equal to zero is not a valid queue
 *       and the function shall fail.
 * \note If the queue creation fails, the \c queue parameter will
 *       remain unchanged.
 *
 * \param[out]  queue        queue to initialize
 * \param[in]   element_size size of stored elements
 * \param[in]   capacity     upper bound on queue size
 * \return      \c true on successful creation, \c false otherwise
 */
bool queue_create(struct queue **queue, size_t element_size, size_t capacity);

/**
 * \brief Deallocates the queue.
 *
 * This function will free all memory associated with the \c queue, including
 * stored data. Behaviour of furhter operations on the @c queue parameter,
 * except queue_create() is undefined.
 *
 * If stored elements contain pointers to other dynamically allocated
 * structures, the client code must free them by other means
 * (e.g. queue_dequeue in a loop with deep deallocation) before calling this
 * function as this would cause memory leaks.
 *
 * \param[in]   queue       queue to deallocate
 */
void queue_destroy(struct queue *queue);

//--  Queries  ----------------------------------------------------------------

/**
 * \brief Query the current queue size.
 *
 * \param[in]   queue       queue to examine
 * \return      number of elements inside the queue
 */
size_t queue_size(const struct queue *queue);

/**
 * \brief Query the maximum number of elements the queue is able to hold.
 *
 * \param[in]   queue       queue to examine
 * \return      maximum capacity of the queue
 */
size_t queue_capacity(const struct queue *queue);

/**
 * \brief Query the element size of the queue.
 *
 * \param[in]   queue       queue to examine
 * \return      size of a single element the queue is able to hold
 */
size_t queue_element_size(const struct queue *queue);

/**
 * \brief Test queue emptiness.
 *
 * \note A queue is empty if it contains zero elements.
 *
 * \param[in]   queue       queue to examine
 * \return      \c true if the queue is empty
 */
bool queue_is_empty(const struct queue *queue);

/**
 * \brief Test queue fullness.
 *
 * \note A queue is full if its size reached its capacity.
 *
 * \param[in]   queue       queue to examine
 * \return      \c true if the queue is full
 */
bool queue_is_full(const struct queue *queue);

//--  Data manipulation  ------------------------------------------------------

/**
 * \brief Enqueue an element.
 *
 * If the queue is not full, the function shall copy exactly \c element_size
 * bytes from the memory pointed to by \c data into its internal buffer.
 * If the queue is full, the function shall fail.
 *
 * Since the queue stores the copy of the data, the modification of
 * the memory pointed to by \c data will not alter the value stored in the
 * queue after this function ends successfully.
 *
 * \note The behaviour of the function is undefined if \c data is \c NULL.
 *
 * \param[in]   queue       queue
 * \param[in]   data        data to store inside the queue
 * \return      \c true on success, \c false otherwise
 */
bool queue_enqueue(struct queue *queue, const void *data);

/**
 * \brief Dequeue an element.
 *
 * Removes the oldest element from the queue and copies its value into
 * the memory pointed to by \c data. If this pointer is \c NULL, the function
 * will discard the data without copying it. If the queue is empty,
 * the function shall fail.
 *
 * The memory pointed to by the \c data pointer (when not NULL) should be
 * able to store at least \c element_size bytes of memory. Otherwise
 * the behaviour of this function is undefined.
 *
 * \note If the function fails, the target memory shall not be modified.
 *
 * \param[in]   queue       queue
 * \param[out]  data        pointer to memory where the element will be saved
 * \return      \c true on success, \c false otherwise
 */
bool queue_dequeue(struct queue *queue, void *data);

/**
 * \brief Returns a pointer to the oldest element.
 *
 * If the queue is empty, the function returns \c NULL.
 *
 * \param[in]   queue       queue
 * \return      pointer to the oldest element of \c NULL if there is none
 */
const void *queue_peek(const struct queue *queue);

#endif /* QUEUE_H */
