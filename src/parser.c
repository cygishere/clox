#include "parser.h"
#include "expr.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
  expression    -> equality ;
  equality      -> comparison ( ( "!=" | "==" ) comparison )* ;
  comparison    -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
  term          -> factor ( ( "-" | "+" ) factor )* ;
  factor        -> unary ( ( "/" | "*" ) unary )* ;
  unary         -> ( "!" | "-" ) unary
                 | primary ;
  primary       -> NUMBER | STRING | "true" | "false" | "nil"
                 | "(" expression ")" ;
*/
static union expr *ps_equality (struct ps *ps);
static union expr *ps_comparison (struct ps *ps);
static union expr *ps_term (struct ps *ps);
static union expr *ps_factor (struct ps *ps);
static union expr *ps_unary (struct ps *ps);
static union expr *ps_primary (struct ps *ps);

static bool ps_is_at_end (const struct ps *ps);
static const struct token *ps_advance (struct ps *ps);
static const struct token *ps_peek_cur (const struct ps *ps);
static const struct token *ps_peek_prev (const struct ps *ps);
static const struct token *ps_peek_next (const struct ps *ps);

struct ps
ps_get_parser (const struct token *tokens)
{
  return (struct ps){ .tokens = tokens, .current = 0 };
}

/**
 * expression -> equality ;
 */
union expr *
ps_expression (struct ps *ps)
{
  return ps_equality (ps);
}

/**
 * equality -> comparison ( ( "!=" | "==" ) comparison )* ;
 */
union expr *
ps_equality (struct ps *ps)
{
  union expr *e = ps_comparison (ps);

  bool keep_going = true;
  while (keep_going)
    {
      switch (ps->tokens[ps->current].type)
        {
        case LT_BANG_EQUAL:
        case LT_EQUAL_EQUAL:
          {
            ps_advance (ps);

            const struct token *operator= ps_peek_prev (ps);
            union expr *right = ps_comparison (ps);

            union expr *new = malloc (sizeof *new);
            new->unary = (struct expr_unary){
              .type = LE_UNARY, .operator= operator, .right = right };

            e = new;
          }
          break;
        default:
          keep_going = false;
          break;
        }
    }

  return e;
}

/**
 * comparison -> term ( ( ">" | ">=" | "<" | "<=" ) term )* ;
 */
union expr *
ps_comparison (struct ps *ps)
{
  union expr *e = ps_term (ps);

  bool keep_going = true;
  while (keep_going)
    {
      switch (ps->tokens[ps->current].type)
        {
        case LT_GREATER:
        case LT_GREATER_EQUAL:
        case LT_LESS:
        case LT_LESS_EQUAL:
          {
            ps_advance (ps);

            const struct token *op = ps_peek_prev (ps);
            union expr *right = ps_term (ps);

            union expr *new = malloc (sizeof *new);
            new->binary = (struct expr_binary){
              .type = LE_BINARY, .left = e, .operator= op, .right = right
            };

            e = new;
          }
          break;
        default:
          keep_going = false;
          break;
        }
    }

  return e;
}

/**
 * term -> factor ( ( "-" | "+" ) factor )* ;
 */
union expr *
ps_term (struct ps *ps)
{
  union expr *e = ps_factor (ps);

  bool keep_going = true;
  while (keep_going)
    {
      switch (ps->tokens[ps->current].type)
        {
        case LT_MINUS:
        case LT_PLUS:
          {
            ps_advance (ps);

            union expr *new = malloc (sizeof *new);
            const struct token *op = ps_peek_prev (ps);
            union expr *right = ps_factor (ps);
            new->binary = (struct expr_binary){
              .type = LE_BINARY, .left = e, .operator= op, .right = right
            };

            e = new;
          }
          break;
        default:
          keep_going = false;
          break;
        }
    }

  return e;
}

/**
 * factor -> unary ( ( "/" | "*" ) unary )* ;
 */
union expr *
ps_factor (struct ps *ps)
{
  union expr *e = ps_unary (ps);

  bool keep_going = true;
  while (keep_going)
    {
      switch (ps->tokens[ps->current].type)
        {
        case LT_SLASH:
        case LT_STAR:
          {
            ps_advance (ps);

            union expr *new = malloc (sizeof *new);
            const struct token *op = ps_peek_prev (ps);
            union expr *right = ps_unary (ps);
            new->binary = (struct expr_binary){
              .type = LE_BINARY, .left = e, .operator= op, .right = right
            };

            e = new;
          }
          break;
        default:
          keep_going = false;
          break;
        }
    }

  return e;
}

/**
 * unary -> ( "!" | "-" ) unary
 *        | primary ;
 */
union expr *
ps_unary (struct ps *ps)
{
  bool keep_going = true;
  while (keep_going)
    {
      switch (ps->tokens[ps->current].type)
        {
        case LT_BANG:
        case LT_MINUS:
          {
            ps_advance (ps);

            union expr *new = malloc (sizeof *new);
            const struct token *op = ps_peek_prev (ps);
            union expr *right = ps_unary (ps);
            new->unary = (struct expr_unary){ .type = LE_UNARY,
                                              .operator= op,
                                              .right = right };
            return new;
          }
          break;
        default:
          keep_going = false;
          break;
        }
    }

  return ps_primary (ps);
}

/**
 * primary -> NUMBER | STRING | "true" | "false" | "nil"
 *          | "(" expression ")" ;
 */
union expr *
ps_primary (struct ps *ps)
{
  switch (ps->tokens[ps->current].type)
    {
    case LT_NUMBER:
    case LT_STRING:
    case LT_TRUE:
    case LT_FALSE:
    case LT_NIL:
      {
        ps_advance (ps);

        union expr *new = malloc (sizeof *new);
        const struct token *literal = ps_peek_prev (ps);
        new->literal
            = (struct expr_literal){ .type = LE_LITERAL, .value = literal };
        return new;
      }
    case LT_LEFT_PAREN:
      {
        ps_advance (ps);
        union expr *expr = ps_expression (ps);

        if (ps_peek_cur (ps)->type == LT_RIGHT_PAREN)
          {
            union expr *new = malloc (sizeof *new);
            new->grouping
                = (struct expr_grouping){ .type = LE_GROUPING, .expr = expr };
            ps_advance (ps);
            return new;
          }
        else
          {
            fprintf (stderr, "Expect ')' after expression.\n");
            return NULL;
          }
      }
    default:
      fprintf (stderr, "What is this literal?\n");
      return NULL;
    }
}

bool
ps_is_at_end (const struct ps *ps)
{
  return (ps_peek_cur (ps)->type == LT_EOF);
}

const struct token *
ps_advance (struct ps *ps)
{
  if (!ps_is_at_end (ps))
    {
      ps->current++;
      puts (">> advanced");
    }
  return ps_peek_prev (ps);
}

const struct token *
ps_peek_cur (const struct ps *ps)
{
  return ps->tokens + ps->current;
}

const struct token *
ps_peek_prev (const struct ps *ps)
{
  return ps->tokens + ps->current - 1;
}

const struct token *
ps_peek_next (const struct ps *ps)
{
  return ps->tokens + ps->current + 1;
}
