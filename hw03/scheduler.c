#include "scheduler.h"
#include <assert.h>
#include <string.h>

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

#ifdef CONFIG_ENABLE_DEBUG
#include <stdio.h>
#define DEBUG(STATEMENT)                                                       \
  do {                                                                         \
    STATEMENT;                                                                 \
  } while (0)
#else
#define DEBUG(STATEMENT)
#endif /* CONFIG_ENABLE_DEBUG */

priority_queue create_queue(void) {
  priority_queue queue = {NULL, NULL, 0};
  return queue;
}

bool copy_queue(priority_queue *dest, const priority_queue *source) {
  assert(source != NULL);
  assert(dest != NULL);
  dest->top = NULL;
  priority_queue_item *pSourceNode = source->top;
  priority_queue_item *pDestNode = NULL;

  while (pSourceNode != NULL) {
    priority_queue_item *pNewDestNode = malloc(sizeof(priority_queue_item));
    if (pNewDestNode == NULL) {
      clear_queue(dest);
      return false;
    }
    if (dest->top == NULL) {
      dest->top = pNewDestNode;
    } else {
      pDestNode->next = pNewDestNode;
    } // pDestNode is not NULL
    memcpy(&(pNewDestNode->process), &(pSourceNode->process),
           sizeof(process_type));
    pNewDestNode->prev = pDestNode;
    pDestNode = pNewDestNode;
    pSourceNode = pSourceNode->next;
  }
  if (pDestNode != NULL)
    pDestNode->next = NULL;
  dest->bottom = pDestNode;
  dest->size = source->size;
  return true;
}

void clear_queue(priority_queue *queue) {
  assert(queue != NULL);
  priority_queue_item *pNode = queue->top;
  while (pNode != NULL) {
    priority_queue_item *pNext = pNode->next;
    free(pNode);
    pNode = pNext;
  }
  queue->top = NULL;
  queue->bottom = NULL;
  queue->size = 0;
}

int bit_count(uint16_t cpu_mask) {
  int count = 0, bit_pointer = 1;
  while (cpu_mask >= bit_pointer) {
    if ((bit_pointer & cpu_mask) != 0)
      count += 1;
    bit_pointer <<= 1;
  }
  return count;
}

enum push_result pusher(priority_queue *queue,
                        priority_queue_item *pPushPosition,
                        process_type process, bool atEnd) {
  priority_queue_item *pushNode = malloc(sizeof(priority_queue_item));
  if (pushNode == NULL)
    return push_error;

  if (pPushPosition == NULL) {
    pushNode->next = NULL;
    if (atEnd) {
      queue->bottom->next = pushNode;
      pushNode->prev = queue->bottom;
      queue->bottom = pushNode;
    } else {
      pushNode->prev = NULL;
      queue->top = pushNode;
    }
  } else if (queue->top == pPushPosition) {
    pushNode->next = pPushPosition;
    queue->top = pushNode;
    pPushPosition->prev = pushNode;
  } else {
    pushNode->next = pPushPosition;
    pushNode->prev = pPushPosition->prev;
    pPushPosition->prev->next = pushNode;
    pPushPosition->prev = pushNode;
  }

  if (queue->bottom == NULL)
    queue->bottom = pushNode;
  memcpy(&(pushNode->process), &process, sizeof(process_type));
  return push_success;
}

enum push_result push_to_queue(priority_queue *queue, process_type process) {
  assert(queue != NULL);
  assert(process.niceness >= 10 && process.niceness < 50);

  priority_queue_item *pNode = queue->top;
  priority_queue_item *pPushPosition = NULL;
  bool positionFound = false, empty = true;

  while (pNode != NULL) {
    if (!positionFound &&
        pNode->process.niceness * pNode->process.remaining_time >=
            process.niceness * process.remaining_time) {
      pPushPosition = pNode;
      positionFound = true;
    }
    empty = false;
    if (!(pNode->process.callback == process.callback &&
          pNode->process.context == process.context)) {
      pNode = pNode->next;
      continue;
    }

    if (pNode->process.niceness == process.niceness &&
        pNode->process.remaining_time == process.remaining_time &&
        pNode->process.cpu_mask == process.cpu_mask)
      return push_duplicate;
    return push_inconsistent;
  }

