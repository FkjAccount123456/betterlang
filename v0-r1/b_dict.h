#ifndef B_DICT_H
#define B_DICT_H

#include "gc.h"

typedef struct Dict {
  struct Dict **children;
  Object *val;
} Dict;

Dict *Dict_new();
void Dict_free(void *_dict);
void *Dict_pass(void *_base);
void *Dict_copy(void *_dict);

#endif // B_DICT_H