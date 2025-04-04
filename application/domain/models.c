#include <stdio.h>

#include "models.h"
#include "../../memory/stats.h"
#include "../parsing/parser.h"
#include "commands/expressions.h"

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

int execute_command(const char* command, void (*presenter)(const char*)) {
    ParsedCommand parsed_command;
    if (!parse_command(command, &parsed_command)) {
        presenter("not parsed\n");
        return -1;
    }

    int command_result = 0;
    if (parsed_command.type == Parse) {
        ParsedExpression* parsed = parse_expression(parsed_command.arguments);
        command_result = parse(parsed, presenter);
        free_parsed_tree(parsed);
    }

    if (parsed_command.type == SavePrefix)
        command_result = save_prefix(presenter);

    if (parsed_command.type == SavePostfix)
        command_result = save_postfix(presenter);

    if (parsed_command.type == LoadPrefix) {
        ParsedExpression* parsed = parse_prefix_expression(parsed_command.arguments);
        command_result = load_prefix(parsed, presenter);
        free_parsed_tree(parsed);
    }

    if (parsed_command.type == LoadPostfix) {
        ParsedExpression* parsed = parse_postfix_expression(parsed_command.arguments);
        command_result = load_postfix(parsed, presenter);
        free_parsed_tree(parsed);
    }

    if (parsed_command.type == Eval) {
        ParsedEvalCommand* parsed = (ParsedEvalCommand*)track_malloc(sizeof(ParsedEvalCommand));
        parse_eval_arguments(parsed_command.arguments, parsed);
        command_result = eval(parsed, presenter);
        track_free(parsed);
    }

    if (parsed_command.type == Free)
        command_result = free_expression();

    if (parsed_command.has_arguments)
        track_free(parsed_command.arguments);

    return command_result ? 0 : -1;
}