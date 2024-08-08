/*
AST基本操作，解析见parse.c，编译见compile.c
*/

#pragma once

#include "b_lex.h"
#include "b_object.h"
#include "b_seq.h"

typedef struct ASTNode ASTNode;

typedef enum ASTType {
  NoOpStmt,   // -null
  BlockStmt,  // -list
  ExprStmt,   // -node
  AssignStmt, // -list
  IfStmt,
  WhileStmt,
  VarDeclStmt,
  FuncDefStmt,
  ReturnStmt, // -node
  BreakStmt,  // -null
  ContinueStmt,

  IntExpr,    // -int
  FloatExpr,  // -float
  StringExpr, // -string
  NoneExpr,   // -null
  VarExpr,    // -string
  ExprExpr,   // -list
  AndExpr,    // 需短路处理
  OrExpr,
  UnaryExpr,
  TernaryExpr,
  BuildListExpr,
  BuildDictExpr,
  LambdaExpr,
  CallExpr,
  AttrExpr,
  IndexExpr,

  OpTerminal, // -TokenType
} ASTType;

/* typedef struct ASTList {
  ASTNode **nodes;
  size_t len, max;
} ASTList;

ASTList *ASTList_new(size_t);
void ASTList_append(ASTList *, ASTNode *);
void ASTList_free(ASTList *); */

typedef struct Seq(ASTNode *) ASTList;

typedef struct ASTNode {
  ASTType type;
  union {
    TokenType opAST;
    long long intAST;
    double floatAST;
    String *stringAST;
    ASTList listAST;
    ASTNode *nodeAST;
  };
} ASTNode;

#define ASTNode_init(tp, attr, val)                                            \
  ({                                                                           \
    ASTNode *node = ASTNode_new(tp);                                           \
    node->attr = val;                                                          \
    node;                                                                      \
  })

ASTNode *ASTNode_new(ASTType);
void ASTNode_free(ASTNode *);
void ASTNode_print(ASTNode *, size_t);
