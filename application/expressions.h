#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H
#include "domain/models.h"

ExpressionNode* currentExpression;
int execute_command(const char*, void (*)(const char*));
int free_expression();

#endif //EXPRESSIONS_H
