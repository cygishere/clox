#include "scanner.h"
#include "hash.h"
#include "lox.h"
#include "token_type.h"

#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum sc_status
{
  LOX_SC_OK = 0,
  LOX_SC_FAIL_REALLOC
};

static bool sc_is_at_end (const struct sc *sc);
static void sc_scan_token (struct sc *sc);
static char sc_advance (struct sc *sc);
static void sc_add_token (struct sc *sc, enum token_type type, void *literal);
static void sc_add_token_string (struct sc *sc);
static void sc_add_token_number (struct sc *sc);
static void sc_add_token_identifier (struct sc *sc);
static bool sc_match (struct sc *sc, const char expected);
static char sc_peek (const struct sc *sc);
static char sc_peek_next (const struct sc *sc);

static bool sc_is_digit (const char c);
static bool sc_is_alpha (const char c);
static enum sc_status sc_tokens_append (struct sc *sc, enum token_type type,
                                        char *text, void *literal);

struct sc
sc_get_scanner (const char *source)
{
  return (struct sc){ .source = source,
                      .source_len = strlen (source),
                      .tokens = NULL,
                      .num_tokens = 0,
                      .cap_tokens = 0,
                      .token_start = 0,
                      .current = 0,
                      .current_line = 1 };
}

struct token *
sc_scan_tokens (struct sc *sc)
{
  while (!sc_is_at_end (sc))
    {
      sc->token_start = sc->current;
      sc_scan_token (sc);
    }

  return NULL;
}

bool
sc_is_at_end (const struct sc *sc)
{
  return sc->current >= sc->source_len;
}

void
sc_scan_token (struct sc *sc)
{
  char c = sc_advance (sc);
  printf ("scanning ... %ld: %c\n", sc->current, c);
  switch (c)
    {
    case '(':
      sc_add_token (sc, LT_LEFT_PAREN, NULL);
      break;
    case ')':
      sc_add_token (sc, LT_RIGHT_PAREN, NULL);
      break;
    case '{':
      sc_add_token (sc, LT_LEFT_BRACE, NULL);
      break;
    case '}':
      sc_add_token (sc, LT_RIGHT_BRACE, NULL);
      break;
    case ',':
      sc_add_token (sc, LT_COMMA, NULL);
      break;
    case '.':
      sc_add_token (sc, LT_DOT, NULL);
      break;
    case '-':
      sc_add_token (sc, LT_MINUS, NULL);
      break;
    case '+':
      sc_add_token (sc, LT_PLUS, NULL);
      break;
    case ';':
      sc_add_token (sc, LT_SEMICOLON, NULL);
      break;
    case '*':
      sc_add_token (sc, LT_STAR, NULL);
      break;

    case '!':
      sc_add_token (sc, sc_match (sc, '=') ? LT_BANG_EQUAL : LT_BANG, NULL);
      break;
    case '=':
      sc_add_token (sc, sc_match (sc, '=') ? LT_EQUAL_EQUAL : LT_EQUAL, NULL);
      break;
    case '<':
      sc_add_token (sc, sc_match (sc, '=') ? LT_LESS_EQUAL : LT_LESS, NULL);
      break;
    case '>':
      sc_add_token (sc, sc_match (sc, '=') ? LT_GREATER_EQUAL : LT_GREATER,
                    NULL);
      break;

    case '/':
      if (sc_match (sc, '/'))
        {
          while (sc_peek (sc) != '\n' && !sc_is_at_end (sc))
            {
              sc_advance (sc);
            }
        }
      else
        {
          sc_add_token (sc, LT_SLASH, NULL);
        }
      break;

    case ' ':
    case '\r':
    case '\t':
      break;

    case '\n':
      sc->current_line++;
      break;

    case '"':
      sc_add_token_string (sc);
      break;

    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      sc_add_token_number (sc);
      break;

    default:
      if (sc_is_alpha (c))
        {
          sc_add_token_identifier (sc);
        }
      else
        {
          lox_error (sc->current_line, "Unexpected character: '%c'\n", c);
        }
      break;
    }
}

char
sc_advance (struct sc *sc)
{
  return sc->source[sc->current++];
}

void
sc_add_token (struct sc *sc, enum token_type type, void *literal)
{
  size_t len = sc->current - sc->token_start;
  char *text = malloc (len + 1);
  memcpy (text, sc->source + sc->token_start, len);
  text[len] = 0;

  if (LOX_SC_OK != sc_tokens_append (sc, type, text, literal))
    {
      free (text);
    }
}

