#include "stack.h"

#include <stdlib.h>

struct stack_item
{
    void *data;
    struct stack_item *next;
};

void stack_init(struct stack *stack)
{
    if (!stack)
        return;
    stack->top = NULL;
}

void stack_push(struct stack *stack, void *data)
{
    if (!stack)
        return;

    struct stack_item *item = (struct stack_item *) malloc(sizeof(struct stack_item));
    item->data = data;
    item->next = stack->top;
    stack->top = item;
}

void *stack_top(struct stack *stack)
{
    if (!stack || !stack->top)
        return NULL;

    return stack->top->data;
}

void stack_pop(struct stack *stack)
{
    if (!stack || !stack->top)
        return;

    struct stack_item *to_remove = stack->top;
    stack->top = to_remove->next;
    free(to_remove);
}

int stack_empty(struct stack *stack)
{
    return !stack || !stack->top;
}
