#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "parser.h"
#include "../../memory/stats.h"

#define COMMANDS_COUNT 6

typedef struct {
    int position;
    const char* expression;
} ParserState;

const char* command_strings[COMMANDS_COUNT] = {
    "parse", "load_prf", "load_pst", "save_prf", "save_pst", "eval"
};

char* tokenize(char* origin, const char symbol, char** save_pointer) {
    if (origin) *save_pointer = origin;
    else if (*save_pointer == NULL || **save_pointer == '\0') return NULL;

    char* start = *save_pointer;
    while (*start && *start == symbol) start++;

    if (*start == '\0') return NULL;

    char* token = start;
    while (**save_pointer) {
        if (**save_pointer == symbol) {
            **save_pointer = '\0';
            (*save_pointer)++;

            while (**save_pointer && **save_pointer == symbol) (*save_pointer)++;

            return token;
        }

        (*save_pointer)++;
    }

    *save_pointer = NULL;
    return token;
}

int get_command_type(const char* command_string, CommandType* type) {
    for (int index = 0; index < COMMANDS_COUNT; index++) {
        if (strcmp(command_string, command_strings[index]) == 0) {
            *type = index;
            return 1;
        }
    }

    return 0;
}

int parse_command(const char* command, ParsedCommand* parsed_command) {
    const int command_length = strlen(command);
    char* command_copy = (char*)track_malloc(command_length + 1);
    strncpy(command_copy, command, command_length + 1);

    char* save_pointer;
    char* token = tokenize(command_copy, ' ', &save_pointer);

    if (!get_command_type(token, &parsed_command->type)) {
        track_free(command_copy);
        return 0;
    }

    token = tokenize(NULL, ' ', &save_pointer);
    parsed_command->has_arguments = token != NULL;
    if (!parsed_command->has_arguments) {
        parsed_command->arguments = NULL;
        track_free(command_copy);
        return 1;
    }

    const int args_length = strlen(token);
    parsed_command->arguments = (char*)track_malloc(args_length + 1);
    strncpy(parsed_command->arguments, token, args_length + 1);

    track_free(command_copy);
    return 1;
}

ParsedExpression* create_operand_node(char* operand) {
    ParsedExpression* node = track_malloc(sizeof(ParsedExpression));
    if (node == NULL) return NULL;

    node->operand = operand;
    node->operator = 0;
    node->left = node->right = NULL;
    return node;
}

ParsedExpression* create_operator_node(char operator, ParsedExpression* left, ParsedExpression* right) {
    ParsedExpression* node = (ParsedExpression*)track_malloc(sizeof(ParsedExpression));
    if (node == NULL) return NULL;

    node->operand = NULL;
    node->operator = operator;
    node->left = left;
    node->right = right;
    return node;
}

void free_parsed_tree(ParsedExpression* node) {
    if (node == NULL) return;

    free_parsed_tree(node->left);
    free_parsed_tree(node->right);

    if (node->operand)
        track_free(node->operand);

    track_free(node);
}

void skip_spaces(ParserState* state) {
    while (state->expression[state->position] == ' ')
        state->position++;
}

ParsedExpression* parse_operand(ParserState* state);
ParsedExpression* parse_unary(ParserState* state);
ParsedExpression* parse_parentheses(ParserState* state);
ParsedExpression* recursive_parse_expression(ParserState* state, int min_priority, int except_operand);

int is_unary_context(ParserState* state) {
    if (state->position == 0) return 1;

    char prev = state->expression[state->position - 1];
    return (prev == '(' || prev == '+' || prev == '-' || prev == '*' ||
            prev == '/' || prev == '%' || prev == '^' || prev == ' ');
}

ParsedExpression* parse_operand(ParserState* state) {
    skip_spaces(state);

    int sign = 1;
    if (state->expression[state->position] == '-' && isdigit(state->expression[state->position + 1])) {
        sign = -1;
        state->position++;
    }

    int start = state->position;
    while (isalnum(state->expression[state->position]))
        state->position++;

    if (start == state->position) return NULL;

    char* operand = track_strndup(state->expression + start - (sign == -1),
                                state->position - start + (sign == -1));

    return create_operand_node(operand);
}

ParsedExpression* parse_unary(ParserState* state) {
    skip_spaces(state);

    char op = state->expression[state->position];
    if (strchr("+-~", op) && is_unary_context(state)) {
        state->position++;
        ParsedExpression* operand = parse_unary(state);
        return operand ? create_operator_node(op, operand, NULL) : NULL;
    }

    return parse_parentheses(state);
}

ParsedExpression* parse_parentheses(ParserState* state) {
    skip_spaces(state);

    if (state->expression[state->position] != '(')
        return parse_operand(state);

    state->position++;
    ParsedExpression* expr = recursive_parse_expression(state, 0, 1);

    skip_spaces(state);
    if (state->expression[state->position] != ')') {
        free_parsed_tree(expr);
        return NULL;
    }

    state->position++;
    return expr;
}

int get_operator_priority(char op, int is_unary) {
    if (is_unary) {
        switch(op) {
            case '-': case '~': return 5;
            default: return -1;
        }
    }

    switch(op) {
        case '+': case '-': return 1;
        case '*': case '/': case '%': return 2;
        case '^': return 3;
        default: return -1;
    }
}

ParsedExpression* recursive_parse_expression(ParserState* state, int min_priority, int expect_operand) {
    ParsedExpression* left;

    if (expect_operand)
        left = parse_unary(state);

    else left = parse_parentheses(state);

    if (left == NULL)
        return NULL;

    while (1) {
        skip_spaces(state);
        char op = state->expression[state->position];
        if (op == '\0') break;

        int is_unary = 0;
        if (expect_operand && strchr("+-~", op) && is_unary_context(state))
            is_unary = 1;

        int priority = get_operator_priority(op, is_unary);
        if (priority < min_priority) break;

        state->position++;

        ParsedExpression* right;
        if (is_unary)
            right = recursive_parse_expression(state, priority + 1, 1);

        else right = recursive_parse_expression(state, priority + 1, 1);

        if (right == NULL) {
            free_parsed_tree(left);
            return NULL;
        }

        left = create_operator_node(op, left, is_unary ? NULL : right);
    }

    return left;
}

ParsedExpression* parse_expression(const char* expression) {
    ParserState state = {0, expression};
    ParsedExpression* result = recursive_parse_expression(&state, 0, 1);

    if (result == NULL)
        return NULL;

    skip_spaces(&state);
    if (state.expression[state.position] != '\0') {
        free_parsed_tree(result);
        return NULL;
    }

    return result;
}

ParsedEvalCommand* parse_eval_arguments(const char* arguments) {
    return 0;
}