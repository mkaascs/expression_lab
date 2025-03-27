#ifndef MODELS_H
#define MODELS_H

#define MAX_EXPRESSION_LENGTH 256

typedef struct ExpressionNode {
    char c_operator;
    int is_variable;
    union {
        int number;
        char variable;
    } data;

    struct ExpressionNode *left, *right;
} ExpressionNode;

void init_tree(ExpressionNode*);
void free_tree(ExpressionNode*);

#endif //MODELS_H
