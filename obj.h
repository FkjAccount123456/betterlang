#ifndef OBJ_H
#define OBJ_H

#include "gc.h"

typedef struct Object {
  ObjType tp;
  union {
    long long int_obj;
    double float_obj;
    GCPtr *gc_obj;
  };
};

Object Object_Null();
Object Object_int(long long int_obj);
Object Object_float(double float_obj);
Object Object_gc(GCPtr *gc_obj);
void Object_free(Object obj);
Object Object_pass(Object base);

#endif // OBJ_H
