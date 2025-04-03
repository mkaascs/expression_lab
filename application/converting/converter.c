#include "converter.h"
#include "../../memory/stats.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

int convert_to_operand(const char* operand, Operand* result) {
    if (result == NULL)
        return 0;

    int is_digit = 1;
    for (int i = 0; operand[i]; i++) {
        if (!isdigit(operand[i])) {
            is_digit = 0;
            break;
        }
    }

    result->is_variable = !is_digit;
    if (is_digit)
        result->number = atoi(operand);
    else
        result->variable = operand[0];

    return 1;
}

int convert_to_entity(const ParsedExpression* parsed, ExpressionNode* result) {
    if (parsed == NULL || result == NULL)
        return 0;

    result->operator = parsed->operator;
    result->operand = NULL;
    result->left = result->right = NULL;

    if (parsed->operand != NULL) {
        result->operand = (Operand*)track_malloc(sizeof(Operand));
        if (result->operand == NULL)
            return 0;

        if (!convert_to_operand(parsed->operand, result->operand)) {
            track_free(result->operand);
            return 0;
        }

        return 1;
    }

    if (parsed->operator != 0) {
        result->left = (ExpressionNode*)track_malloc(sizeof(ExpressionNode));
        if (result->left == NULL)
            return 0;

        if (!convert_to_entity(parsed->left, result->left)) {
            track_free(result->left);
            return 0;
        }

        if (parsed->right) {
            result->right = (ExpressionNode*)track_malloc(sizeof(ExpressionNode));
            if (result->right == NULL)
                return 0;

            if (!convert_to_entity(parsed->right, result->right)) {
                track_free(result->right);
                return 0;
            }
        }
    }

    return 1;
}