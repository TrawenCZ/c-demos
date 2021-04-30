#include <stdio.h>

#ifndef TREE_H
#define TREE_H

struct node
{
    int value;
    struct node *left;
    struct node *right;
};

struct tree
{
    struct node *root;
};

struct data_pack
{
    int value;
    int interval[2];
    int *values;
};

/** Create semi-balanced tree.
 * @param size total count of nodes in tree
 * @return pointer to generated tree
 */
struct tree *tree_create(int size);

/** Deallocate the tree. */
void tree_destroy(struct tree *tree);

/** Print tree with root on top and layers of children on rows.
 *
 * Example:
 *     4
 *   /   \
 * 2      6
 *  \    /
 *   3  5
 *
 * @param tree to be printed
 * @param out is the handle to the output file(stream)
 */
void tree_print(const struct tree *tree, FILE *out);

#endif
