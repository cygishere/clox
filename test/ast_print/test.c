#include "astprint.h"
#include "tok.h"

#include <stddef.h>
#include <stdio.h>

int
main (void)
{
  double d0 = 123;
  double d1 = 45.67;

  struct token op0 = token_get_token (LT_STAR, "-", NULL, 1);
  struct token op1 = token_get_token (LT_PLUS, "*", NULL, 1);
  struct token num0 = token_get_token (LT_NUMBER, "", &d0, 1);
  struct token num1 = token_get_token (LT_NUMBER, "", &d1, 1);

  union expr l0 = { .literal = { .type = LE_LITERAL, .value = &num0 } };
  union expr l1 = { .literal = { .type = LE_LITERAL, .value = &num1 } };

  union expr b
      = { .unary = { .type = LE_UNARY, .operator= & op0, .right = &l0 } };

  union expr c = { .grouping = { .type = LE_GROUPING, .expr = &l1 } };

  union expr d
      = { .binary
          = { .type = LE_BINARY, .left = &b, .operator= & op1, .right = &c } };

  ast_print (&d, stdout);
  fputs ("\n", stdout);
}
