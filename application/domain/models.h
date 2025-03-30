#ifndef MODELS_H
#define MODELS_H

typedef struct {
    union {
        char variable;
        int number;
    };

    int is_positive;
    int is_variable;
} Operand;

typedef struct ExpressionNode {
    char c_operator;
    Operand* operand;
    struct ExpressionNode *left, *right;
} ExpressionNode;

void init_tree(ExpressionNode*);
void free_tree(ExpressionNode*);

#endif //MODELS_H
