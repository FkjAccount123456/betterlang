#pragma once

#include "b_lex.h"
#include "b_object.h"
#include "b_seq.h"
#include <stddef.h>


typedef enum OpCode {
  Exit,
  NoOp,
  PushI,
  PushF,
  PushN,
  PushS,
  Pop,
  Binary,
  Unary,
  BuildList,
  BuildDict,
  BuildFunc,
  Index,
  SetIndex,
  Call,
  Ret,
  Jmp,
  Jz,
  Jnz,
  JzNoPop,
  JnzNoPop,
  LoadV,
  SetV,
} OpCode;

typedef struct ByteCode {
  OpCode head;
  union {
    long long i;
    double f;
    String *s;
    size_t l;
    TokenType op;
    struct {
      unsigned int scope, pos;
    } v;
  };
} ByteCode;

ByteCode ByteCode_new(OpCode head);
#define ByteCode_init(head, attr, val)                                         \
  ({                                                                           \
    ByteCode code = ByteCode_new(head);                                        \
    code.attr = val;                                                           \
    code;                                                                      \
  })
void ByteCode_free(ByteCode code);
void ByteCode_print(ByteCode code);

typedef struct Seq(ByteCode) ByteCodeList;
void ByteCodeList_free(ByteCodeList list);

typedef struct Seq(Object) VMStack;
void run(ByteCodeList bytecode, size_t reserve);
