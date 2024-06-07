#include "b_parse.h"
#include "ast.h"
#include "b_lexer.h"
#include "seq.h"
#include <stdio.h>
#include <stdlib.h>

void Parser_eat(Lexer *l, TokenType tp) {
  if (!l->token || l->token->tp != tp) {
    printf("Unexpected token.");
    exit(-1);
  }
  Lexer_next(l);
}

Expr *Parser_factor(Lexer *l) {
  Expr *res;
  if (l->token == NULL) {
    printf("Unexpected EOF.");
    exit(-1);
  } else if (l->token->tp == INT_TOKEN) {
    res = Expr_new(INT_AST);
    res->int_ast = l->token->intToken;
    Lexer_next(l);
  } else if (l->token->tp == ID_TOKEN) {
    res = Expr_new(ID_AST);
    res->str_ast = l->token->strToken;
    Lexer_next(l);
  } else if (l->token->tp == LPAREN_TOKEN) {
    Lexer_next(l);
    res = Parser_expr(l);
    Parser_eat(l, RPAREN_TOKEN);
  } else if (l->token->tp == LSQBR_TOKEN) {
    Lexer_next(l);
    NewSeq(Expr *, items);
    if (l->token && l->token->tp != RSQBR_TOKEN) {
      SeqAppend(Expr *, items, Parser_expr(l));
      while (l->token && l->token->tp == COMMA_TOKEN) {
        Lexer_next(l);
        SeqAppend(Expr *, items, Parser_expr(l));
      }
    }
    Parser_eat(l, RSQBR_TOKEN);
    res = Expr_new(BUILDLIST_AST);
    res->buildlist_ast.items = items_val;
    res->buildlist_ast.nitems = items_size;
  } else if (l->token->tp == ADD_TOKEN || l->token->tp == SUB_TOKEN ||
             l->token->tp == NOT_TOKEN) {
    res = Expr_new(UNARY_AST);
    res->unary_ast.op = l->token->tp;
    Lexer_next(l);
    res->unary_ast.expr = Parser_expr(l);
    return res;
  } else {
    printf("Unexpected token.");
    exit(-1);
  }

  while (true) {
    if (l->token && l->token->tp == LPAREN_TOKEN) {
      Lexer_next(l);
      NewSeq(Expr *, args);
      if (l->token && l->token->tp != RPAREN_TOKEN) {
        SeqAppend(Expr *, args, Parser_expr(l));
        while (l->token && l->token->tp == COMMA_TOKEN) {
          Lexer_next(l);
          SeqAppend(Expr *, args, Parser_expr(l));
        }
      }
      Parser_eat(l, RPAREN_TOKEN);
      Expr *func = res;
      res = Expr_new(CALL_AST);
      res->call_ast.func = func;
      res->call_ast.args = args_val;
      res->call_ast.nargs = args_size;
    } else if (l->token && l->token->tp == LSQBR_TOKEN) {
      Lexer_next(l);
      Expr *expr = res;
      Expr *index = Parser_expr(l);
      res = Expr_new(INDEX_AST);
      res->index_ast.expr = res;
      res->index_ast.index = index;
    } else {
      break;
    }
  }
  return res;
}

Expr *Parser_expr(Lexer *l) {
  NewSeq(TokenType, op_stack);
  NewSeq(Expr *, res_stack);
  while (l->token && op_prio[l->token->tp]) {
    TokenType op = l->token->tp;
    Lexer_next(l);
    while (op_stack_size && op_prio[SeqBack(op_stack)] >= op_prio[op]) {
      res_stack_size--;
      Expr *left = SeqBack(res_stack);
      SeqBack(res_stack) = Expr_new(BINARY_AST);
      SeqBack(res_stack)->binary_ast.op = SeqPop(op_stack);
      SeqBack(res_stack)->binary_ast.left = left;
      SeqBack(res_stack)->binary_ast.right = res_stack_val[res_stack_size];
    }
    SeqAppend(TokenType, op_stack, op);
    SeqAppend(Expr *, res_stack, Parser_factor(l));
  }
  while (op_stack_size) {
    res_stack_size--;
    Expr *left = SeqBack(res_stack);
    SeqBack(res_stack) = Expr_new(BINARY_AST);
    SeqBack(res_stack)->binary_ast.op = SeqPop(op_stack);
    SeqBack(res_stack)->binary_ast.left = left;
    SeqBack(res_stack)->binary_ast.right = res_stack_val[res_stack_size];
  }
  Expr *res = SeqBack(res_stack);
  FreeSeq(res_stack);
  FreeSeq(op_stack);
  return res;
}

