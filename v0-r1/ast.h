#ifndef AST_H
#define AST_H

#include "b_lexer.h"
#include "gc.h"
#include "b_scope.h"

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

typedef enum StmtType {
  VARDECL_AST,
  FUNCDEF_AST,
  IF_AST,
  WHILE_AST,
  RETURN_AST,
  BREAK_AST,
  CONTINUE_AST,
  BLOCK_AST,
  EXPR_AST,
  NOOP_AST,
  ASSIGN_AST,
} StmtType;

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

typedef struct Stmt {
  StmtType type;
  union {
    struct {
      size_t nvars;
      String **varnames;
      Expr **varvals;
    } vardecl_ast;
    struct {
      String *name;
      String **params;
      size_t nparams;
      struct Stmt *body;
    } funcdef_ast;
    struct {
      Expr *cond;
      struct Stmt *t, *f;
    } if_ast;
    struct {
      Expr *cond;
      struct Stmt *body;
    } while_ast;
    struct {
      struct Stmt **stmts;
      size_t nstmts;
    } block_ast;
    struct {
      Expr *left, *right;
    } assign_ast;
    Expr *expr_ast;
  };
} Stmt;

typedef enum RSignalType {
  RETURN_SIGNAL,
  BREAK_SIGNAL,
  CONTINUE_SIGNAL,
} RSignalType;

typedef struct RunSignal {
  RSignalType signal;
  Object ret_val;
} RunSignal;

Expr *Expr_new(ExprType type);
void Expr_free(Expr *expr);
Object Expr_eval(Expr *expr, Scope *scope);

Stmt *Stmt_new(StmtType type);
void Stmt_free(Stmt *stmt);
RunSignal Stmt_run(Stmt *stmt, Scope *scope);

#endif // AST_H