  unsigned int inputPriority = process.niceness * process.remaining_time;
  if (pPushPosition == NULL) {
    if (empty)
      return pusher(queue, pPushPosition, process, false);
    return pusher(queue, pPushPosition, process, true);
  }

  if (pPushPosition->process.niceness * pPushPosition->process.remaining_time >
      inputPriority) {
    return pusher(queue, pPushPosition, process, false);
  } else {
    unsigned int sourcePriority =
        pPushPosition->process.niceness * pPushPosition->process.remaining_time;
    while (inputPriority == sourcePriority &&
           bit_count(pPushPosition->process.cpu_mask) <
               bit_count(process.cpu_mask)) {
      pPushPosition = pPushPosition->next;
      if (pPushPosition == NULL)
        break;
      sourcePriority = pPushPosition->process.niceness *
                       pPushPosition->process.remaining_time;
    }
    return pusher(queue, pPushPosition, process, true);
  }
}

void pop_item(priority_queue *queue, priority_queue_item *item) {
  assert(queue != NULL);
  if (queue->top == item || queue->bottom == item) {
    if (queue->top == item && queue->bottom == item) {
      queue->top = NULL;
      queue->bottom = NULL;
    } else if (queue->top == item) {
      queue->top = item->next;
      item->next->prev = NULL;
    } else {
      queue->bottom = item->prev;
      item->prev->next = NULL;
    }
    queue->size -= 1;
    return;
  }
  item->next->prev = item->prev;
  item->prev->next = item->next;
  queue->size -= 1;
  return;
}

priority_queue_item *top_getter(const priority_queue *queue,
                                uint16_t cpu_mask) {
  assert(queue != NULL);
  priority_queue_item *pNode = queue->top;
  while (pNode != NULL && (cpu_mask & pNode->process.cpu_mask) == 0) {
    pNode = pNode->next;
  }
  return pNode;
}

process_type *get_top(const priority_queue *queue, uint16_t cpu_mask) {
  assert(queue != NULL);
  priority_queue_item *pNode = top_getter(queue, cpu_mask);
  if (pNode == NULL)
    return false;
  return &(pNode->process);
}

bool pop_top(priority_queue *queue, uint16_t cpu_mask, process_type *out) {
  assert(queue != NULL);
  priority_queue_item *top = top_getter(queue, cpu_mask);
  if (top == NULL || queue == NULL || queue->top == NULL)
    return false;
  if (out != NULL)
    memcpy(out, &(top->process), sizeof(process_type));
  pop_item(queue, top);
  free(top);
  return true;
}

unsigned int run_top(priority_queue *queue, uint16_t cpu_mask,
                     unsigned int run_time) {
  assert(queue != NULL);
  priority_queue_item *pNode = top_getter(queue, cpu_mask);
  if (pNode == NULL)
    return 0;
  unsigned int output =
      pNode->process.callback(run_time, pNode->process.context);
  if (output == 0) {
    pop_item(queue, pNode);
    free(pNode);
    return 0;
  }
  pNode->process.remaining_time =
      MAX((int)(pNode->process.remaining_time - run_time), 0) + output;
  process_type process = pNode->process;
  pop_item(queue, pNode);
  free(pNode);
  push_to_queue(queue, process);
  return process.remaining_time;
}

bool renice(priority_queue *queue, cb_type callback, void *context,
            unsigned int niceness) {
  assert(queue != NULL);
  assert(niceness >= 10 && niceness < 50);
  priority_queue_item *pNode = queue->top;
  while (pNode != NULL && (pNode->process.context != context ||
                           pNode->process.callback != callback)) {
    pNode = pNode->next;
  }
  if (pNode == NULL)
    return false;
  pNode->process.niceness = niceness;
  pop_item(queue, pNode);
  push_to_queue(queue, pNode->process);
  free(pNode);
  return true;
}