#include "b_lexer.h"
#include "gc.h"
#include "b_std.h"

Scope *scope;

void init() {
  ObjTrait_init();
  init_op_prio();
  scope = std_scope();
}

void quit() {}

int main(int argc, char **argv) {
  init();
  return 0;
}
