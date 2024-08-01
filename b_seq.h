#pragma once

#include "b_stdlib.h"

// 经典seq.h
// tree-sitter的array.h码风不错，借用一点

#define Seq(T)                                                                 \
  {                                                                            \
    T *v;                                                                      \
    size_t len, max;                                                           \
  }

#define SeqNew(T)                                                              \
  ({                                                                           \
    T name;                                                                    \
    name.len = 0, name.max = 8,                                                \
    name.v = b_malloc((void *)(name.v + 8) - (void *)name.v);                  \
    name;                                                                      \
  })

#define SeqAppend(name, val)                                                   \
  do {                                                                         \
    if (name.len == name.max) {                                                \
      name.max *= 2;                                                           \
      name.v =                                                                 \
          b_realloc(name.v, (void *)(name.v + name.max) - (void *)name.v);     \
    }                                                                          \
    name.v[name.len++] = val;                                                  \
  } while (0)

#define SeqPop(name) (name.v[--name.len])

#define SeqFree(name) (b_free(name.v))
