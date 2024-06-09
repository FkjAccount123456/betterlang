#include "b_std.h"

Object _std_Print(size_t nargs, Object *args) {
  // puts("Hello!");
  for (size_t i = 0; i < nargs; i++) {
    (*args[i].tp->printer)(&args[i]);
  }
  return Object_int(0);
}

Object _std_GetRc(size_t nargs, Object *args) {
  assert(nargs == 1);
  return Object_int(args[0].gcObj->rc);
}

Scope *std_scope() {
  Scope *scope = Scope_new(NULL);

  Scope_define(scope, "Print", Object_builtinfunc(_std_Print));
  Scope_define(scope, "GetRc", Object_builtinfunc(_std_GetRc));

  return scope;
}
