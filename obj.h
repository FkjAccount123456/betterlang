#ifndef OBJ_H
#define OBJ_H

#include "gc.h"

#define ObjTypePtr(tp, obj) ((tp *)obj.gc_obj->ptr->ptr)

extern GCTrait str_trait, list_trait, dict_trait, vmframe_trait, func_trait;

typedef struct Object Object;

// 内置函数所传参数在函数内回收
typedef Object (*Builtin)(size_t, Object *);

typedef struct Object {
  ObjType tp;
  union {
    long long int_obj;
    double float_obj;
    GCPtr *gc_obj;
    Builtin builtin_obj;
  };
} Object;

Object Object_Null();
Object Object_int(long long int_obj);
Object Object_float(double float_obj);
Object Object_builtin(Builtin builtin);
Object Object_gc(GCPtr *gc_obj);
void Object_free(Object obj);
Object Object_pass(Object base);
Object Object_copy(Object base);
void Object_connect(GCPtr *base, Object obj);
void Object_disconnect(GCPtr *base, Object obj);

typedef struct String {
  char *val;
  size_t size, max;
} String;

String *_String_new(char *base);
GCPtr *String_new(char *base);
GCPtr *String_copy(String *base);
void String_append(String *str, char c);
void String_free(String *str);

typedef struct List {
  Object *items;
  size_t size, max;
} List;

List *_List_new();
GCPtr *List_new();
GCPtr *List_copy(List *list);
void List_append(Object obj, Object item);
void List_free(List *list);

typedef struct Dict {
  Object val;
  bool has_val;
  struct Dict **children;
} Dict;

GCPtr *Dict_new();
GCPtr *Dict_copy(Dict *dict);
Object *Dict_find(Dict *dict, char *key);
void Dict_insert(Object dict, char *key, Object val);
void Dict_free(Dict *dict);

// 相当于Scope
typedef struct VMFrame {
  List *varlist;
  struct VMFrame *parent;
} VMFrame;

Object VMFrame_new(VMFrame *parent);
void VMFrame_free(VMFrame *frame);
bool Object_to_bool(Object o);
char Object_cmp(Object a, Object b);

typedef struct Func {
  size_t pc;
  Object frame;
} Func;

GCPtr *Func_new(size_t pc, Object frame);
void Func_free(Func *f);

#endif // OBJ_H
