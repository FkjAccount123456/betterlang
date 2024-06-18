#ifndef B_OBJ_H
#define B_OBJ_H

#include "b_gc.h"

typedef enum ObjType {
  INT_OBJ,
  LIST_OBJ,
  DICT_OBJ,
  STR_OBJ,
} ObjType;

typedef struct Object {
  ObjType tp;
  union {
    long long int_obj;
    GC_Node *gc_obj;
  };
} Object;

Object Object_int(long long int_obj);
Object Object_gc(ObjType tp, GC_Node *gc_obj, GC_Node *parent);
Object Object_pass(Object obj);
void Object_free(Object obj);

typedef struct List {
  size_t size, max;
  GC_Node *items;
} List;

GC_Node *List_new(GC_Root *gc);
void List_append(List *list, Object obj, GC_Root *gc);

typedef struct String {
  size_t size, max;
  GC_Node *val;
} String;

GC_Node *String_new(char *base, GC_Root *gc);
void String_append(String *base, char ch, GC_Root *gc);
void String_cat(String *base, String *other, GC_Root *gc);

typedef struct Dict {
  Object *obj;
  GC_Node *children;
} Dict;

GC_Node *Dict_new(GC_Root *gc);
void Dict_insert(Dict *dict, char *key, Object obj, GC_Root *gc);
Object *Dict_find(Dict *dict, char *key);

#endif // B_OBJ_H
