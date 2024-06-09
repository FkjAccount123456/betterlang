#ifndef B_DICT_H
#define B_DICT_H

#include "gc.h"

typedef struct Dict {
  struct Dict **children;
  Object *val;
} Dict;

Dict *Dict_new();
void Dict_free(void *_dict);
void *Dict_pass(void *_base, size_t rc_offset);
void *Dict_copy(void *_dict);
Object *Dict_find(Dict *dict, char *name);
void Dict_insert(Dict *dict, char *name, Object obj);
size_t Dict_size(Dict *dict);

#endif // B_DICT_H
