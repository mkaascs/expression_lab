#include <stdlib.h>

#include "expressions.h"

#include <stdio.h>

#include "../memory/stats.h"
#include "domain/models.h"
#include "parsing/parser.h"

ExpressionNode* currentExpression = NULL;

int execute_command(const char* command, void (*presenter)(const char*)) {
    if (currentExpression == NULL)
        init_tree(currentExpression);

    ParsedCommand parsed_command;
    if (!parse_command(command, &parsed_command))
        return -1;

    if (parsed_command.type == Parse) {
        ParsedExpression* expression = parse_expression(parsed_command.arguments);
        if (expression == NULL) {
            presenter("Невалидный хуй");
            if (parsed_command.has_arguments)
                track_free(parsed_command.arguments);

            return -1;
        }

        presenter("Валидный хуй");
        free_expression_tree(expression);
    }

    if (parsed_command.has_arguments)
        track_free(parsed_command.arguments);

    return 0;
}