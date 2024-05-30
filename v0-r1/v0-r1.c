#include "b_lexer.h"
#include "gc.h"
#include "includes.h"

void init() {
  ObjTrait_init();
  init_op_prio();
}

void quit() {}

int main(int argc, char **argv) {
  init();
  return 0;
}
