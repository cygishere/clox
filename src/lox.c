#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ast_print.h"
#include "lox.h"
#include "parser.h"
#include "scanner.h"

#define LOX_BUF_SIZE 256

bool lox_had_error = false;

void lox_report (int line, const char *where, const char *message, va_list ap);

int
main (int argc, char **argv)
{
  if (argc > 2)
    {
      fputs ("Usage: lox [script]\n", stderr);
      return LOX_EX_USAGE;
    }
  else if (argc == 2)
    {
      return lox_run_file (argv[1]);
    }
  else
    {
      lox_run_prompt ();
    }
}

int
lox_run_file (const char *filename)
{
  int rc = LOX_EX_OK;
  FILE *file = fopen (filename, "r");
  if (!file)
    {
      fprintf (stderr, "Failed to open file %s\n", filename);
      return rc;
    }

  fseek (file, 0, SEEK_END);
  long fsize = ftell (file);
  rewind (file);

  char *buf = malloc (fsize);
  if (!buf)
    {
      fprintf (stderr, "Failed to allocate memory\n");
      goto fail_malloc;
    }

  size_t bytes_read = fread (buf, 1, fsize, file);
  if (bytes_read != fsize)
    {
      fprintf (stderr, "Failed to read file %s\n", filename);
      goto fail_fread;
    }

  buf[fsize - 1] = 0;

  lox_run (buf);

  if (lox_had_error)
    {
      rc = LOX_EX_DATAERR;
    };

fail_fread:
  free (buf);
fail_malloc:
  fclose (file);
  return rc;
}

void
lox_run_prompt (void)
{
  char buf[LOX_BUF_SIZE];
  while (1)
    {
      printf ("> ");
      if (!fgets (buf, LOX_BUF_SIZE, stdin))
        {
          break;
        }
      lox_run (buf);
    }
}

void
lox_run (const char *source)
{
  struct sc sc = sc_get_scanner (source);
  sc_scan_tokens (&sc);

  struct ps ps = ps_get_parser (sc.tokens);
  union expr *e = ps_expression (&ps);

  ast_print (e, stdout);
  puts ("");

  sc_free_tokens (&sc);
}

void
lox_error (int line, const char *message, ...)
{
  va_list ap;
  va_start (ap, message);
  lox_report (line, "", message, ap);
  va_end (ap);
}

void
lox_report (int line, const char *where, const char *message, va_list ap)
{
  fprintf (stderr, "[line %d] Error%s: ", line, where);
  vfprintf (stderr, message, ap);
  lox_had_error = true;
}
