#ifndef AST_H
#define AST_H

#include "b_lexer.h"
#include "b_string.h"

typedef enum ExprType {
  INT_AST,
  STR_AST,
  ID_AST,
  BINARY_AST,
  UNARY_AST,
  CALL_AST,
  BUILDLIST_AST,
  INDEX_AST,
} ExprType;

typedef struct Expr {
  ExprType type;
  union {
    long long int_ast;
    String *str_ast;
    struct {
      TokenType op;
      struct Expr *left, *right;
    } binary_ast;
    struct {
      TokenType op;
      struct Expr *expr;
    } unary_ast;
    struct {
      struct Expr *func;
      struct Expr **args;
      int nargs;
    } call_ast;
    struct {
      struct Expr **items;
      int nitems;
    } buildlist_ast;
    struct {
      struct Expr *expr;
      struct Expr *index;
    } index_ast;
  };
} Expr;

Expr *Expr_new(ExprType type);
void Expr_free(Expr *expr);

#endif // AST_H
