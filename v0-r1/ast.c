#include "ast.h"

Expr *Expr_new(ExprType type) {
  Expr *expr = (Expr *)malloc(sizeof(Expr));
  if (expr == NULL) {
    printf("Failed to malloc");
    exit(-1);
  }
  expr->type = type;
  return expr;
}

void Expr_free(Expr *expr) {
  switch (expr->type) {
  case INT_AST:
    break;
  case STR_AST:
    break;
  case ID_AST:
    break;
  case BINARY_AST:
    Expr_free(expr->binary_ast.left);
    Expr_free(expr->binary_ast.right);
    break;
  case UNARY_AST:
    Expr_free(expr->unary_ast.expr);
    break;
  case CALL_AST:
    Expr_free(expr->call_ast.func);
    for (size_t i = 0; i < expr->call_ast.nargs; i++) {
      Expr_free(expr->call_ast.args[i]);
    }
    free(expr->call_ast.args);
    break;
  case BUILDLIST_AST:
    for (size_t i = 0; i < expr->buildlist_ast.nitems; i++) {
      Expr_free(expr->buildlist_ast.items[i]);
    }
    free(expr->buildlist_ast.items);
    break;
  case INDEX_AST:
    Expr_free(expr->index_ast.expr);
    Expr_free(expr->index_ast.index);
    break;
  default:
    printf("Unknown ast type.");
    exit(-1);
  }
  free(expr);
}

Stmt *Stmt_new(StmtType type) {
  Stmt *stmt = (Stmt *)malloc(sizeof(Stmt));
  if (stmt == NULL) {
    printf("Failed to malloc");
    exit(-1);
  }
  stmt->type = type;
  return stmt;
}

void Stmt_free(Stmt *stmt) {
  switch (stmt->type) {
  case VARDECL_AST:
    for (size_t i = 0; i < stmt->vardecl_ast.nvars; i++) {
      String_free(stmt->vardecl_ast.varnames[i]);
      Expr_free(stmt->vardecl_ast.varvals[i]);
    }
    free(stmt->vardecl_ast.varnames);
    free(stmt->vardecl_ast.varvals);
    break;
  case FUNCDEF_AST:
    free(stmt->funcdef_ast.name);
    for (size_t i = 0; i < stmt->funcdef_ast.nparams; i++) {
      String_free(stmt->funcdef_ast.params[i]);
    }
    free(stmt->funcdef_ast.params);
    break;
  case IF_AST:
    Expr_free(stmt->if_ast.cond);
    Stmt_free(stmt->if_ast.t);
    Stmt_free(stmt->if_ast.f);
    break;
  case WHILE_AST:
    Expr_free(stmt->while_ast.cond);
    Stmt_free(stmt->while_ast.body);
    break;
  case EXPR_AST:
  case RETURN_AST:
    Expr_free(stmt->expr_ast);
    break;
  case NOOP_AST:
  case BREAK_AST:
  case CONTINUE_AST:
    break;
  case BLOCK_AST:
    for (size_t i = 0; i < stmt->block_ast.nstmts; i++) {
      Stmt_free(stmt->block_ast.stmts[i]);
    }
    free(stmt->block_ast.stmts);
    break;
  case ASSIGN_AST:
    Expr_free(stmt->assign_ast.left);
    Expr_free(stmt->assign_ast.right);
    break;
  }
  free(stmt);
}

long long _binary_operate(TokenType op, long long left, long long right) {
  switch (op) {
    case ADD_TOKEN:
      return left + right;
    case SUB_TOKEN:
      return left - right;
    case MUL_TOKEN:
      return left * right;
    case DIV_TOKEN:
      return left / right;
  }
}

Object Expr_eval(Expr *expr, Scope *scope) {
  switch (expr->type) {
  case INT_AST:
    return Object_int(expr->int_ast);
  case STR_AST:
    return Object_gc(&strTrait, GC_Object_init(String_copy(expr->str_ast)));
  case ID_AST:
  {
    Object *res = Scope_find(scope, expr->str_ast->val);
    if (res == NULL) {
      printf("Undefine variable '%s'.", expr->str_ast->val);
      exit(-1);
    }
    return *res;
  }
  case BINARY_AST:
  }
}

RunSignal Stmt_run(Stmt *stmt, Scope *scope) {
  switch (stmt->type) {
  }
}
