#ifndef PARSER_H
#define PARSER_H

#include <stdlib.h>

#define MAX_VARIABLES 10

typedef enum {
    Parse, LoadPrefix, LoadPostfix, SavePrefix, SavePostfix, Eval
} CommandType;

typedef struct {
    CommandType type;
    int has_arguments;
    char* arguments;
} ParsedCommand;

typedef struct {
    char variables[MAX_VARIABLES];
    int values[MAX_VARIABLES];
    size_t variables_count;
} ParsedEvalCommand;

typedef struct ParsedExpression {
    char* operand;
    char operator;
    struct ParsedExpression *left, *right;
} ParsedExpression;

int parse_command(const char*, ParsedCommand*);
int parse_eval_arguments(const char*, ParsedEvalCommand*);
ParsedExpression* parse_expression(const char*);
void free_parsed_tree(ParsedExpression*);

#endif //PARSER_H
