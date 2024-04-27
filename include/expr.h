#ifndef LOX_EXPR_H
#define LOX_EXPR_H

#include "token.h"

enum expr_type
{
  LE_BINARY,
  LE_GROUPING,
  LE_LITERAL,
  LE_UNARY
};

struct expr_binary
{
  enum expr_type type;
  union expr *left;
  struct token *operator;
  union expr *right;
};

struct expr_grouping
{
  enum expr_type type;
  union expr *expr;
};

struct expr_literal
{
  enum expr_type type;
  struct token *value;
};

struct expr_unary
{
  enum expr_type type;
  struct token *operator;
  union expr *right;
};

union expr
{
  enum expr_type type;
  struct expr_binary binary;
  struct expr_grouping grouping;
  struct expr_literal literal;
  struct expr_unary unary;
};

#endif /* LOX_EXPR_H */
