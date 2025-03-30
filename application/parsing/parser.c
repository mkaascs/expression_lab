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

void free_expression_tree(ParsedExpression* node) {
    if (node == NULL) return;

    free_expression_tree(node->left);
    free_expression_tree(node->right);

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
ParsedExpression* recursive_parse_expression(ParserState* state, int min_priority);

ParsedExpression* parse_operand(ParserState* state) {
    skip_spaces(state);
    if (!isdigit(state->expression[state->position]) && !isalpha(state->expression[state->position]))
        return NULL;

    int start = state->position;
    while (isdigit(state->expression[state->position]) || isalpha(state->expression[state->position]))
        state->position++;

    char* operand = track_strndup(state->expression + start, state->position - start);
    if (operand == NULL)
        return NULL;

    ParsedExpression* node = create_operand_node(operand);
    if (node != NULL)
        return node;

    track_free(operand);
    return NULL;
}

ParsedExpression* parse_unary(ParserState* state) {
    skip_spaces(state);

    char op = state->expression[state->position];
    if (op != '-' && op != '~')
        return parse_operand(state);

    state->position++;

    skip_spaces(state);
    if (state->expression[state->position] == ')')
        return NULL;

    ParsedExpression* operand = parse_unary(state);
    if (operand == NULL)
        return NULL;

    return create_operator_node(op, operand, NULL);
}

ParsedExpression* parse_parentheses(ParserState* state) {
    skip_spaces(state);

    if (state->expression[state->position] != '(')
        return parse_unary(state);

    state->position++;
    ParsedExpression* expr = recursive_parse_expression(state, 0);

    skip_spaces(state);
    if (state->expression[state->position] != ')') {
        free_expression_tree(expr);
        return NULL;
    }

    state->position++;
    return expr;
}

int get_operator_priority(char op) {
    switch (op) {
        case '+': case '-': return 1;
        case '*': case '/': case '%': return 2;
        case '^': return 3;
        case '~': return 4;
        default: return -1;
    }
}

ParsedExpression* recursive_parse_expression(ParserState* state, int min_priority) {
    ParsedExpression* left = parse_parentheses(state);
    if (left == NULL) return NULL;

    while (1) {
        skip_spaces(state);
        char op = state->expression[state->position];
        int priority = get_operator_priority(op);

        if (priority < min_priority) break;

        state->position++;
        ParsedExpression* right = recursive_parse_expression(state, priority + 1);
        if (right == NULL) {
            free_expression_tree(left);
            return NULL;
        }

        left = create_operator_node(op, left, right);
    }

    return left;
}

ParsedExpression* parse_expression(const char* expression) {
    ParserState state = { 0, expression };
    ParsedExpression* result = recursive_parse_expression(&state, 0);
    if (result == NULL)
        return NULL;

    skip_spaces(&state);
    if (state.expression[state.position] != '\0') {
        free_expression_tree(result);
        return NULL;
    }

    return result;
}