#include "token.h"

#include <stdlib.h>

struct token
token_get_token (enum token_type type, char *lexeme, void *literal, int line)
{
  return (struct token){
    .type = type, .lexeme = lexeme, .literal = literal, .line = line
  };
}

const char *
token_to_string (struct token token)
{
  return "todo";
}
