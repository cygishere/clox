#ifndef LOX_PARSER_H
#define LOX_PARSER_H

#include "token.h"

#include <stddef.h>

struct ps
{
  const struct token *tokens;
  size_t current;
};

struct ps ps_get_parser (const struct token *tokens);
union expr *ps_expression (struct ps *ps);

#endif /* LOX_PARSER_H */
