#include <stdlib.h>

#include "expressions.h"

#include <stdio.h>

#include "domain/models.h"
#include "parsing/parser.h"

ExpressionNode* currentExpression = NULL;

int execute_command(const char* command, void (*presenter)(char*)) {
    if (currentExpression == NULL)
        init_tree(currentExpression);

    CommandType type;
    get_command_type(command, &type);

    presenter("хуй");

    return 0;
}