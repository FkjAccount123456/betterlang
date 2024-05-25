#ifndef B_DICT_H
#define B_DICT_H

#include "b_includes.h"
#include "b_gc.h"

typedef struct Dict {
  struct Dict **children;
  Object *obj;
} Dict;

Dict *Dict_new();
Object *Dict_find(Dict *dict, char *key);
Object *Dict_insert(Dict *dict, char *key, Object val);
void *Dict_copy(void *base);
void Dict_free(void *dict);

ObjTrait *Dict_ObjTrait();

#endif // B_DICT_H
