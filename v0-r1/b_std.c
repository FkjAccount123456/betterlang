#include "b_std.h"

Object _std_Print(size_t nargs, Object *args) {
  // puts("Hello!");
  for (size_t i = 0; i < nargs; i++) {
    (*args[i].tp->printer)(&args[i]);
  }
  return Object_int(0);
}

Scope *std_scope() {
  Scope *scope = Scope_new(NULL);

  Scope_define(scope, "Print", Object_builtinfunc(_std_Print));

  return scope;
}
