#ifndef COMPILE_H
#define COMPILE_H

#include "lex.h"
#include "vm.h"
#include "idict.h"

extern char op_prio[128];

void op_prio_init();

typedef struct ParserScope ParserScope;

typedef struct SizeList {
  size_t size, max;
  size_t *items;
} SizeList;

SizeList *SizeList_new();
void SizeList_append(SizeList *list, size_t val);
void SizeList_free(SizeList *list);

typedef struct Parser {
  Token *tokens;
  Token *cur;
  VMCode *output;
  size_t size, max;
  ParserScope *ps;
  SizeList *while_beginposs;
  SizeList *while_jmpends;
} Parser;

Parser *Parser_new(TokenList *tokens);
void Parser_free(Parser *parser);
void Parser_program(Parser *p);

typedef struct ParserScope {
  IDict *dict;
  unsigned int cnt;
  struct ParserScope *parent;
} ParserScope;

ParserScope *ParserScope_new(ParserScope *parent);
void ParserScope_free(ParserScope **ps);

#endif // COMPILE_H
