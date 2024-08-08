#pragma once

#include "gc.h"
#include <stddef.h>

typedef struct Object Object;

typedef struct String String;
typedef struct List List;
typedef struct Dict Dict;
typedef struct Func Func;
typedef struct Method Method;
typedef Object (*Builtin)(size_t, Object *);

typedef gc_ObjNode *(*gc_obj_get_t)(Object *);
typedef void (*obj_print_t)(Object *, bool);

typedef enum ObjType {
  NoneObj,
  IntObj,
  FloatObj,
  StringObj,
  ListObj,
  DictObj,
  FuncObj,
  BuiltinObj,
  MethodObj,
} ObjType;

typedef struct ObjTrait {
  ObjType tp;
  gc_free_t freer;
  gc_obj_get_t getter;
  obj_print_t printer;
} ObjTrait;

extern ObjTrait none_trait, int_trait, float_trait, string_trait, list_trait,
    dict_trait, func_trait, builtin_trait, method_trait;

void init_traits();

typedef struct Object {
  ObjTrait *tp;
  union {
    long long intVal;
    double floatVal;
    String *stringVal;
    List *listVal;
    Dict *dictVal;
    Func *funcVal;
    Builtin builtinVal;
    Method *methodVal;
  };
} Object;

Object Object_none();
Object Object_int(long long);
Object Object_float(double);
Object Object_string(String *);
Object Object_list(List *);
Object Object_dict(Dict *);
Object Object_func(Func *);
Object Object_builtin(Builtin);
Object Object_method(Method *);
void Object_print(Object *, bool);

// 字符串复制构造，不在gc体系之内
// 2024-8-8 算了，还是放进GC里比较妥当
typedef struct String {
  gc_ObjNode *gcobj;
  char *v;
  size_t len, max;
} String;

String *String_new(char *);
String *String_copy(String *);
void String_append(String *, char);
void String_cat(String *, String *);
void String_free(String *);
gc_ObjNode *String_get(Object *);

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

typedef struct Scope {
  gc_ObjNode *gcobj;
  List *varlist;
  struct Scope *parent;
} Scope;

Scope *Scope_new(Scope *);
void Scope_free(Scope *);

typedef struct Func {
  gc_ObjNode *gcobj;
  size_t pc;
  size_t reserve;
  Scope *closure;
} Func;

Func *Func_new(size_t, size_t, Scope *);
gc_ObjNode *Func_get(Object *);
void Func_free(Func *);

typedef struct Method {
  gc_ObjNode *gcobj;
  Object obj, func;
} Method;

Method *Method_new(Object, Object);
gc_ObjNode *Method_get(Object *);
void Method_free(Method *);
