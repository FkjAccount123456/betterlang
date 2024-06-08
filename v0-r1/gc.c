#include "gc.h"
#include "b_dict.h"
#include "b_func.h"
#include "b_list.h"
#include "b_string.h"


ObjTrait intTrait, strTrait, listTrait, dictTrait, funcTrait, builtinfuncTrait;

bool intObj_toBool(Object *obj) { return obj->intObj; }

bool strObj_toBool(Object *obj) { return ((String *)obj->gcObj->obj)->size; }

bool listObj_toBool(Object *obj) { return ((List *)obj->gcObj->obj)->size; }

bool dictObj_toBool(Object *obj) { return Dict_size(obj->gcObj->obj); }

bool funcObj_toBool(Object *obj) { return true; }

void intObj_print(Object *obj) { printf("%lld", obj->intObj); }

void strObj_print(Object *obj) {
  printf("\"%s\"", ((String *)obj->gcObj->obj)->val);
}

void listObj_print(Object *obj) {
  List *list = obj->gcObj->obj;
  printf("[");
  if (list->size) {
    (*list->val[0].tp->printer)(&list->val[0]);
    for (size_t i = 1; i < list->size; i++) {
      printf(", ");
      (*list->val[i].tp->printer)(&list->val[i]);
    }
  }
  printf("]");
}

void err_print(Object *obj) {
  printf("Failed to print object.");
  exit(-1);
}

void ObjTrait_init() {
  intTrait.need_gc = false;
  intTrait.tp = INT_OBJ;
  intTrait.toBooler = intObj_toBool;
  intTrait.printer = intObj_print;

  strTrait.need_gc = true;
  strTrait.tp = STR_OBJ;
  strTrait.copier = String_copy;
  strTrait.destructor = String_free;
  strTrait.passer = NULL;
  strTrait.toBooler = strObj_toBool;
  strTrait.printer = strObj_print;

  listTrait.need_gc = true;
  listTrait.tp = LIST_OBJ;
  listTrait.copier = List_copy;
  listTrait.destructor = List_free;
  listTrait.passer = List_pass;
  listTrait.toBooler = listObj_toBool;
  listTrait.printer = listObj_print;

  dictTrait.need_gc = true;
  dictTrait.tp = DICT_OBJ;
  dictTrait.copier = Dict_copy;
  dictTrait.destructor = Dict_free;
  dictTrait.passer = Dict_pass;
  dictTrait.toBooler = dictObj_toBool;
  dictTrait.printer = err_print;

  funcTrait.need_gc = true;
  funcTrait.tp = FUNC_OBJ;
  funcTrait.copier = Func_pass;
  funcTrait.destructor = Func_free;
  funcTrait.passer = Func_pass;
  funcTrait.toBooler = funcObj_toBool;
  funcTrait.printer = err_print;

  builtinfuncTrait.need_gc = false;
  builtinfuncTrait.tp = BUILTINFUNC_OBJ;
  builtinfuncTrait.toBooler = funcObj_toBool;
  builtinfuncTrait.printer = err_print;
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

Object Object_builtinfunc(BuiltinFunc func) {
  Object obj;
  obj.tp = &builtinfuncTrait;
  obj.builtinfuncObj = func;
  return obj;
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
