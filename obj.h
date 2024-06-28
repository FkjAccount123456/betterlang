#ifndef OBJ_H
#define OBJ_H

#include "gc.h"

typedef enum ObjectType {
  INT_OBJ,
  FLOAT_OBJ,
  NONE_OBJ,
  STR_OBJ,
  LIST_OBJ,
  DICT_OBJ,
} ObjectType;

typedef struct String String;
typedef struct List List;
typedef struct Dict Dict;

typedef struct ObjectTrait {
  ObjectType tp;
  GC_Dstcor dstcor;
} ObjectTrait;

extern ObjectTrait none_trait, int_trait, float_trait, str_trait, list_trait, dict_trait;

void ObjectTrait_init();

typedef struct Object {
  ObjectTrait *tp;
  union {
    long long i;
    double f;
    String *s;
    List *l;
    Dict *d;
  };
} Object;

Object Object_none();
Object Object_int(long long i);
Object Object_float(double f);
Object Object_String(String *s);
Object Object_List(List *l);
Object Object_Dict(Dict *d);

size_t Object_get_gcval(Object obj);

typedef struct String {
  size_t gc_base;
  char *val;
  size_t size, max;
} String;

String *String_new(char *base);
void String_append(String *str, char ch);
void String_cat(String *str, char *other);
void String_free(String *str);

typedef struct List {
  size_t gc_base;
  Object *items;
  size_t size, max;
} List;

List *List_new();
void List_append(List *list, Object obj);
void List_free(List *list);

typedef struct DictBase {
  Object obj;
  bool has_obj;
  struct DictBase **chs;
} DictBase;

DictBase *_Dict_new();
Object *_Dict_find(DictBase *dict, char *key);
size_t _Dict_insert(DictBase *dict, char *key, Object obj);
void _Dict_free(DictBase *dict);

typedef struct Dict {
  size_t gc_base;
  DictBase *val;
} Dict;

Dict *Dict_new();
Object *Dict_find(Dict *dict, char *key);
void Dict_insert(Dict *dict, char *key, Object obj);
void Dict_free(Dict *dict);

#endif // OBJ_H
