#include "b_list.h"
#include "b_alloc.h"

List *List_new() {
  List *list = (List *)b_alloc(sizeof(List));
  list->max = 8;
  list->size = 0;
  list->val = (Object *)b_alloc(sizeof(Object) * list->max);
  return list;
}

void List_append(List *list, Object obj) {
  if (list->max == list->size) {
    list->max *= 2;
    list->val = (Object *)b_realloc(list->val, sizeof(Object) * list->max);
  }
  list->val[list->size++] = obj;
}

void List_free(void *list) {
  for (size_t i = 0; i < ((List *)list)->size; i++) {
    if (((List *)list)->val[i].type == OBJ_GC)
      GC_Object_decrc(((List *)list)->val[i].gcObj);
  }
  b_free(((List *)list)->val);
  b_free(list);
}

void *List_copy(void *list) {
  List *cpy = (List *)b_alloc(sizeof(List));
  cpy->max = ((List *)list)->max;
  cpy->size = ((List *)list)->size;
  cpy->val = (Object *)b_alloc(sizeof(Object) * cpy->max);
  for (size_t i = 0; i < ((List *)list)->size; i++) {
    cpy->val[i].type = ((List *)list)->val[i].type;
    if (((List *)list)->val[i].type == OBJ_GC)
      cpy->val[i].gcObj = GC_Object_copy(((List *)list)->val[i].gcObj->ptr);
    else
      cpy->val[i].intObj = ((List *)list)->val[i].intObj;
  }
  return cpy;
}

ObjTrait *List_ObjTrait() {
  ObjTrait *objtrait = (ObjTrait *)b_alloc(sizeof(ObjTrait));
  objtrait->copier = &List_copy;
  objtrait->destructor = &List_free;
  return objtrait;
}
