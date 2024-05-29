#ifndef B_LEXER_H
#define B_LEXER_H

#include "includes.h"
#include "b_string.h"

typedef enum TokenType {
  INT_TOKEN,
  ID_TOKEN,
  STR_TOKEN,
  
  IF_TOKEN,
  ELSE_TOKEN,
  WHILE_TOKEN,
  RETURN_TOKEN,
  BREAK_TOKEN,
  CONTINUE_TOKEN,
  VAR_TOKEN,
  FUNC_TOKEN,

  ADD_TOKEN,
  SUB_TOKEN,
  MUL_TOKEN,
  DIV_TOKEN,
  MOD_TOKEN,
  EQ_TOKEN,
  NE_TOKEN,
  GT_TOKEN,
  LT_TOKEN,
  GE_TOKEN,
  LE_TOKEN,
  AND_TOKEN,
  OR_TOKEN,
  XOR_TOKEN,
  NOT_TOKEN,

  LPAREN_TOKEN,
  RPAREN_TOKEN,
  LSQBR_TOKEN,
  RSQBR_TOKEN,
  BEGIN_TOKEN,
  END_TOKEN,
  COMMA_TOKEN,
  SEMI_TOKEN,
  COLON_TOKEN,
  DOT_TOKEN,
  ASSIGN_TOKEN,
} TokenType;

extern size_t op_prio[128];
void init_op_prio();

typedef struct Token {
  TokenType tp;
  union {
    long long intToken;
    String* strToken;
  };
} Token;

Token* Token_new(TokenType tp);

Token* Token_int(long long intToken);
Token* Token_str(TokenType tp, String* strToken);

typedef struct Lexer {
  char* code;
  char* cur;
  Token* token;
} Lexer;

Lexer* Lexer_new(char* code);
void Lexer_free(Lexer* lexer);
void Lexer_next(Lexer* l);
char Lexer_escape(Lexer* l);
void Lexer_skip(Lexer* l);

#endif // B_LEXER_H