#ifndef PARSER_H
#define PARSER_H

typedef enum {
    Parse, LoadPrefix, LoadPostfix, SavePrefix, SavePostfix, Eval
} CommandType;

typedef struct {
    CommandType type;
    int has_arguments;
    char* arguments;
} ParsedCommand;

typedef struct ParsedExpression {
    char* operand;
    char operator;
    struct ParsedExpression *left, *right;
} ParsedExpression;

int parse_command(const char*, ParsedCommand*);
ParsedExpression* parse_expression(const char* expression);
void free_expression_tree(ParsedExpression*);

#endif //PARSER_H
