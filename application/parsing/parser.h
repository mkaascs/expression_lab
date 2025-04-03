#ifndef PARSER_H
#define PARSER_H

#define VARIABLES_COUNT 10

typedef enum {
    Parse, LoadPrefix, LoadPostfix, SavePrefix, SavePostfix, Eval
} CommandType;

typedef struct {
    CommandType type;
    int has_arguments;
    char* arguments;
} ParsedCommand;

typedef struct {
    char* variables[VARIABLES_COUNT];
    char* values[VARIABLES_COUNT];
    int variables_count;
} ParsedEvalCommand;

typedef struct ParsedExpression {
    char* operand;
    char operator;
    struct ParsedExpression *left, *right;
} ParsedExpression;

int parse_command(const char*, ParsedCommand*);
ParsedExpression* parse_expression(const char*);
ParsedEvalCommand* parse_eval_arguments(const char*);
void free_parsed_tree(ParsedExpression*);

#endif //PARSER_H
