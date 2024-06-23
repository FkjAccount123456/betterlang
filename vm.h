#ifndef VM_H
#define VM_H

#include "b_obj.h"

typedef enum ByteCode {
  EXIT,

  PUSH_I,
  PUSH_F,
  PUSH_S,
  POP,
  
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
} ByteCode;

typedef struct VMCode {
  ByteCode head;
  union {
    long long i;
    double f;
    GC_Node *s;
    size_t l;
  };
} VMCode;

VMCode VMCode_new(ByteCode head);

void VMCode_run(VMCode *code, GC_Root *gc);

long long _int_binaryop(ByteCode op, long long a, long long b);
double _float_binaryop(ByteCode op, double b, double b);
Object Object_binaryop(ByteCode op, Object a, Object b);

long long _int_unaryop(ByteCode op, long long a);
double _float_unaryop(ByteCode op, double a);
Object Object_unaryop(ByteCode op, Object a);

typedef struct VMFrame {
  GC_Node *varlist;
  GC_Node *parent;
} VMFrame;

#endif // VM_H
