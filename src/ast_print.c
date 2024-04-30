#include "astprint.h"

#include <stdarg.h>
#include <stdio.h>

void
ast_print (const union expr *expr, FILE *f)
{
  switch (expr->type)
    {
    case LE_BINARY:
      fprintf (f, " (%s", expr->binary.operator->lexeme);
      ast_print (expr->binary.left, f);
      ast_print (expr->binary.right, f);
      fputs (")", f);
      break;
    case LE_GROUPING:
      fputs (" (group", f);
      ast_print (expr->grouping.expr, f);
      fputs (")", f);
      break;
    case LE_LITERAL:
      switch (expr->literal.value->type)
        {
        case LT_IDENTIFIER:
        case LT_STRING:
          fprintf (f, " %s", expr->literal.value->lexeme);
          break;
        case LT_NUMBER:
          fprintf (f, " %.2f", *(double *)expr->literal.value->literal);
          break;
        default:
          fprintf (f, " (ERROR: %s not a literal)",
                   expr->literal.value->lexeme);
          break;
        }
      break;
    case LE_UNARY:
      fprintf (f, " (%s", expr->unary.operator->lexeme);
      ast_print (expr->unary.right, f);
      fputs (")", f);
      break;
    default:
      fprintf (stderr, "What expr is this? %d\n", expr->type);
    }
}
