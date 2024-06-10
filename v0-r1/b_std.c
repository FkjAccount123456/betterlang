#include "b_std.h"
#include "b_list.h"

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

Object _std_GetChar(size_t nargs, Object *args) {
  assert(nargs == 0);
  return Object_int(getchar());
}

Object _std_Len(size_t nargs, Object *args) {
  assert(nargs == 1);
  return Object_int((*args[0].tp->lenFn)(&args[0]));
}

Object _std_Append(size_t nargs, Object *args) {
  assert(nargs == 2);
  if (args[0].tp->tp == LIST_OBJ)
    List_append(args[0].gcObj->obj, Object_pass(&args[1], 1));
  else if (args[0].tp->tp == STR_OBJ) {
    assert(args[1].tp->tp == INT_OBJ);
    String_append(args[0].gcObj->obj, args[1].intObj);
  } else {
    assert(false);
  }
  return Object_int(0);
}

Scope *std_scope() {
  Scope *scope = Scope_new(NULL);

  Scope_define(scope, "Print", Object_builtinfunc(_std_Print));
  Scope_define(scope, "GetRc", Object_builtinfunc(_std_GetRc));
  Scope_define(scope, "GetChar", Object_builtinfunc(_std_GetChar));
  Scope_define(scope, "Len", Object_builtinfunc(_std_Len));
  Scope_define(scope, "Append", Object_builtinfunc(_std_Append));

  return scope;
}
