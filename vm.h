#ifndef VM_H
#define VM_H

#include "obj.h"

typedef enum ByteCode {
  EXIT,

  PUSH_I,
  PUSH_F,
  PUSH_S,
  PUSH_N,
  PUSH_FN,
  LOAD_V,
  SET_V,
  ADD_V,
  POP,
  BUILD_LIST,
  
  ADD,
  SUB,
  MUL,
  DIV,
  EQ,
  NE,
  GT,
  LT,
  GE,
  LE,
  AND,
  OR,
  XOR,

  POS,
  NEG,
  NOT,

  JMP,
  JZ,
  JNZ,

  CALL,
  RET,

  NTH,
  SET_NTH,
} ByteCode;

typedef struct VMCode {
  ByteCode head;
  union {
    long long i;
    double f;
    String *s;
    size_t l;
  };
} VMCode;

VMCode VMCode_new(ByteCode head);
void VMCode_free(VMCode *code);

void VMCode_run(VMCode *code);

long long _int_binaryop(ByteCode op, long long a, long long b);
double _float_binaryop(ByteCode op, double a, double b);
Object Object_binaryop(ByteCode op, Object a, Object b);

long long _int_unaryop(ByteCode op, long long a);
double _float_unaryop(ByteCode op, double a);
Object Object_unaryop(ByteCode op, Object a);

#endif // VM_H
