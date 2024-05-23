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
  Object_free(&obj);
}

void test_list() {
  ObjTrait *list_trait = List_ObjTrait();
  Object obj = Object_gc(GC_Object_init(List_new(), list_trait));
  for (long long i = 0; i < 10; i++) {
    List_append((List *)obj.gcObj->ptr, Object_int(i));
    for (size_t i = 0; i < ((List *)obj.gcObj->ptr)->size; i++) {
      printf("%lld ", ((List *)obj.gcObj->ptr)->val[i].intObj);
    }
    puts("");
  }
  Object_free(&obj);
}

int main() {
  // test_string();
  test_list();
  return 0;
}
