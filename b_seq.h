#pragma once

#include "b_stdlib.h"

// 经典seq.h
// tree-sitter的array.h码风不错，借用一点

#define Seq(T)                                                                 \
  {                                                                            \
    T *v;                                                                      \
    size_t len, max, Tsize;                                                    \
  }

#define SeqNew(T)                                                              \
  ({                                                                           \
    T name;                                                                    \
    name.len = 0, name.max = 8, name.Tsize = sizeof(T),                        \
    name.v = b_malloc(name.Tsize * 8);                                         \
    name;                                                                      \
  })

#define SeqAppend(name, val)                                                   \
  do {                                                                         \
    if ((name).len == (name).max) {                                            \
      (name).max *= 2;                                                         \
      (name).v = b_realloc((name).v, (name).Tsize * (name).max);               \
    }                                                                          \
    (name).v[(name).len++] = val;                                              \
  } while (0)

#define SeqPop(name) ((name).v[--(name).len])

#define SeqFree(name) (b_free((name).v))
