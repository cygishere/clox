#ifndef LOX_SCANNER_H
#define LOX_SCANNER_H

#include "token.h"

#include <stddef.h>

struct sc
{
  const char *source;
  const size_t source_len;

  struct token *tokens;
  size_t num_tokens;
  size_t cap_tokens;

  size_t start;
  size_t current;
  int line;
};

struct sc sc_get_scanner (const char *source);
struct token *sc_scan_tokens (struct sc *sc);

void sc_free_tokens (struct sc *sc);

#endif /* LOX_SCANNER_H */
