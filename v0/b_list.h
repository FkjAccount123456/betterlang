#ifndef B_LIST_H
#define B_LIST_H

#include "b_object.h"
#include "b_gc.h"
#include "b_includes.h"

typedef struct List {
  Object *val;
  size_t size, max;
} List;

List *List_new();
void List_append(List *list, Object obj);
void List_free(void *list);
void *List_copy(void *list);

ObjTrait *List_ObjTrait();

#endif // B_LIST_H
