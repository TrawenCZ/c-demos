#define CUT
#define CUT_MAIN
#ifndef DEBUG
#  define CUT_FORK_MODE
#endif
#include "cut.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "queue.h"

//=============================================================================
//  Examples
//=============================================================================

/*
 * Trivial test that succeeds.
 */
TEST(trivial_success)
{
    int zero = 0;
    CHECK(zero == 0);
}

/*
 * Trivial test that fails.
 */
TEST(trivial_failure)
{
    int zero = 0;
    CHECK(zero == 1);
}

//-----------------------------------------------------------------------------

/*
 * Example of queue_create test.
 *
 * Scenario
 * --------
 * WHEN     queue_create() gets called
 * WHEN     requested capacity is zero
 * THEN     queue is created successfully
 */
TEST(queue_create__zero_capacity)
{
    struct queue *queue;
    // the queue_create should succeed
    CHECK(queue_create(&queue, sizeof(int), 0u));

    // queue should now point to allocated structure
    CHECK(queue != NULL);

    // attributes should be set properly
    CHECK(queue->element   == sizeof(int));
    CHECK(queue->capacity  == 0u);
    CHECK(queue->index     == 0u);
    CHECK(queue->size      == 0u);
    // queue.memory may or may not be NULL, the documentation does not
    // specify this

    // well done, free the structure
    free(queue);
    // we didn't use queueDestroy since we are supposed to call only
    // one function from the queue interface in each test
}

/**
 * Example of queue_enqueue test
 *
 * Scenario
 * --------
 * GIVEN    empty queue of ints with capacity 32
 * WHEN     a single element is enqueued
 * THEN     size changes to 1
 */
TEST(queue_enqueue__single)
{
    int memory[32u]; // this simulates queue's internal memory
    struct queue queue = {          // queue initialization
        .element    = sizeof(int),  // element size
        .capacity   = 32u,          // capacity is same as array size
        .memory     = memory,       // fake internal memory
        // all other attributes not mentioned here will
        // be initialized to 0
    };

    // now let's enqueue a single element
    int number = 123456;
    // the function shall not fail
    CHECK(queue_enqueue(&queue, &number));

    // check that the element was stored in the memory
    CHECK(memory[0] == number);
    // make sure that size has increased
    CHECK(queue.size == 1u);
    // index should remain 0
    CHECK(queue.index == 0u);
}

//=============================================================================
//  Your tests go here
//=============================================================================

// TODO
