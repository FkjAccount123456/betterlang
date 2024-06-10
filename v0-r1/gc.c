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
  printf("%s", ((String *)obj->gcObj->obj)->val);
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

size_t strObj_len(Object *obj) { return ((String *)obj->gcObj->obj)->size; }

size_t listObj_len(Object *obj) { return ((List *)obj->gcObj->obj)->size; }

size_t dictObj_len(Object *obj) { return Dict_size(obj->gcObj->obj); }

size_t err_len(Object *obj) {
  printf("Failed to get length of object.");
  exit(-1);
}

long long _binary_operate(TokenType op, long long left, long long right) {
  switch (op) {
  case ADD_TOKEN:
    return left + right;
  case SUB_TOKEN:
    return left - right;
  case MUL_TOKEN:
    return left * right;
  case DIV_TOKEN:
    return left / right;
  case MOD_TOKEN:
    return left % right;
  case EQ_TOKEN:
    return left == right;
  case NE_TOKEN:
    return left != right;
  case GT_TOKEN:
    return left > right;
  case GE_TOKEN:
    return left >= right;
  case LT_TOKEN:
    return left < right;
  case LE_TOKEN:
    return left <= right;
  default:
    printf("Unknown binary operator.");
    exit(-1);
  }
}

long long _unary_operate(TokenType op, long long val) {
  switch (op) {
  case ADD_TOKEN:
    return +val;
  case SUB_TOKEN:
    return -val;
  default:
    printf("Unknown unary operator.");
    exit(-1);
  }
}

Object Object_binary(TokenType op, Object *left, Object *right) {
  if (op == AND_TOKEN)
    return Object_int((*left->tp->toBooler)(left) && (*right->tp->toBooler)(right));
  if (op == OR_TOKEN)
    return Object_int((*left->tp->toBooler)(left) || (*right->tp->toBooler)(right));
  if (op == XOR_TOKEN)
    return Object_int((*left->tp->toBooler)(left) ^ (*right->tp->toBooler)(right));
  if (op == EQ_TOKEN && left->tp->tp != right->tp->tp)
    return Object_int(0);
  if (op == NE_TOKEN && left->tp->tp != right->tp->tp)
    return Object_int(1);
  if (left->tp->tp == INT_OBJ && right->tp->tp == INT_OBJ) {
    long long res = _binary_operate(op, left->intObj, right->intObj);
    return Object_int(res);
  } else if (left->tp->tp == STR_OBJ && right->tp->tp == STR_OBJ) {
    String *l = left->gcObj->obj;
    String *r = right->gcObj->obj;
    switch (op) {
    case EQ_TOKEN:
      return Object_int(strcmp(l->val, r->val) == 0);
    case NE_TOKEN:
      return Object_int(strcmp(l->val, r->val) != 0);
    case GT_TOKEN:
      return Object_int(strcmp(l->val, r->val) > 0);
    case GE_TOKEN:
      return Object_int(strcmp(l->val, r->val) >= 0);
    case LT_TOKEN:
      return Object_int(strcmp(l->val, r->val) < 0);
    case LE_TOKEN:
      return Object_int(strcmp(l->val, r->val) <= 0);
    default:;
    }
  }
  printf("Unsupported binary operation.");
  exit(-1);
}

Object Object_unary(TokenType op, Object *right) {
  if (op == NOT_TOKEN)
    return Object_int(!(*right->tp->toBooler)(right));
  if (right->tp->tp == INT_OBJ) {
    long long res = _unary_operate(op, right->intObj);
    return Object_int(res);
  } else {
    printf("Unsupported unary operation.");
    exit(-1);
  }
}

void ObjTrait_init() {
  intTrait.need_gc = false;
  intTrait.tp = INT_OBJ;
  intTrait.toBooler = intObj_toBool;
  intTrait.printer = intObj_print;
  intTrait.lenFn = err_len;

  strTrait.need_gc = true;
  strTrait.tp = STR_OBJ;
  strTrait.copier = String_copy;
  strTrait.destructor = String_free;
  strTrait.passer = NULL;
  strTrait.toBooler = strObj_toBool;
  strTrait.printer = strObj_print;
  strTrait.lenFn = strObj_len;

  listTrait.need_gc = true;
  listTrait.tp = LIST_OBJ;
  listTrait.copier = List_copy;
  listTrait.destructor = List_free;
  listTrait.passer = List_pass;
  listTrait.toBooler = listObj_toBool;
  listTrait.printer = listObj_print;
  listTrait.lenFn = listObj_len;

  dictTrait.need_gc = true;
  dictTrait.tp = DICT_OBJ;
  dictTrait.copier = Dict_copy;
  dictTrait.destructor = Dict_free;
  dictTrait.passer = Dict_pass;
  dictTrait.toBooler = dictObj_toBool;
  dictTrait.printer = err_print;
  dictTrait.lenFn = dictObj_len;

  funcTrait.need_gc = true;
  funcTrait.tp = FUNC_OBJ;
  funcTrait.copier = NULL;
  funcTrait.destructor = Func_free;
  funcTrait.passer = Func_pass;
  funcTrait.toBooler = funcObj_toBool;
  funcTrait.printer = err_print;
  funcTrait.lenFn = err_len;

  builtinfuncTrait.need_gc = false;
  builtinfuncTrait.tp = BUILTINFUNC_OBJ;
  builtinfuncTrait.toBooler = funcObj_toBool;
  builtinfuncTrait.printer = err_print;
  builtinfuncTrait.lenFn = err_len;
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
    Object_pass(obj, -1);
    // printf("InitFree tp = %d rc = %d: ", obj->tp->tp, obj->gcObj->rc);
    // (*obj->tp->printer)(obj);
    // puts("");
    if (obj->gcObj->rc <= 0) {
      // printf("Free tp = %d: ", obj->tp->tp);
      // (*obj->tp->printer)(obj);
      // puts("");
      (*obj->tp->destructor)(obj->gcObj->obj);
      free(obj->gcObj);
      obj->gcObj = NULL;
    }
  }
}

void Object_free_nontop(Object *obj) {
  if (obj->tp->need_gc && obj->gcObj) {
    if (obj->gcObj->rc <= 0) {
      (*obj->tp->destructor)(obj->gcObj->obj);
      free(obj->gcObj);
      obj->gcObj = NULL;
    }
  }
}

Object Object_pass(Object *base, size_t rc_offset) {
  Object obj = *base;
  if (obj.tp->need_gc && obj.gcObj) {
    obj.gcObj->rc += rc_offset;
    if (obj.tp->passer)
      obj.gcObj->obj = (*obj.tp->passer)(obj.gcObj->obj, rc_offset);
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
