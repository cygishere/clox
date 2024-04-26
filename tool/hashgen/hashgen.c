#include "hash.h"

#include <assert.h>
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct pair
{
  const char *word;
  int hash;
};

/*
 * I know globals are bad.
 * But come on, it's just a small little tool.
 * What could go wrong?
 */
struct pair *pairs;
size_t pairs_cap;
size_t pairs_cur;

int print_word_hash (const char *word);
void assert_conflict (const char *word);
void print_enums (void);

int
main (void)
{
  pairs = NULL;
  assert_conflict ("and");
  assert_conflict ("class");
  assert_conflict ("else");
  assert_conflict ("false");
  assert_conflict ("for");
  assert_conflict ("fun");
  assert_conflict ("if");
  assert_conflict ("nil");
  assert_conflict ("or");
  assert_conflict ("print");
  assert_conflict ("return");
  assert_conflict ("super");
  assert_conflict ("this");
  assert_conflict ("true");
  assert_conflict ("var");
  assert_conflict ("while");

  puts ("");

  print_enums ();

  free (pairs);
}

int
print_word_hash (const char *word)
{
  int hash = lox_fnv (word, strlen (word));
  printf ("\"%s\": %d\n", word, hash);
  return hash;
}

void
assert_conflict (const char *word)
{
  if (!pairs)
    {
      pairs = malloc (sizeof *pairs * 10);
      assert (pairs && "Failed to malloc");
      pairs_cap = 10;
      pairs_cur = 0;
    }

  int hash = print_word_hash (word);
  for (size_t i = 0; i < pairs_cur; ++i)
    {
      if (hash == pairs[i].hash)
        {
          printf ("Hash conflict detected: %s and %s\n", word, pairs[i].word);
          exit (1);
        }
    }

  if (pairs_cur + 1 > pairs_cap - 1)
    {
      pairs_cap *= 2;
      pairs = realloc (pairs, sizeof *pairs * pairs_cap);
    }

  pairs[pairs_cur].hash = hash;
  pairs[pairs_cur].word = word;
  pairs_cur++;
}

void
print_enums (void)
{
  for (size_t i = 0; i < pairs_cur; ++i)
    {
      printf ("LT_");
      /* printf ("%s\n", pairs[i].word); */
      for (const char *j = pairs[i].word; *j; ++j)
        {
          printf ("%c", toupper (*j));
        }
      printf (" = %d,\n", pairs[i].hash);
    }
}
