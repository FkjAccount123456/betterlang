#include "b_gc.h"

GC_Object *GC_Object_init(void *ptr, ObjTrait *type) {
  puts("GC_Object_init");
  GC_Object *obj = (GC_Object *)b_alloc(sizeof(GC_Object));
  obj->ptr = ptr;
  obj->rc = 1;
  obj->type = type;
  return obj;
}

void GC_Object_decrc(GC_Object *obj) {
  puts("GC_Object_decrc");
  obj->rc--;
  if (obj->rc <= 0) {
    (*obj->type->destructor)(obj->ptr);
    b_free(obj);
  }
}

GC_Object *GC_Object_copy(GC_Object *obj) {
  GC_Object *cpy = (GC_Object *)b_alloc(sizeof(GC_Object));
  cpy->ptr = (*cpy->type->copier)(cpy->ptr);
  cpy->rc = 1;
  return cpy;
}

GC_Object *GC_Object_pass(GC_Object *obj) {
  obj->rc++;
  return obj;
}

Object Object_int(long long intObj) {
  Object obj;
  obj.type = OBJ_INT;
  obj.intObj = intObj;
  return obj;
}

Object Object_gc(GC_Object *gcObj) {
  Object obj;
  obj.type = OBJ_GC;
  obj.gcObj = gcObj;
  return obj;
}

void Object_free(Object *obj) {
  puts("Object_free");
  if (obj->type == OBJ_GC)
    GC_Object_decrc(obj->gcObj);
}

Object Object_copy(Object *base) {
  Object obj = *base;
  if (obj.type == OBJ_GC)
    obj.gcObj = GC_Object_copy(obj.gcObj);
  return obj;
}
