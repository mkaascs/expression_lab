#ifndef CONVERTER_H
#define CONVERTER_H

#include "../../domain/models.h"
#include "../../parsing/parser.h"

int convert_to_entity(const ParsedExpression*, ExpressionNode*);

#endif //CONVERTER_H
