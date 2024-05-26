#ifndef GC_H
#define GC_H

#include "includes.h"

typedef void (*Destructor)(void*);
typedef void* (*Copier)(void*);
typedef void* (*Passer)(void*);

typedef enum ObjType {
  INT_OBJ,
  STR_OBJ,
  LIST_OBJ,
} ObjType;

typedef struct ObjTrait {
  bool need_gc;
  ObjType tp;
  Destructor destructor;
  Copier copier;
  Passer passer;
} ObjTrait;

extern ObjTrait intTrait, strTrait, listTrait;
void ObjTrait_init();

typedef struct GC_Object {
  size_t rc;
  void* obj;
} GC_Object;

GC_Object* GC_Object_init(void* obj);

typedef struct Object {
  ObjTrait* tp;
  union {
    GC_Object* gcObj;
    long long intObj;
  };
} Object;

Object Object_int(long long intObj);
Object Object_gc(ObjTrait* trait, GC_Object* gcObj);
void Object_free(Object* obj);
Object Object_pass(Object* base);
Object Object_copy(Object* base);

#endif  // GC_H
