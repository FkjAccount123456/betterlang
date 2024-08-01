#include "b_lex.h"
#include "b_object.h"
#include "gc.h"
#include "parse.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void init();
void test_dict();
void test_list();
void test_parse();

int main(int argc, char **argv) {
  init();
  // test_dict();
  // test_list();
  test_parse();
  return 0;
}

void init() {
  gc_init();
  init_traits();
  init_opstr_table();
}

void test_dict() {
  Object dict = Object_dict(Dict_new());
  Dict_insert(dict.dictVal, "abc", Object_int(10));
  Dict_insert(dict.dictVal, "abd", Object_int(20));
  Dict_insert(dict.dictVal, "acd", Object_int(30));
  Object_print(&dict, true);
  puts("");
  Object_print(Dict_find(dict.dictVal, "abc"), true);
  puts("");
  Object_print(Dict_find(dict.dictVal, "abd"), true);
  puts("");
  Object_print(Dict_find(dict.dictVal, "acd"), true);
  puts("");
  Dict_set(dict.dictVal, "acd", Object_int(20));
  Object_print(&dict, true);
  puts("");
  gc_collect();
}

void test_list() {
  Object list = Object_list(List_new());
  for (long long i = 1; i <= 10; i++) {
    List_append(list.listVal, Object_list(List_new()));
    for (long long j = 1; j <= i; j++) {
      List_append(list.listVal->v[list.listVal->len-1].listVal, Object_int(j));
    }
  }
  Object_print(&list, true);
  puts("");
  gc_collect();
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

void test_parse() {
  String *code = read_file("test2.bl");
  TokenList *tokens = tokenize(code->v);
  Parser *p = Parser_new(tokens);
  ASTNode *ast = parse_program(p);
  ASTNode_print(ast, 0);
  Parser_free(p);
  ASTNode_free(ast);
  String_free(code);
}
