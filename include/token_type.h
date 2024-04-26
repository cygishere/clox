#ifndef TOKEN_TYPE_H
#define TOKEN_TYPE_H

enum token_type
{
  /* Single-character tokens.*/
  LT_LEFT_PAREN,
  LT_RIGHT_PAREN,
  LT_LEFT_BRACE,
  LT_RIGHT_BRACE,
  LT_COMMA,
  LT_DOT,
  LT_MINUS,
  LT_PLUS,
  LT_SEMICOLON,
  LT_SLASH,
  LT_STAR,

  /* One or two character tokens.*/
  LT_BANG,
  LT_BANG_EQUAL,
  LT_EQUAL,
  LT_EQUAL_EQUAL,
  LT_GREATER,
  LT_GREATER_EQUAL,
  LT_LESS,
  LT_LESS_EQUAL,

  /* Literals.*/
  LT_IDENTIFIER,
  LT_STRING,
  LT_NUMBER,

  /* Keywords.*/
  LT_AND,
  LT_CLASS,
  LT_ELSE,
  LT_FALSE,
  LT_FUN,
  LT_FOR,
  LT_IF,
  LT_NIL,
  LT_OR,
  LT_PRINT,
  LT_RETURN,
  LT_SUPER,
  LT_THIS,
  LT_TRUE,
  LT_VAR,
  LT_WHILE,

  LT_EOF
};

#endif /* TOKEN_TYPE_H */
