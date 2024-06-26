#include "obj.h"
#include "lex.h"
#include "compile.h"
#include "vm.h"

void init() {
  GC_init();
  op_prio_init();
  ObjectTrait_init();
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

void test_List() {
  List *list = List_new();
  GC_active_add(list->gc_base);
  for (long long i = 0; i < 10; i++) {
    List_append(list, Object_int(i));
    for (size_t j = 0; j < list->size; j++)
      printf("%lld ", list->items[j].i);
    puts("");
  }
  GC_active_remove(list->gc_base);
  GC_collect();
}

void test_Dict() {
  Dict *dict = Dict_new();
  GC_active_add(dict->gc_base);
  Dict_insert(dict, "abc", Object_int(1));
  Dict_insert(dict, "abd", Object_int(2));
  Dict_insert(dict, "acd", Object_int(3));
  printf("abc: %lld, abd: %lld, acd: %lld\n",
         Dict_find(dict, "abc")->i,
         Dict_find(dict, "abd")->i,
         Dict_find(dict, "acd")->i);
  GC_active_remove(dict->gc_base);
  GC_collect();
}

String *read_file(char *filename) {
  FILE *fp = fopen(filename, "r");
  if (fp == NULL) {
    printf("Error: file not found\n");
    exit(-1);
  }
  String *res = String_new("");
  char c;
  while ((c = fgetc(fp)) != EOF) {
    String_append(res, c);
  }
  fclose(fp);
  return res;
}

void run_file(char *filename) {
  String *data = read_file(filename);
  // printf("%s", data->val);
  TokenList *tl = tokenize(data->val);
  GC_collect();
  // for (size_t i = 0; i < tl->size; i++) {
  //   printf("%d\n", tl->tokens[i].tp);
  // }
  Parser *p = Parser_new(tl);
  GC_active_add(p->gc_base);
  Parser_program(p);
  for (size_t i = 0; i < p->size; i++) {
    printf("%llu: ", i);
    VMCode_print(p->output[i]);
  }
  VMCode_run(p->output);
  GC_active_remove(p->gc_base);
  GC_active_remove(tl->gc_base);
  GC_collect();
}

void quit() {
  GC_quit();
}

int main(int argc, char **argv) {
  init();
  printf("+: %d *: %d", op_prio[ADD], op_prio[SUB]);
  // test_String();
  // test_List();
  // test_Dict();
  if (argc != 2) {
    printf("Expect file name");
    exit(-1);
  }
  run_file(argv[1]);
  quit();
  return 0;
}
