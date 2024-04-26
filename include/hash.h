#ifndef LOX_HASH_H
#define LOX_HASH_H

#include <stddef.h>

/*
 * I randomly chose a good looking hash function.
 * As long as there are not collisions, everything will
 * be just fine.
 */

/*
 * This hash function is copied from https://stackoverflow.com/a/626599
 */
unsigned
lox_fnv (const char *key, size_t len)
{
  const unsigned char *p = (const unsigned char *)key;
  unsigned h = 0x811c9dc5;
  size_t i;

  for (i = 0; i < len; ++i)
    {
      h = (h ^ p[i]) * 0x01000193;
    }

  return h;
}

#endif /* LOX_HASH_H */
