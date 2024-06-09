#include "b_lexer.h"
#include "b_std.h"
#include "gc.h"
#include "b_parse.h"

Scope *scope;

void init() {
  ObjTrait_init();
  init_op_prio();
  scope = std_scope();
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
  String *code = read_file(filename);
  Lexer *lexer = Lexer_new(code->val);
  Stmt *program = Parser_program(lexer);
  Stmt_run(program, scope);
  String_free(code);
}

void quit() { Scope_free(scope); }

int main() {
  init();
  run_file("test.txt");
  // puts("Hello!");
  quit();
  // puts("Hello!");
  return 0;
}
