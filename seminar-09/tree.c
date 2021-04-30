#include "tree.h"

#include <stdlib.h>
#include <string.h>

static int maximum(int a, int b)
{
    return a > b ? a : b;
}

struct node *node_create(int size)
{
    if (size <= 0)
        return NULL;
    struct node *node = (struct node *) malloc(sizeof(struct node));
    node->value = rand() % 20;

    int right_part = (size - 1) / 2;
    int left_part = size - 1 - right_part;

    node->left = left_part ? node_create(left_part) : NULL;
    node->right = right_part ? node_create(right_part) : NULL;
    return node;
}

struct tree *tree_create(int size)
{
    struct tree *t = (struct tree *) malloc(sizeof(struct tree));
    t->root = node_create(size);
    return t;
}

void node_destroy(struct node *node)
{
    if (node->left)
        node_destroy(node->left);
    if (node->right)
        node_destroy(node->right);
    free(node);
}

void tree_destroy(struct tree *tree)
{
    if (!tree)
        return;

    if (tree->root)
        node_destroy(tree->root);

    free(tree);
}

int node_depth(const struct node *node)
{
    int result = 0;

    if (node->left)
        result = node_depth(node->left);

    if (node->right) {
        int right_depth = node_depth(node->right);
        result = maximum(result, right_depth);
    }
    return 1 + result;
}

int tree_depth(const struct tree *tree)
{
    if (tree && tree->root)
        return node_depth(tree->root);
    return 0;
}

struct position
{
    int validity;
    int value;
};

void recursive_tree_print(const struct node *node, struct position **lines, int line, int column)
{
    if (!node)
        return;
    lines[line][column].validity = 1;
    lines[line][column].value = node->value;

    recursive_tree_print(node->left, lines, line + 1, column * 2);
    recursive_tree_print(node->right, lines, line + 1, column * 2 + 1);
}

void tree_print(const struct tree *tree, FILE *out)
{
    static const char *spaces = "  ";
    static const char *format = "%2i";
    static const char *hline = "--";

    if (!tree || !tree->root)
        return;

    int depth = tree_depth(tree);
    struct position **lines = (struct position **) malloc(sizeof(struct position *) * depth);
    int width = 1;
    for (int i = 0; i < depth; ++i) {
        lines[i] = (struct position *) malloc(sizeof(struct position) * width);
        memset(lines[i], 0, sizeof(struct position) * width);
        width *= 2;
    }
    recursive_tree_print(tree->root, lines, 0, 0);
    int columns = 1;
    for (int line = 0; line < depth; ++line, columns *= 2, width /= 2) {
        for (int column = 0; column < columns; ++column) {
            int s_count = width - 1;
            if (column == 0)
                s_count = width / 2 - 1;

            for (int s = 0; s < s_count; ++s)
                fputs(spaces, out);

            if (lines[line][column].validity)
                fprintf(out, format, lines[line][column].value);
            else
                fputs(spaces, out);
        }
        fputc('\n', out);
    }
    for (int i = 0; i < columns; ++i)
        fputs(hline, out);
    fputc('\n', out);

    for (int i = 0; i < depth; ++i)
        free(lines[i]);
    free(lines);
}
