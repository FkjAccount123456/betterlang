#include "b_object.h"
#include "b_gc.h"
#include "b_string.h"
#include "b_list.h"
#include <stdio.h>

void test_string() {
  char *s = "Hello, world!";
  String *str = String_new(s);
  printf("%s\n", str->val);
  ObjTrait *str_trait = String_ObjTrait();
  Object obj = Object_gc(GC_Object_init(str, str_trait));
  printf("%s\n", ((String *)obj.gcObj->ptr)->val);
  GC_Object_decrc(obj.gcObj);
  Object_free(&obj);
}

int main() {
  test_string();
  return 0;
}
