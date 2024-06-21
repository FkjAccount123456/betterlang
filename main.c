#include "b_obj.h"

#define ObjPtr(obj) (obj.gc_obj->ptr)
#define ObjTypePtr(type, obj) ((type *)obj.gc_obj->ptr)
#define NodeTypePtr(type, node) ((type *)node->ptr)

void test_obj() {
  GC_Root gc = GC_new();
  printf("Root: %llx\n", gc.gc_rootnode);
  Object str1 = Object_gc(STR_OBJ, String_new("Hello, ", &gc), gc.gc_rootnode);
  Object str2 = Object_gc(STR_OBJ, String_new("world!", &gc), gc.gc_rootnode);
  Object sumstr = Object_gc(STR_OBJ, String_new(((String *)ObjPtr(str1))->val->ptr, &gc), gc.gc_rootnode);
  Object str4 = Object_pass(str1);
  String_cat(ObjPtr(sumstr), ObjPtr(str2), &gc);
  printf("%s\n", ((String *)ObjPtr(str1))->val->ptr);
  printf("%s\n", ((String *)ObjPtr(str2))->val->ptr);
  printf("%s\n", ((String *)ObjPtr(sumstr))->val->ptr);
  printf("%s\n", ((String *)ObjPtr(str4))->val->ptr);
  Object_free(str1);
  Object_free(str2);
  Object_free(sumstr);
  Object_free(str4);
  printf("%llu\n", str1.gc_obj->cnt);
  GC_collect(&gc);
}

void test_list() {
  GC_Root gc = GC_new();
  printf("Root: %llx\n", gc.gc_rootnode);
  Object list = Object_gc(LIST_OBJ, List_new(&gc), gc.gc_rootnode);
  for (long long i = 0; i < 10; i++) {
    List_append(ObjPtr(list), Object_int(i), &gc);
    for (size_t i = 0; i < ObjTypePtr(List, list)->size; i++) {
      printf("%lld ", NodeTypePtr(Object, ObjTypePtr(List, list)->items)[i].int_obj);
    }
    puts("");
  }
  Object_free(list);
  GC_collect(&gc);
}

void test_dict() {
  GC_Root gc = GC_new();
  printf("Root: %llx\n", gc.gc_rootnode);
  Object dict = Object_gc(DICT_OBJ, Dict_new(&gc), gc.gc_rootnode);
  Dict_insert(ObjPtr(dict), "abc", Object_int(10), &gc);
  Dict_insert(ObjPtr(dict), "abd", Object_int(20), &gc);
  Dict_insert(ObjPtr(dict), "acc", Object_int(30), &gc);
  printf("abc: %lld\n", Dict_find(ObjPtr(dict), "abc")->int_obj);
  printf("abd: %lld\n", Dict_find(ObjPtr(dict), "abd")->int_obj);
  printf("acc: %lld\n", Dict_find(ObjPtr(dict), "acc")->int_obj);
  Object_free(dict);
  GC_collect(&gc);
}

int main(int argc, char *argv[]) {
  // test_obj();
  // test_list();
  test_dict();
  return 0;
}
