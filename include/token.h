#ifndef LOX_TOKEN_H
#define LOX_TOKEN_H

#include "token_type.h"

struct token
{
  enum token_type type;
  char *lexeme;
  void *literal;
  int line;
};

struct token token_get_token (enum token_type type, char *lexeme,
                              void *literal, int line);

const char *token_to_string (struct token token);

#endif /* LOX_TOKEN_H */
