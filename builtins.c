#include "builtins.h"
#include <assert.h>
#include <stdio.h>

Object _b_print(size_t nargs, Object *args) {
  for (size_t i = 0; i < nargs; i++) {
    Object_print(args + i, true);
  }
  return Object_none();
}

Object _b_println(size_t nargs, Object *args) {
  for (size_t i = 0; i < nargs; i++) {
    Object_print(args + i, true);
  }
  putchar('\n');
  return Object_none();
}

Object _b_getchar(size_t nargs, Object *args) {
  assert(nargs == 0);
  return Object_int(getchar());
}

Object _b_putchar(size_t nargs, Object *args) {
  assert(nargs == 1);
  assert(args[0].tp == &int_trait);
  putchar(args[0].intVal);
  return Object_none();
}

Object _b_ord(size_t nargs, Object *args) {
  assert(nargs == 1);
  assert(args[0].tp == &string_trait);
  assert(args[0].stringVal->len == 1);
  return Object_int(args[0].stringVal->v[0]);
}

Object _b_chr(size_t nargs, Object *args) {
  assert(nargs == 1);
  assert(args[0].tp == &int_trait);
  String *str = String_new("");
  String_append(str, args[0].intVal);
  return Object_string(str);
}

Object _b_len(size_t nargs, Object *args) {
  assert(nargs == 1);
  if (args[0].tp == &string_trait) {
    return Object_int(args[0].stringVal->len);
  }
  if (args[0].tp == &list_trait) {
    return Object_int(args[0].listVal->len);
  }
  assert(false);
}

Object _b_append(size_t nargs, Object *args) {
  assert(nargs == 2);
  if (args[0].tp == &list_trait) {
    List_append(args[0].listVal, args[1]);
    return Object_none();
  } else if (args[0].tp == &string_trait) {
    assert(args[1].tp == &int_trait);
    String_append(args[0].stringVal, args[1].intVal);
    return Object_none();
  }
  assert(false);
}
