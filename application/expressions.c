#include <stdio.h>
#include <stdlib.h>

#include "expressions.h"
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