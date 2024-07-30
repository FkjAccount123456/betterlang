#pragma once

#include <stddef.h>
#include "gc.h"

typedef struct String String;
typedef struct List List;
typedef struct Dict Dict;

typedef struct Object Object;

typedef gc_ObjNode *(*gc_obj_get_t)(Object *);
typedef void (*obj_print_t)(Object *, bool);

typedef enum ObjType {
  NoneObj,
  IntObj,
  FloatObj,
  StringObj,
  ListObj,
  DictObj,
} ObjType;

typedef struct ObjTrait {
  ObjType tp;
  gc_free_t freer;
  gc_obj_get_t getter;
  obj_print_t printer;
} ObjTrait;

extern ObjTrait none_trait, int_trait, float_trait,
                string_trait, list_trait, dict_trait;

void init_traits();

typedef struct Object {
  ObjTrait *tp;
  union {
    long long intVal;
    double floatVal;
    String *stringVal;
    List *listVal;
    Dict *dictVal;
  };
} Object;

Object Object_none();
Object Object_int(long long);
Object Object_float(double);
Object Object_string(String *);
Object Object_list(List *);
Object Object_dict(Dict *);
void Object_print(Object *, bool);

// 字符串复制构造，不在gc体系之内
typedef struct String {
  char *v;
  size_t len, max;
} String;

String *String_new(char *);
String *String_copy(String *);
void String_append(String *, char);
void String_cat(String *, String *);
void String_free(String *);

char *str_copy(char *);
size_t str_hash(char *);

typedef struct List {
  // 声明即加入gc
  gc_ObjNode *gcobj;
  Object *v;
  size_t len, max;
} List;

List *List_new();
void List_append(List *, Object);
void List_free(List *);
gc_ObjNode *List_get(Object *);

typedef struct DictEntry {
  struct DictEntry *next;
  char *key;
  Object val;
} DictEntry;

void DictEntry_append(DictEntry **, char *, Object);

// 拉链法，最慢、最占空间但是最简单的哈希冲突解决办法
// 算是以时间换空间了，这个的常数超大
typedef struct Dict {
  gc_ObjNode *gcobj;
  DictEntry **entries;
} Dict;

#define DICT_ENTRIES 1024

Dict *Dict_new();
void Dict_insert(Dict *, char *, Object);
Object *Dict_find(Dict *, char *);
void Dict_set(Dict *, char *, Object);
void Dict_free(Dict *);
gc_ObjNode *Dict_get(Object *);
