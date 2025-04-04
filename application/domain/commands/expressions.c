#include <stdio.h>
#include <stdlib.h>

#include "expressions.h"
#include "../../parsing/parser.h"
#include "../models.h"

#include <math.h>

#include "../../../memory/stats.h"
#include "../converting/converter.h"

ExpressionNode* currentExpression = NULL;

void switch_tree(const ParsedExpression* newTree) {
    if (currentExpression != NULL) {
        free_tree(currentExpression);
        currentExpression = NULL;
    }

    currentExpression = (ExpressionNode*)track_malloc(sizeof(ExpressionNode));
    convert_to_entity(newTree, currentExpression);
}

int parse(const ParsedExpression* parsed, void (*presenter)(const char*)) {
    if (parsed == NULL) {
        presenter("incorrect\n");
        return 0;
    }

    switch_tree(parsed);
    presenter("success\n");
    return 1;
}

void print_prefix_recursive(const ExpressionNode* node, void (*presenter)(const char*)) {
    if (node == NULL)
        return;

    char buffer[32];
    if (node->operator != 0) {
        char c_operator[2] = { node->operator, '\0' };
        presenter(c_operator);

        presenter("(");
        print_prefix_recursive(node->left, presenter);
        if (node->right != NULL) {
            presenter(",");
            print_prefix_recursive(node->right, presenter);
        }

        presenter(")");
        return;
    }

    if (node->operand == NULL)
        return;

    if (node->operand->is_variable) {
        char variable[2] = { node->operand->variable, '\0' };
        presenter(variable);
        return;
    }

    sprintf(buffer, "%d", node->operand->number);
    presenter(buffer);
}

void print_postfix_recursive(const ExpressionNode* node, void (*presenter)(const char*)) {
    if (node == NULL)
        return;

    if (node->operator != 0) {
        presenter("(");

        if (node->left != NULL)
            print_postfix_recursive(node->left, presenter);

        if (node->left != NULL && node->right != NULL)
            presenter(",");

        if (node->right != NULL)
            print_postfix_recursive(node->right, presenter);

        presenter(")");
        char c_operator[2] = { node->operator, '\0' };
        presenter(c_operator);
        return;
    }

    if (node->operand == NULL)
        return;

    if (node->operand->is_variable) {
        char variable[2] = { node->operand->variable, '\0' };
        presenter(variable);
        return;
    }

    char buffer[32];
    sprintf(buffer, "%d", node->operand->number);
    presenter(buffer);
}

int save_prefix(void (*presenter)(const char*)) {
    if (currentExpression == NULL)
        return 0;

    print_prefix_recursive(currentExpression, presenter);
    presenter("\n");
    return 1;
}

int save_postfix(void (*presenter)(const char*)) {
    if (currentExpression == NULL) return 0;

    print_postfix_recursive(currentExpression, presenter);
    presenter("\n");
    return 1;
}

int load_prefix(const ParsedExpression* parsed, void (*presenter)(const char*)) {
    if (parsed == NULL) {
        presenter("incorrect\n");
        return 0;
    }

    switch_tree(parsed);
    presenter("success\n");
    return 1;
}

int load_postfix(const ParsedExpression* parsed, void (*presenter)(const char*)) {
    if (parsed == NULL) {
        presenter("incorrect\n");
        return 0;
    }

    switch_tree(parsed);
    presenter("success\n");
    return 1;
}

typedef enum {
    EvalOK, NoVarValues, MathError, InvalidNode, UnknownOperator
} EvalError;

int get_variable_value(const char variable, ParsedEvalCommand parsed, int* value) {
    for (size_t index = 0; index < parsed.variables_count; index++) {
        if (variable == parsed.variables[index]) {
            *value = parsed.values[index];
            return 1;
        }
    }

    return 0;
}

int eval_node(const ExpressionNode* node, ParsedEvalCommand parsed, EvalError* error) {
    if (node == NULL || *error != EvalOK)
        return 0;

    if (node->operand != NULL) {
        if (!node->operand->is_variable)
            return node->operand->number;

        int value = 0;
        if (!get_variable_value(node->operand->variable, parsed, &value)) {
            *error = NoVarValues;
            return 0;
        }

        return value;
    }

    if (node->operator == 0) {
        *error = InvalidNode;
        return 0;
    }

    // Unary
    if ((node->operator == '-' || node->operator == '~') && node->left != NULL && node->right == NULL) {
        int value = eval_node(node->left, parsed, error);
        if (*error > 0)
            return 0;

        switch(node->operator) {
            case '-': return -value;
            case '~': return abs(value);
            default: {
                *error = UnknownOperator;
                return 0;
            }
        }
    }

    if (node->left == NULL && node->right == NULL) {
        *error = InvalidNode;
        return 0;
    }

    int left = eval_node(node->left, parsed, error);
    if (*error != EvalOK) return 0;

    int right = eval_node(node->right, parsed, error);
    if (*error != EvalOK) return 0;

    switch (node->operator) {
        case '+': return left + right;
        case '-': return left - right;
        case '*': return left * right;
        case '/':
            if (right == 0) {
                *error = MathError;
                return 0;
            }

            return left / right;

        case '%':
            if (right == 0) {
                *error = MathError;
                return 0;
            }

            return left % right;

        case '^':
            if (right < 0) {
                *error = MathError;
                return 0;
            }

            return (int)pow(left, right);

        default:
            *error = UnknownOperator;
            return 0;
    }
}

int eval(ParsedEvalCommand* parsed, void (*presenter)(const char*)) {
    if (parsed == NULL) {
        presenter("incorrect\n");
        return 0;
    }

    EvalError error = EvalOK;
    int result = eval_node(currentExpression, *parsed, &error);

    switch (error) {
        case EvalOK: {
            char buffer[32];
            sprintf(buffer, "%d\n", result);
            presenter(buffer);
            break;
        }

        case NoVarValues:
            presenter("no_var_values\n");
            break;

        case MathError:
            presenter("error\n");
            break;

        case UnknownOperator:
            presenter("incorrect\n");
            break;

        default: ;
    }

    return 1;
}

int free_expression() {
    if (currentExpression == NULL)
        return -1;

    free_tree(currentExpression);
    return 0;
}