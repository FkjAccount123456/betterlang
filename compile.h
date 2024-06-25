#ifndef COMPILE_H
#define COMPILE_H

#include "lex.h"
#include "vm.h"

typedef struct Parser {
  Token *tokens;
  Token *cur;
  VMCode *output;
  size_t size, max;
} Parser;

Parser *Parser_new(TokenList *tokens);
void Parser_free(Parser *parser);
void Parser_program(Parser *p);

#endif // COMPILE_H