Stmt *Parser_stmt(Lexer *l) {
  if (l->token == NULL) {
    printf("Unexpected EOF.");
    exit(-1);
  } else if (l->token->tp == SEMI_TOKEN) {
    Lexer_next(l);
    return Stmt_new(NOOP_AST);
  } else if (l->token->tp == RETURN_TOKEN) {
    Lexer_next(l);
    Stmt *res = Stmt_new(RETURN_AST);
    res->expr_ast = Parser_expr(l);
    Parser_eat(l, SEMI_TOKEN);
    return res;
  } else if (l->token->tp == BREAK_TOKEN) {
    Lexer_next(l);
    Parser_eat(l, SEMI_TOKEN);
    return Stmt_new(BREAK_AST);
  } else if (l->token->tp == CONTINUE_TOKEN) {
    Lexer_next(l);
    Parser_eat(l, SEMI_TOKEN);
    return Stmt_new(CONTINUE_AST);
  } else if (l->token->tp == BEGIN_TOKEN) {
    return Parser_block(l);
  } else if (l->token->tp == VAR_TOKEN) {
    Lexer_next(l);
    NewSeq(String *, varnames);
    NewSeq(Expr *, varvals);
    if (!l->token || l->token->tp != ID_TOKEN) {
      printf("Expect an identifier.");
      exit(-1);
    }
    SeqAppend(String *, varnames, l->token->strToken);
    Lexer_next(l);
    if (l->token && l->token->tp == ASSIGN_TOKEN) {
      Lexer_next(l);
      SeqAppend(Expr *, varvals, Parser_expr(l));
    } else {
      Expr *zero = Expr_new(INT_AST);
      zero->int_ast = 0;
      SeqAppend(Expr *, varvals, zero);
    }
    while (l->token && l->token->tp == SEMI_TOKEN) {
      if (!l->token || l->token->tp != ID_TOKEN) {
        printf("Expect an identifier.");
        exit(-1);
      }
      SeqAppend(String *, varnames, l->token->strToken);
      Lexer_next(l);
      if (l->token && l->token->tp == ASSIGN_TOKEN) {
        Lexer_next(l);
        SeqAppend(Expr *, varvals, Parser_expr(l));
      } else {
        Expr *zero = Expr_new(INT_AST);
        zero->int_ast = 0;
        SeqAppend(Expr *, varvals, zero);
      }
    }
    Parser_eat(l, SEMI_TOKEN);
    Stmt *res = Stmt_new(VARDECL_AST);
    res->vardecl_ast.nvars = varnames_size;
    res->vardecl_ast.varnames = varnames_val;
    res->vardecl_ast.varvals = varvals_val;
    return res;
  } else if (l->token->tp == FUNC_TOKEN) {
    Lexer_next(l);
    if (!l->token || l->token->tp != ID_TOKEN) {
      printf("Expect an identifier.");
      exit(-1);
    }
    String *name = l->token->strToken;
    Lexer_next(l);
    Parser_eat(l, LPAREN_TOKEN);
    NewSeq(String *, params);
    if (l->token && l->token->tp != RPAREN_TOKEN) {
      if (!l->token || l->token->tp != ID_TOKEN) {
        printf("Expect an identifier.");
        exit(-1);
      }
      SeqAppend(String *, params, l->token->strToken);
      while (l->token && l->token->tp == COMMA_TOKEN) {
        Lexer_next(l);
        if (!l->token || l->token->tp != ID_TOKEN) {
          printf("Expect an identifier.");
          exit(-1);
        }
        SeqAppend(String *, params, l->token->strToken);
      }
    }
    Parser_eat(l, RPAREN_TOKEN);
    Stmt *body = Parser_block(l);
    Stmt *res = Stmt_new(FUNCDEF_AST);
    res->funcdef_ast.body = body;
    res->funcdef_ast.name = name;
    res->funcdef_ast.nparams = params_size;
    res->funcdef_ast.params = params_val;
    return res;
  } else if (l->token->tp == IF_TOKEN) {
    Lexer_next(l);
    Stmt *res = Stmt_new(IF_AST);
    res->if_ast.cond = Parser_expr(l);
    res->if_ast.t = Parser_block(l);
    res->if_ast.f = NULL;
    for (Stmt *cur = res->if_ast.f; l->token->tp == ELSE_TOKEN; cur = cur->if_ast.f) {
      Lexer_next(l);
      if (l->token->tp == IF_TOKEN) {
        Lexer_next(l);
        cur->if_ast.cond = Parser_expr(l);
        cur->if_ast.t = Parser_block(l);
        cur->if_ast.f = NULL;
      } else {
        cur->if_ast.f = Parser_block(l);
        break;
      }
    }
    return res;
  } else if (l->token->tp == WHILE_TOKEN) {
    Lexer_next(l);
    Stmt *res = Stmt_new(WHILE_AST);
    res->while_ast.cond = Parser_expr(l);
    res->while_ast.body = Parser_block(l);
    return res;
  } else {
    Expr *left = Parser_expr(l);
    if (l->token && l->token->tp == ASSIGN_TOKEN) {
      Lexer_next(l);
      Expr *right = Parser_expr(l);
      Parser_eat(l, SEMI_TOKEN);
      Stmt *res = Stmt_new(ASSIGN_AST);
      res->assign_ast.left = left;
      res->assign_ast.right = right;
      return res;
    }
    Parser_eat(l, SEMI_TOKEN);
    Stmt *res = Stmt_new(EXPR_AST);
    return res;
  }
}

Stmt *Parser_block(Lexer *l) {
  Parser_eat(l, BEGIN_TOKEN);
  NewSeq(Stmt *, stmts);
  while (l->token && l->token->tp != END_TOKEN) {
    SeqAppend(Stmt *, stmts, Parser_stmt(l));
  }
  Parser_eat(l, END_TOKEN);
  Stmt *stmt = Stmt_new(BLOCK_AST);
  stmt->block_ast.nstmts = stmts_size;
  stmt->block_ast.stmts = stmts_val;
  return stmt;
}
