#include "b_vm.h"
#include "b_gc.h"
#include "b_string.h"

Frame *Frame_new(size_t from_pc, Frame *from_frame) {
  Frame *frame = (Frame *)b_alloc(sizeof(Frame));
  frame->from_pc = from_pc;
  frame->from_frame = from_frame;
  frame->vars = List_new();
  return frame;
}

Frame *Frame_free(Frame *frame) {
  Frame *from_frame = frame;
  List_free(frame->vars);
  b_free(frame);
  return from_frame;
}

void runByteCode(long long *code) {
  Frame *frame = Frame_new(0, 0);
  ObjTrait *strtrait = String_ObjTrait();
  size_t pc = 0;
  while (code[pc]) {
    if (code[pc] == PUSH) {
      List_append(frame->vars, Object_int(code[++pc]));
    } else if (code[pc] == POP) {
      Object_free(&frame->vars->val[--frame->vars->size]);
    } else if (code[pc] == LOAD_STRING) {
      List_append(frame->vars, Object_gc(GC_Object_init(String_new((char *)code[++pc]), strtrait)));
    } else {
      printf("Unknown bytecode head %d.\n", code[pc]);
      exit(-1);
    }
    pc++;
  }
  Frame_free(frame);
}
