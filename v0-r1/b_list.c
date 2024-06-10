#include "b_list.h"

List *List_new() {
  List *list = (List *)malloc(sizeof(List));
  if (list == NULL) {
    printf("Failed to malloc.");
    exit(-1);
  }
  list->max = 8;
  list->size = 0;
  list->val = (Object *)calloc(list->max, sizeof(Object));
  if (list->val == NULL) {
    printf("Failed to malloc.");
    exit(-1);
  }
  return list;
}

void *List_copy(void *base) {
  List *list = (List *)malloc(sizeof(List));
  if (list == NULL) {
    printf("Failed to malloc.");
    exit(-1);
  }
  list->max = ((List *)base)->max;
  list->size = ((List *)base)->size;
  list->val = (Object *)calloc(list->max, sizeof(Object));
  if (list->val == NULL) {
    printf("Failed to malloc.");
    exit(-1);
  }
  for (size_t i = 0; i < list->size; i++) {
    list->val[i] = Object_pass(&list->val[i], 1);
  }
  return list;
}

void List_free(void *list) {
  // printf("free");
  // listObj_print(Object_gc(&list, GC_Object_init(list)));
  // puts("");
  for (size_t i = 0; i < ((List *)list)->size; i++) {
    Object_free_nontop(&((List *)list)->val[i]);
  }
  free(((List *)list)->val);
  free(list);
}

void *List_pass(void *base, size_t rc_offset) {
  for (size_t i = 0; i < ((List *)base)->size; i++) {
    ((List *)base)->val[i] = Object_pass(&((List *)base)->val[i], rc_offset);
  }
  return base;
}

void List_append(List *list, Object obj) {
  if (list->max == list->size) {
    list->max *= 2;
    Object *p = (Object *)realloc(list->val, sizeof(Object) * list->max);
    if (p == NULL) {
      printf("Failed to realloc.");
      exit(-1);
    }
    list->val = p;
  }
  list->val[list->size++] = obj;
}
