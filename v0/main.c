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
  b_free(str_trait);
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
  b_free(list_trait);
}

void test_gc() {
  ObjTrait *str_trait = String_ObjTrait();
  Object str1 = Object_gc(GC_Object_init(String_new("Hello, "), str_trait));
  Object str2 = Object_gc(GC_Object_init(String_new("world!"), str_trait));
  Object str3 = Object_gc(GC_Object_pass(str1.gcObj));
  printf("%s\n", GC_Object_getString(str1)->val);
  printf("%s\n", GC_Object_getString(str2)->val);
  printf("%s\n", GC_Object_getString(str3)->val);
  Object_free(&str1);
  Object_free(&str2);
  Object_free(&str3);
  b_free(str_trait);
}

int main() {
  // test_string();
  // test_list();
  // test_gc();
  return 0;
}
