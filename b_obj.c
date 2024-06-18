#include "b_obj.h"
#include "b_gc.h"

Object Object_int(long long int_obj) {
  Object obj;
  obj.tp = INT_OBJ;
  obj.int_obj = int_obj;
  return obj;
}

Object Object_gc(ObjType tp, GC_Node *gc_obj, GC_Node *parent) {
  Object obj;
  obj.tp = tp;
  obj.gc_obj = gc_obj;
  GC_Node_connect(parent, gc_obj);
  return obj;
}

void Object_free(Object obj) {
  if (obj.tp != INT_OBJ) {
    // printf("Object_free %llx\n", obj.gc_obj);
    GC_Node_dennect(obj.gc_obj->parent, obj.gc_obj);
  }
}

Object Object_pass(Object obj) {
  if (obj.tp == LIST_OBJ || obj.tp == STR_OBJ) {
    obj.gc_obj->cnt++;
  }
  return obj;
}

GC_Node *List_new(GC_Root *gc) {
  GC_Node *n = GC_malloc(sizeof(List), gc);
  List *list = n->ptr;
  list->max = 8;
  list->size = 0;
  list->items = GC_malloc(sizeof(Object) * list->max, gc);
  return n;
}

void List_append(List *list, Object obj, GC_Root *gc) {
  if (list->max == list->size) {
    list->max *= 2;
    list->items = GC_realloc(list->items, sizeof(Object) * list->max, gc);
  }
  ((Object *)list->items->ptr)[list->size++] = obj;
}

GC_Node *String_new(char *base, GC_Root *gc) {
  GC_Node *n = GC_malloc(sizeof(String), gc);
  String *str = n->ptr;
  str->max = str->size = strlen(base);
  if (str->max == 0)
    str->max = 8;
  str->val = GC_malloc(sizeof(char) * (str->max + 1), gc);
  strcpy(str->val->ptr, base);
  return n;
}

void String_append(String *base, char ch, GC_Root *gc) {
  if (base->max == base->size) {
    base->max *= 2;
    base->val = GC_realloc(base->val, sizeof(char) * (base->max + 1), gc);
  }
  ((char *)base->val->ptr)[base->size++] = ch;
  ((char *)base->val->ptr)[base->size] = 0;
}

void String_cat(String *base, String *other, GC_Root *gc) {
  size_t new_size = base->size + other->size;
  if (new_size > base->max) {
    while (base->max < new_size) {
      base->max *= 2;
    }
    base->val = GC_realloc(base->val, sizeof(char) * (base->max + 1), gc);
  }
  strcpy(base->val->ptr + base->size, other->val->ptr);
  base->size = new_size;
}

GC_Node *Dict_new(GC_Root *gc) {
  GC_Node *n = GC_malloc(sizeof(Dict), gc);
  Dict *dict = n->ptr;
  dict->obj = NULL;
  dict->children = GC_malloc(sizeof(GC_Node *) * 128, gc);
  for (size_t i = 0; i < 128; i++)
    ((Dict **)dict->children->ptr)[i] = NULL;
  return n;
}

void Dict_insert(Dict *dict, char *key, Object obj, GC_Root *gc) {
  if (!*key) {
    if (dict->obj)
      Object_free(*dict->obj);
    *dict->obj = obj;
  }
  if (((GC_Node **)dict->children->ptr)[*key] == NULL) {
    ((GC_Node **)dict->children->ptr)[*key] = Dict_new(gc);
  }
  Dict_insert(((Dict **)dict->children->ptr)[*key], key + 1, obj, gc);
}

Object *Dict_find(Dict *dict, char *key) {
  if (!*key) {
    return dict->obj;
  }
  if (((GC_Node **)dict->children->ptr)[*key] == NULL) {
    return NULL;
  }
  return Dict_find(((GC_Node **)dict->children->ptr)[*key]->ptr, key + 1);
}
