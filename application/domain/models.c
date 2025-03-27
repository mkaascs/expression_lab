#include <stdio.h>

#include "models.h"
#include "../../memory/stats.h"

void init_tree(ExpressionNode* tree) {
    if (tree == NULL)
        return;

    tree->c_operator = '\0';
    tree->is_variable = 0;
    tree->data.number = 0;
    tree->left = NULL;
    tree->right = NULL;
}

void free_tree(ExpressionNode* tree) {
    if (tree == NULL)
        return;

    free_tree(tree->left);
    free_tree(tree->right);
    track_free(tree);
}