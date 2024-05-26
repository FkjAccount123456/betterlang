#ifndef B_LIST_H
#define B_LIST_H

#include "gc.h"

typedef struct List {
  Object* val;
  size_t size, max;
} List;

List* List_new();
void* List_copy(void* base);
void List_free(void* list);
void* List_pass(void* list);
void List_append(List* list, Object obj);

#endif // B_LIST_H

