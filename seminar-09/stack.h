//#include <stdio.h>

#ifndef STACK_H
#define STACK_H

struct stack_item;

struct stack
{
    struct stack_item *top;
};

void stack_init(struct stack *);
void stack_push(struct stack *, void *);
void *stack_top(struct stack *);
void stack_pop(struct stack *);
int stack_empty(struct stack *);

#endif
