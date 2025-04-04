#ifndef EXPRESSIONS_H
#define EXPRESSIONS_H
#include "../../parsing/parser.h"

int parse(const ParsedExpression*, void (*)(const char*));
int save_prefix(void (*)(const char*));
int save_postfix(void (*)(const char*));
int load_prefix(const ParsedExpression*, void (*)(const char*));
int load_postfix(const ParsedExpression*, void (*)(const char*));
int eval(ParsedEvalCommand*, void (*)(const char*));

int free_expression();

#endif //EXPRESSIONS_H
