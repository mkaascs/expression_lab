#ifndef OPERATIONS_H
#define OPERATIONS_H
#include "domain/models.h"

void save_prefix(const ExpressionNode*, void (*)(char*));
void save_postfix(const ExpressionNode*, void (*)(char*));

#endif //OPERATIONS_H
