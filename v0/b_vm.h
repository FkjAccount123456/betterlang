#ifndef B_VM_H
#define B_VM_H

#include "b_includes.h"
#include "b_list.h"

typedef enum ByteCode {
  EXIT,
  PUSH, POP,
  ADD, SUB, MUL, DIV, MOD,
  EQ, NE, GT, LT, GE, LE,
  LSH, RSH,
  AND, OR, BAND, BOR, XOR,
  LOAD_STRING,
  BUILD_LIST,
  LOAD_PTR,
  SET_PTR,
  LOAD_VAR,
  SET_VAR,
  DEF_VAR,
  DEL_VAR,
} ByteCode;

typedef struct Frame {
  List *vars;
  size_t from_pc;
  struct Frame *from_frame;
} Frame;

Frame *Frame_new(size_t from_pc, Frame *from_frame);
Frame *Frame_free(Frame *frame);

void runByteCode(long long *code);

#endif // B_VM_H
