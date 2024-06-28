#include "obj.h"

void init() {
  GC_init();
}

void test_String() {
  String *str = String_new("Hello, ");
  String *str2 = String_new("world!");
  GC_active_add(str->gc_base);
  GC_active_add(str2->gc_base);
  printf("%s\n", str->val);
  printf("%s\n", str->val);
  String_cat(str, str2->val);
  printf("%s\n", str->val);
  printf("str: %llu, str2: %llu\n", str->gc_base, str2->gc_base);
  String *str3 = malloc(sizeof(String));
  *str3 = *str;
  // TODO: 此处有误
  str3->gc_base = GC_objs_add(gc.G_bases[str->gc_base]);
  GC_active_add(str3->gc_base);
  GC_obj_add_ch(str->gc_base, str3->gc_base);
  GC_obj_add_ch(str3->gc_base, str->gc_base);
  printf("%s\n", str3->val);
  printf("str: %llu, str2: %llu, str3: %llu\n", str->gc_base, str2->gc_base, str3->gc_base);
  GC_active_remove(str->gc_base);
  GC_collect();
  GC_active_remove(str2->gc_base);
  GC_collect();
  GC_active_remove(str3->gc_base);
  GC_collect();
}

void quit() {
  GC_quit();
}

int main(int argc, char **argv) {
  init();
  test_String();
  quit();
  return 0;
}
