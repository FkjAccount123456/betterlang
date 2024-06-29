#ifndef LEX_H
#define LEX_H

#include "obj.h"

typedef enum TokenType {
  INT_TOKEN,
  FLOAT_TOKEN,
  ID_TOKEN,
  STR_TOKEN,
  EOF_TOKEN,

  IF_TOKEN,
  ELSE_TOKEN,
  WHILE_TOKEN,
  VAR_TOKEN,
  FUNC_TOKEN,
  RETURN_TOKEN,
  BREAK_TOKEN,
  CONTINUE_TOKEN,
  TRUE_TOKEN,
  FALSE_TOKEN,
  NULL_TOKEN,

  ADD_TOKEN,
  SUB_TOKEN,
  MUL_TOKEN,
  DIV_TOKEN,
  EQ_TOKEN,
  NE_TOKEN,
  GT_TOKEN,
  GE_TOKEN,
  LT_TOKEN,
  LE_TOKEN,
  AND_TOKEN,
  OR_TOKEN,
  XOR_TOKEN,
  NOT_TOKEN,

  LPAREN,
  RPAREN,
  LSQBR,
  RSQBR,
  BEGIN,
  END,
  COMMA,
  DOT,
  COLON,
  SEMICOLON,
  ASSIGN,
} TokenType;

typedef struct Token {
  TokenType tp;
  union {
    long long int_token;
    double float_token;
    String *str_token;
  };
} Token;

Token Token_new(TokenType tp);
Token Token_int(long long int_token);
Token Token_float(double float_token);
Token Token_id(String *str);
Token Token_str(String *str);

typedef struct TokenList {
  Token *tokens;
  size_t size, max;
  size_t gc_base;
} TokenList;

TokenList *TokenList_new();
void TokenList_free(TokenList *tl);
void TokenList_append(TokenList *tl, Token token);

bool contains(char *base, char ch);

typedef struct EscapeRes {
  char res;
  char *cur;
} EscapeRes;

EscapeRes _tokenize_escape(char *cur);
TokenList *tokenize(char *code);

#endif // LEX_H
