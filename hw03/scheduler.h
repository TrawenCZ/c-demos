#ifndef PRIORITY_QUEUE_HW03_H
#define PRIORITY_QUEUE_HW03_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

typedef unsigned int (*cb_type)(unsigned int time, void *context);

enum push_result { push_success, push_duplicate, push_inconsistent, push_error };

typedef struct process_type
{
    cb_type callback;
    void *context;

    unsigned int remaining_time;
    unsigned int niceness;
    uint16_t cpu_mask;
} process_type;

typedef struct priority_queue_item
{
    struct priority_queue_item *next;
    struct priority_queue_item *prev;

    process_type process;
} priority_queue_item;

typedef struct priority_queue
{
    priority_queue_item* top;
    priority_queue_item* bottom;

    size_t size;
} priority_queue;

priority_queue create_queue(void);

bool copy_queue(priority_queue *dest, const priority_queue *source);

void clear_queue(priority_queue *queue);

enum push_result push_to_queue(priority_queue *queue, process_type process);

process_type* get_top(const priority_queue *queue, uint16_t cpu_mask);

bool pop_top(priority_queue *queue, uint16_t cpu_mask, process_type *out);

unsigned int run_top(priority_queue *queue, uint16_t cpu_mask, unsigned int run_time);

bool renice(
    priority_queue *queue,
    cb_type callback,
    void *context,
    unsigned int niceness
);

#endif
