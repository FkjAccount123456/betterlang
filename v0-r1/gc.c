#include "gc.h"
#include "b_dict.h"
#include "b_list.h"
#include "b_string.h"

ObjTrait intTrait, strTrait, listTrait, dictTrait;

void ObjTrait_init() {
  intTrait.need_gc = false;
  intTrait.tp = INT_OBJ;

  strTrait.need_gc = true;
  strTrait.tp = STR_OBJ;
  strTrait.copier = String_copy;
  strTrait.destructor = String_free;
  strTrait.passer = NULL;

  listTrait.need_gc = true;
  listTrait.tp = LIST_OBJ;
  listTrait.copier = List_copy;
  listTrait.destructor = List_free;
  listTrait.passer = List_pass;

  dictTrait.need_gc = true;
  dictTrait.tp = DICT_OBJ;
  dictTrait.copier = Dict_copy;
  dictTrait.destructor = Dict_free;
  dictTrait.passer = Dict_pass;
}

GC_Object *GC_Object_init(void *obj) {
  GC_Object *gc = (GC_Object *)malloc(sizeof(GC_Object));
  if (gc == NULL) {
    printf("Failed to malloc.");
    exit(-1);
  }
  gc->obj = obj;
  gc->rc = 1;
  return gc;
}

Object Object_int(long long intObj) {
  Object obj;
  obj.tp = &intTrait;
  obj.intObj = intObj;
  return obj;
}

Object Object_gc(ObjTrait *trait, GC_Object *gcObj) {
  Object obj;
  obj.tp = trait;
  obj.gcObj = gcObj;
  return obj;
}

void Object_free(Object *obj) {
  if (obj->tp->need_gc && obj->gcObj) {
    obj->gcObj->rc--;
    if (obj->gcObj->rc <= 0) {
      (*obj->tp->destructor)(obj->gcObj->obj);
      free(obj->gcObj);
      obj->gcObj = NULL;
    }
  }
}

Object Object_pass(Object *base) {
  Object obj = *base;
  if (obj.tp->need_gc && obj.gcObj) {
    obj.gcObj->rc++;
    if (obj.tp->passer)
      obj.gcObj->obj = (*obj.tp->passer)(obj.gcObj->obj);
  }
  return obj;
}

Object Object_copy(Object *base) {
  Object obj = *base;
  if (obj.tp->need_gc) {
    obj.gcObj = GC_Object_init((*obj.tp->copier)(obj.gcObj->obj));
  }
  return obj;
}
