#ifndef AST_PRINT_H
#define AST_PRINT_H

#include "expr.h"

#include <stdio.h>

void ast_print (const union expr *expr, FILE *f);

#endif /* AST_PRINT_H */
