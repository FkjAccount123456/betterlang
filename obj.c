#include "obj.h"

Object Object_Null() {
  Object obj;
  obj.tp = NULL_OBJ;
  return obj;
}

Object Object_int(long long int_obj) {
  Object obj;
  obj.tp = INT_OBJ;
  obj.int_obj = int_obj;
  return obj;
}

Object Object_float(double float_obj) {
  Object obj;
  obj.tp = FLOAT_OBJ;
  obj.float_obj = float_obj;
  return obj;
}

Object Object_gc(GCPtr *gc_obj) {
  Object obj;
  obj.tp = gc_obj->ptr->trait->tp;
  obj.gc_obj = gc_obj;
  return obj;
}

void Object_free(Object obj) {
  if (obj->tp > NULL_OBJ) {
    
  }
}
