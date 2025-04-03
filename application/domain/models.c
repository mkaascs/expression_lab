#include <stdio.h>

#include "models.h"
#include "../../memory/stats.h"

void free_tree(ExpressionNode* tree) {
    if (tree == NULL)
        return;

    if (tree->operand != NULL) {
        track_free(tree->operand);
        tree->operand = NULL;
    }

    free_tree(tree->left);
    free_tree(tree->right);
    track_free(tree);
}