#include "operations.h"

#include <stdio.h>

void save_prefix_recursive(const ExpressionNode* node, void (*presenter)(char*)) {
    if (node == NULL)
        return;

    char output[16];
    if (node->c_operator != '\0')
        sprintf(output, "%c ", node->c_operator);

    else if (node->is_variable)
        sprintf(output, "%c ", node->data.variable);

    else sprintf(output, "%d ", node->data.number);

    presenter(output);
    save_prefix_recursive(node->left, presenter);
    save_prefix_recursive(node->right, presenter);
}

void save_postfix_recursive(const ExpressionNode* node, void (*presenter)(char*)) {
    if (node == NULL)
        return;

    save_postfix_recursive(node->left, presenter);
    save_postfix_recursive(node->right, presenter);

    char output[16];
    if (node->c_operator != '\0')
        sprintf(output, "%c ", node->c_operator);

    else if (node->is_variable)
        sprintf(output, "%c ", node->data.variable);

    else sprintf(output, "%d ", node->data.number);

    presenter(output);
}

void save_prefix(const ExpressionNode* tree, void (*presenter)(char*)) {
    if (tree == NULL || presenter == NULL)
        return;

    save_prefix_recursive(tree, presenter);
    presenter("\n");
}

void save_postfix(const ExpressionNode* tree, void (*presenter)(char*)) {
    if (tree == NULL || presenter == NULL)
        return;

    save_postfix_recursive(tree, presenter);
    presenter("\n");
}