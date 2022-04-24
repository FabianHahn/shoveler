#ifndef SHOVELER_HASH_H
#define SHOVELER_HASH_H

#include <glib.h>

static inline guint shovelerHashCombine(guint a, guint b) {
  // see
  // https://stackoverflow.com/questions/5889238/why-is-xor-the-default-way-to-combine-hashes/27952689#27952689
  return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
}

#endif