void
sc_free_tokens (struct sc *sc)
{
  for (size_t i = 0; i < sc->num_tokens; ++i)
    {
      printf ("token %zu: %s, ", i, sc->tokens[i].lexeme);
      switch (sc->tokens[i].type)
        {
        case LT_NUMBER:
          printf ("number = %.2f\n", *(double *)sc->tokens[i].literal);
          break;
        case LT_AND:
        case LT_CLASS:
        case LT_ELSE:
        case LT_FALSE:
        case LT_FOR:
        case LT_FUN:
        case LT_IF:
        case LT_NIL:
        case LT_OR:
        case LT_PRINT:
        case LT_RETURN:
        case LT_SUPER:
        case LT_THIS:
        case LT_TRUE:
        case LT_VAR:
        case LT_WHILE:
          printf ("keyword\n");
          break;
        case LT_IDENTIFIER:
          printf ("identifier\n");
          break;
        default:
          puts ("");
          break;
        }
    }

  for (size_t i = 0; i < sc->num_tokens; ++i)
    {
      if (NULL != sc->tokens[i].lexeme)
        {
          free (sc->tokens[i].lexeme);
        }
      if (NULL != sc->tokens[i].literal)
        {
          free (sc->tokens[i].literal);
        }
    }
  free (sc->tokens);
  sc->tokens = NULL;
}

bool
sc_match (struct sc *sc, const char expected)
{
  if (sc_is_at_end (sc))
    {
      return false;
    }
  if (sc->source[sc->current] != expected)
    {
      return false;
    }

  sc->current++;
  return true;
}

char
sc_peek (const struct sc *sc)
{
  if (sc_is_at_end (sc))
    {
      return 0;
    }
  return sc->source[sc->current];
}

char
sc_peek_next (const struct sc *sc)
{
  if (sc->current + 1 >= sc->source_len)
    {
      return 0;
    }
  return sc->source[sc->current + 1];
}

void
sc_add_token_string (struct sc *sc)
{
  while (sc_peek (sc) != '"' && !sc_is_at_end (sc))
    {
      if (sc_peek (sc) == '\n')
        {
          sc->current_line++;
        }
      sc_advance (sc);
    }

  if (sc_is_at_end (sc))
    {
      lox_error (sc->current_line, "Unterminated string\n");
      return;
    }

  sc_advance (sc);

  size_t text_len = sc->current - sc->token_start - 2;
  char *text = malloc (sizeof *text * (text_len + 1));
  memcpy (text, sc->source + (sc->token_start + 1), text_len);
  text[text_len] = 0;

  sc_tokens_append (sc, LT_STRING, text, NULL);
}

void
sc_add_token_number (struct sc *sc)
{
  while (sc_is_digit (sc_peek (sc)))
    {
      sc_advance (sc);
    }

  if (sc_peek (sc) == '.' && sc_is_digit (sc_peek_next (sc)))
    {
      sc_advance (sc);
      while (sc_is_digit (sc_peek (sc)))
        {
          sc_advance (sc);
        }
    }

  size_t text_len = sc->current - sc->token_start;
  char *text = malloc (sizeof *text * (text_len + 1));
  memcpy (text, sc->source + sc->token_start, text_len);
  text[text_len] = 0;

  double *pnum = malloc (sizeof *pnum);
  char *endptr = NULL;
  *pnum = strtod (text, &endptr);
  if (endptr != text + text_len)
    {
      lox_error (sc->current_line, "Failed to parse number: %s\n", *pnum);
      return;
    }
  if (errno == ERANGE)
    {
      lox_error (sc->current_line, "Number value overflowed: saw %s, got %f\n",
                 text, *pnum);
      errno = 0;
      return;
    }
  sc_tokens_append (sc, LT_NUMBER, text, pnum);
}

void
sc_add_token_identifier (struct sc *sc)
{
  while (sc_is_alpha (sc_peek (sc)))
    {
      sc_advance (sc);
    }

  size_t text_len = sc->current - sc->token_start;
  char *text = malloc (sizeof *text * (text_len + 1));
  memcpy (text, sc->source + sc->token_start, text_len);
  text[text_len] = 0;

  unsigned hash = lox_fnv (text, text_len);
  switch (hash)
    {
    case LT_AND:
    case LT_CLASS:
    case LT_ELSE:
    case LT_FALSE:
    case LT_FOR:
    case LT_FUN:
    case LT_IF:
    case LT_NIL:
    case LT_OR:
    case LT_PRINT:
    case LT_RETURN:
    case LT_SUPER:
    case LT_THIS:
    case LT_TRUE:
    case LT_VAR:
    case LT_WHILE:
      sc_tokens_append (sc, hash, text, NULL);
      break;
    default:
      sc_tokens_append (sc, LT_IDENTIFIER, text, NULL);
      break;
    }
}

enum sc_status
sc_tokens_append (struct sc *sc, enum token_type type, char *text,
                  void *literal)
{
  size_t num_tokens = sc->num_tokens + 1;

  if (sc->cap_tokens < num_tokens)
    {
      size_t cap_tokens = num_tokens * 2;
      sc->tokens = realloc (sc->tokens, sizeof *sc->tokens * cap_tokens);
      if (sc->tokens == NULL)
        {
          fprintf (stderr, "Failed to realloc\n");
          return LOX_SC_FAIL_REALLOC;
        }
      sc->cap_tokens = cap_tokens;
    }

  sc->num_tokens = num_tokens;
  sc->tokens[sc->num_tokens - 1]
      = token_get_token (type, text, literal, sc->current_line);

  return LOX_SC_OK;
}

bool
sc_is_digit (const char c)
{
  return (c >= '0' && c <= '9');
}

bool
sc_is_alpha (const char c)
{
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
