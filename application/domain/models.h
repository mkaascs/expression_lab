#ifndef MODELS_H
#define MODELS_H

typedef struct {
    union {
        char variable;
        int number;
    };

    int is_variable;
} Operand;

typedef struct ExpressionNode {
    char operator;
    Operand* operand;
    struct ExpressionNode *left, *right;
} ExpressionNode;

void free_tree(ExpressionNode*);
int execute_command(const char*, void (*)(const char*));

#endif //MODELS_H
