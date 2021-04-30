#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <limits.h>

#include "stack.h"
#include "tree.h"

#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))
#define UNUSED(x) (void) (x)

/**********
 * Task 1 *
 ******** */

int tree_sum(const struct node *node)
{
    if (node == NULL) return 0;
    return node->value + tree_sum(node->left) + tree_sum(node->right);
}

int tree_size(const struct node *node)
{
    if (node == NULL) return 0;
    return 1 + tree_size(node->left) + tree_size(node->right);
}

int tree_max_value(const struct node *node)
{
    if (node == NULL) return INT_MIN;
    return MAX(node->value, MAX(tree_max_value(node->left), tree_max_value(node->right)));
}

int tree_aggregate(const struct node *node, int (*projector)(int)) 
{
    if (node == NULL) return 0;
    return projector(node->value) + tree_aggregate(node->left, projector) + tree_aggregate(node->right, projector);
}

int three_divisible(int number) 
{
    if (number % 3 == 0) return 1;
    return 0;
}

int even_or_odd(int number) 
{
    if (number % 2 == 0) return 1;
    return -1;
}

int cipher_count(int number) 
{
    int cipher_counter = 0;
    while (number != 0) {
        cipher_counter++;
        number = div(number, 10).quot;
    }
    return cipher_counter;
}

void tree_for_each(const struct node *node, void (*operation)(int, void *), struct data_pack *data_pack)
{
    if (node == NULL) return 0;
    tree_for_each(node->left, operation, data_pack);
    tree_for_each(node->right, operation, data_pack);
    operation(node->value, data_pack);
}

void max_in_tree(int number, struct data_pack *data) 
{
    data->value = MAX(number, data->value);
}

void in_interval(int number, struct data_pack *data)
{
    if (data->interval[0] < number && data->interval[1] > number) {
        data->value++;
    }
}

void usage_task_1(const struct tree *tree)
{
    puts("Task 1");
    if (!tree || !tree->root) {
        fprintf(stderr, "\tthe tree is empty");
        return;
    }

    printf("\tsum: %d\n", tree_sum(tree->root));
    printf("\tsize: %d\n", tree_size(tree->root));
    printf("\tmax number: %d\n", tree_max_value(tree->root));
}

int main()
{
    srand(time(NULL)); // initialize pseudo-random generator
    struct tree *tree = tree_create(11);

    tree_print(tree, stdout);

    // HERE YOU CAN USE THE GENERATED TREE
    usage_task_1(tree);

    struct data_pack *new_data;

    new_data->values = malloc(sizeof(int)*tree_size(tree->root));
    if (new_data->values == NULL) {
        fprintf(stderr, "Could not allocate memory for storing values of tree.\n");
        tree_destroy(tree); 
        return 0;
    }
    new_data->interval[0] = 0; new_data->interval[1] = 15;

    tree_destroy(tree);
    return 0;
}
