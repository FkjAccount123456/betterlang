#include "lex.h"

Parser *Parser_new(TokenList *tokens) {
  Parser *parser = malloc(sizeof(Parser));
  parser->tokens = tokens->tokens;
  parser->cur = parser->tokens;
  parser->size = 0;
  parser->max = 8;
  parser->output = malloc(sizeof(VMCode) * parser->max);
  return parser;
}

Token Parser_next(Parser *p) {
  return *p->cur++;
}

Token Parser_eat(Parser *p, TokenType tp) {
  if (*p->cur == tp)
    return *p->cur++;
  printf("Unexpected token");
  exit(-1);
}

void Parser_add_output(Parser *p, VMCode code) {
  if (p->size == p->max) {
    p->max *= 2;
    p->output = realloc(p->output, sizeof(VMCode) * p->max);
  }
  p->output[p->size++] = code;
}

void Parser_factor(Parser *p) {
  if (p->cur->tp == EOF_TOKEN) {
    printf("Unexpected EOF");
    exit(-1);
  } else if (p->cur->tp == INT_TOKEN) {
    VMCode code = VMCode_new(PUSH_I);
    code.i = Parser_next(p)->int_token;
    Parser_add_output(p, code);
  } else if (p->cur->tp == FLOAT_TOKEN) {
    VMCode code = VMCode_new(PUSH_F);
    code.f = Parser_next(p)->float_token;
    Parser_add_output(p, code);
  } else if (p->cur->tp == STR_TOKEN) {
    VMCode code =  VMCode_new(PUSH_S);
    code.s = Parser_next(p)->str_token;
    Parser_add_output(p, code);
  } else if (p->cur->tp == ID_TOKEN) {

  } else if (p->cur->tp == LSQBR) {
  } else if (p->cur->tp == LPAREN) {
  } else {
    printf("Unexpected token");
    exit(-1);
  }
}

void Parser_expr(Parser *p) {
}

void Parser_stmt(Parser *p) {
}

void Parser_block(Parser *p) {
}

void Parser_program(Parser *p) {
}

void Parser_free(Parser *parser) {
  TokenList_free(parser->tokens);
  for (size_t i = 0; i < parser->size; i++)
    VMCode_free(parser->output[i])
  free(parser->output);
  free(parser);
}
