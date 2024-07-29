#include <stddef.h>
#include "gc.h"

typedef struct String String;
typedef struct List List;

typedef struct Object Object;

typedef gc_ObjNode *(*gc_obj_get_t)(Object *);

typedef enum ObjType {
  IntObj,
  FloatObj,
  StringObj,
  ListObj,
} ObjType;

typedef struct ObjTrait {
  ObjType tp;
  gc_free_t freer;
  gc_obj_get_t getter;
} ObjTrait;

typedef struct Object {
  ObjTrait *tp;
  union {
    long long intVal;
    double floatVal;
    String *stringVal;
    List *listVal;
  };
} Object;

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

// 形似Python的哈希表实现，可惜我没有那样神的哈希函数
typedef struct DictEntry {
  char *key;
  Object val;
} DictEntry;

typedef struct Dict {
  gc_ObjNode *gcobj;
  size_t len, max;
  size_t *indices;
  DictEntry *entries;
} Dict;

Dict *Dict_new();
void Dict_insert(Dict *, char *, Object);
Object *Dict_find(Dict *, char *);
void Dict_free(Dict *);
gc_ObjNode *Dict_get(Object *);
