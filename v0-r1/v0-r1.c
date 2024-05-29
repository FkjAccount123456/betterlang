#include "includes.h"
#include "gc.h"
#include "b_lexer.h"

void init() {
  ObjTrait_init();
  init_op_prio();
}

void quit() {
}

int main(int argc, char** argv) {
  init();
  return 0;
}
