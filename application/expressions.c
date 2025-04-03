#include <stdio.h>
#include <stdlib.h>

#include "expressions.h"

#include <math.h>

#include "converting/converter.h"
#include "parsing/parser.h"
#include "../memory/stats.h"

ExpressionNode* currentExpression = NULL;

int parse(ParsedCommand command, void (*presenter)(const char*)) {
    ParsedExpression* parsed = parse_expression(command.arguments);
    if (parsed == NULL) {
        presenter("incorrect\n");
        return 0;
    }

    if (currentExpression != NULL) {
        free_tree(currentExpression);
        currentExpression = NULL;
    }

    currentExpression = (ExpressionNode*)track_malloc(sizeof(ExpressionNode));
    convert_to_entity(parsed, currentExpression);

    presenter("success\n");
    free_parsed_tree(parsed);
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

int load_prefix(ParsedCommand command, void (*presenter)(const char*)) {
    ParsedExpression* parsed = parse_prefix_expression(command.arguments);
    if (parsed == NULL) {
        presenter("incorrect\n");
        return 0;
    }

    if (currentExpression != NULL) {
        free_tree(currentExpression);
        currentExpression = NULL;
    }

    currentExpression = (ExpressionNode*)track_malloc(sizeof(ExpressionNode));
    convert_to_entity(parsed, currentExpression);

    presenter("success\n");
    free_parsed_tree(parsed);
    return 1;
}

int load_postfix(ParsedCommand command, void (*presenter)(const char*)) {
    ParsedExpression* parsed = parse_postfix_expression(command.arguments);
    if (parsed == NULL) {
        presenter("incorrect\n");
        return 0;
    }

    if (currentExpression != NULL) {
        free_tree(currentExpression);
        currentExpression = NULL;
    }

    currentExpression = (ExpressionNode*)track_malloc(sizeof(ExpressionNode));
    convert_to_entity(parsed, currentExpression);

    presenter("success\n");
    free_parsed_tree(parsed);
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

int eval(ParsedCommand command, void (*presenter)(const char*)) {
    if (currentExpression == NULL) {
        presenter("not_loaded\n");
        return 0;
    }

    ParsedEvalCommand* parsed = (ParsedEvalCommand*)track_malloc(sizeof(ParsedEvalCommand));
    if (!parse_eval_arguments(command.arguments, parsed)) {
        presenter("incorrect\n");
        track_free(parsed);
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

    track_free(parsed);
    return 1;
}

int execute_command(const char* command, void (*presenter)(const char*)) {
    ParsedCommand parsed_command;
    if (!parse_command(command, &parsed_command)) {
        presenter("not parsed\n");
        return -1;
    }

    int command_result = 0;

    if (parsed_command.type == Parse)
        command_result = parse(parsed_command, presenter);

    if (parsed_command.type == SavePrefix)
        command_result = save_prefix(presenter);

    if (parsed_command.type == SavePostfix)
        command_result = save_postfix(presenter);

    if (parsed_command.type == LoadPrefix)
        command_result = load_prefix(parsed_command, presenter);

    if (parsed_command.type == LoadPostfix)
        command_result = load_postfix(parsed_command, presenter);

    if (parsed_command.type == Eval)
        command_result = eval(parsed_command, presenter);

    if (parsed_command.has_arguments)
        track_free(parsed_command.arguments);

    return command_result ? 0 : -1;
}

int free_expression() {
    if (currentExpression == NULL)
        return -1;

    free_tree(currentExpression);
    return 0;
}