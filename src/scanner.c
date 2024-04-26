#include "scanner.h"
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

static bool sc_is_at_end (struct sc *sc);
static void sc_scan_token (struct sc *sc);
static char sc_advance (struct sc *sc);
static void sc_add_token (struct sc *sc, enum token_type type, void *literal);
static void sc_add_token_string (struct sc *sc);
static void sc_add_token_number (struct sc *sc);
static bool sc_match (struct sc *sc, const char expected);
static char sc_peek (struct sc *sc);

static bool sc_is_digit (const char c);
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
                      .start = 0,
                      .current = 0,
                      .line = 1 };
}

struct token *
sc_scan_tokens (struct sc *sc)
{
  while (!sc_is_at_end (sc))
    {
      sc->start = sc->current;
      sc_scan_token (sc);
    }

  return NULL;
}

bool
sc_is_at_end (struct sc *sc)
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
      sc->line++;
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
      lox_error (sc->line, "Unexpected character: '%c'\n", c);
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
  size_t len = sc->current - sc->start;
  char *text = malloc (len + 1);
  memcpy (text, sc->source + sc->start, len);
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
      printf ("%s\n", sc->tokens[i].lexeme);
    }

  for (size_t i = 0; i < sc->num_tokens; ++i)
    {
      free (sc->tokens[i].lexeme);
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
sc_peek (struct sc *sc)
{
  if (sc_is_at_end (sc))
    {
      return 0;
    }
  return sc->source[sc->current];
}

void
sc_add_token_string (struct sc *sc)
{
  while (sc_peek (sc) != '"' && !sc_is_at_end (sc))
    {
      if (sc_peek (sc) == '\n')
        {
          sc->line++;
        }
      sc_advance (sc);
    }

  if (sc_is_at_end (sc))
    {
      lox_error (sc->line, "Unterminated string\n");
      return;
    }

  sc_advance (sc);

  size_t text_len = sc->current - sc->start - 2;
  char *text = malloc (sizeof *text * (text_len + 1));
  memcpy (text, sc->source + (sc->start + 1), text_len);
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

  if (sc_peek (sc) == '.')
    {
      sc_advance (sc);
      while (sc_is_digit (sc_peek (sc)))
        {
          sc_advance (sc);
        }
    }

  size_t text_len = sc->current - sc->start;
  char *text = malloc (sizeof *text * (text_len + 1));
  memcpy (text, sc->source + sc->start, text_len);
  text[text_len] = 0;

  char *endptr = NULL;
  double num = strtod (text, &endptr);
  if (endptr != text + text_len)
    {
      lox_error (sc->line, "Failed to parse number: %s\n", num);
      return;
    }
  if (errno == ERANGE)
    {
      lox_error (sc->line, "Number value overflowed: saw %s, got %f\n", text,
                 num);
      errno = 0;
      return;
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
      fputs ("hello\n", stdout);
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
      = token_get_token (type, text, literal, sc->line);

  return LOX_SC_OK;
}

bool
sc_is_digit (const char c)
{
  return (c >= '0' && c <= '9');
}
