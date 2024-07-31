#pragma once

#include "b_object.h"

typedef enum TokenType {
  EofToken,
  IntToken,
  FloatToken,
  StringToken,
  IdToken,

  IfToken = 100,
  ElseToken,
  WhileToken,
  VarToken,
  FuncToken,
  ReturnToken,
  BreakToken,
  ContinueToken,

  AddToken = 200,
  SubToken,
  MulToken,
  DivToken,
  ModToken,
  EqToken,
  NeToken,
  GtToken,
  LtToken,
  GeToken,
  LeToken,
  LshToken = 300,
  RshToken,
  BitAndToken,
  BitOrToken,
  XorToken,
  AndToken,
  OrToken,
  InvToken,
  NotToken,

  AddEqToken = 1200,
  SubEqToken,
  MulEqToken,
  DivEqToken,
  ModEqToken,
  LshEqToken = 1300,
  RshEqToken,
  BitAndEqToken,
  BitOrEqToken,
  XorEqToken,

  LParen = 2000,
  RParen,
  LSqBr,
  RSqBr,
  Begin,
  End,
  Comma,
  Colon,
  Semi,
  Dot,
  Assign,
} TokenType;

extern TokenType opstr_table[65536];
void init_opstr_table();

typedef struct Token {
  TokenType tp;
  union {
    long long intToken;
    double floatToken;
    String *strToken;
  };
} Token;

typedef struct TokenList {
  Token *tokens;
  size_t len, max;
} TokenList;

TokenList *tokenize(char *);
void TokenList_free(TokenList *);
