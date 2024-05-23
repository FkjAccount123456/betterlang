#ifndef B_GC_H
#define B_GC_H

#include "b_alloc.h"
#include "b_object.h"
#include "b_includes.h"

typedef struct GC_Object {
  ObjTrait *type;
  size_t rc;
  void *ptr;
} GC_Object;

GC_Object *GC_Object_init(void *ptr, ObjTrait *type);
void GC_Object_decrc(GC_Object *obj);
GC_Object *GC_Object_copy(GC_Object *obj);
GC_Object *GC_Object_pass(GC_Object *obj);

typedef enum ObjType {
  OBJ_INT,
  OBJ_GC,
} ObjType;

typedef struct Object {
  ObjType type;
  union {
    long long intObj;
    GC_Object *gcObj;
  };
} Object;

Object Object_int(long long intObj);
Object Object_gc(GC_Object *gcObj);
void Object_free(Object *obj);

#endif // B_GC_H
