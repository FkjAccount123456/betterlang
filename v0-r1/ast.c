#include "ast.h"
#include "b_func.h"
#include "b_list.h"
#include "b_string.h"
#include "seq.h"
// #include "b_std.h"

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
  default:
    printf("Unknown ast type.");
    exit(-1);
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
  case MOD_TOKEN:
    return left % right;
  case EQ_TOKEN:
    return left == right;
  case NE_TOKEN:
    return left != right;
  case GT_TOKEN:
    return left > right;
  case GE_TOKEN:
    return left >= right;
  case LT_TOKEN:
    return left < right;
  case LE_TOKEN:
    return left <= right;
  case AND_TOKEN:
    return left && right;
  case OR_TOKEN:
    return left || right;
  case XOR_TOKEN:
    return left ^ right;
  default:
    printf("Unknown binary operator.");
    exit(-1);
  }
}

long long _unary_operate(TokenType op, long long val) {
  switch (op) {
  case ADD_TOKEN:
    return +val;
  case SUB_TOKEN:
    return -val;
  case NOT_TOKEN:
    return !val;
  default:
    printf("Unknown unary operator.");
    exit(-1);
  }
}

Object Expr_eval(Expr *expr, Scope *scope) {
  // printf("Expr_eval: %d\n", expr->type);
  switch (expr->type) {
  case INT_AST:
    return Object_int(expr->int_ast);
  case STR_AST:
    return Object_gc(&strTrait, GC_Object_init(String_copy(expr->str_ast)));
  case ID_AST: {
    Object *res = Scope_find(scope, expr->str_ast->val);
    if (res == NULL) {
      printf("Undefine variable '%s'.", expr->str_ast->val);
      exit(-1);
    }
    return Object_pass(res, 1);
  }
  case BINARY_AST: {
    Object left = Expr_eval(expr->binary_ast.left, scope);
    Object right = Expr_eval(expr->binary_ast.right, scope);
    if (left.tp->tp != INT_OBJ || right.tp->tp != INT_OBJ) {
      printf("Expect two integers.");
      exit(-1);
    }
    Object res = Object_int(
        _binary_operate(expr->binary_ast.op, left.intObj, right.intObj));
    Object_free(&left);
    Object_free(&right);
    return res;
  }
  case UNARY_AST: {
    Object val = Expr_eval(expr->unary_ast.expr, scope);
    if (val.tp->tp != INT_OBJ) {
      printf("Expect an integer.");
      exit(-1);
    }
    Object res = Object_int(_unary_operate(expr->unary_ast.op, val.intObj));
    Object_free(&val);
    return res;
  }
  case CALL_AST: {
    Object _func = Expr_eval(expr->call_ast.func, scope);
    if (_func.tp->tp == BUILTINFUNC_OBJ) {
      BuiltinFunc func = _func.builtinfuncObj;
      NewSeq(Object, args);
      for (size_t i = 0; i < expr->call_ast.nargs; i++) {
        Object arg = Expr_eval(expr->call_ast.args[i], scope);
        SeqAppend(Object, args, arg);
      }
      // printf("fn: %d %d\n", func, _std_Print);
      Object res = func(expr->call_ast.nargs, args_val);
      for (size_t i = 0; i < expr->call_ast.nargs; i++) {
        Object_free(&args_val[i]);
      }
      FreeSeq(args);
      return res;
    }
    if (_func.tp->tp != FUNC_OBJ) {
      printf("Expect a function.");
      exit(-1);
    }
    Func *func = (Func *)_func.gcObj->obj;
    if (func->nparams != expr->call_ast.nargs) {
      printf("Expect %d arguments, got %d.", func->nparams,
             expr->call_ast.nargs);
      exit(-1);
    }
    Scope *call_scope = Scope_new(func->closure);
    for (size_t i = 0; i < expr->call_ast.nargs; i++) {
      Object arg = Expr_eval(expr->call_ast.args[i], scope);
      Scope_define(call_scope, func->params[i]->val, arg);
    }
    RunSignal signal = Stmt_run(func->body, call_scope);
    Scope_free(call_scope);
    if (signal.signal == RETURN_SIGNAL) {
      return signal.ret_val;
    }
    return Object_int(0);
  }
  case BUILDLIST_AST: {
    List *list = List_new();
    for (size_t i = 0; i < expr->buildlist_ast.nitems; i++) {
      Object item = Expr_eval(expr->buildlist_ast.items[i], scope);
      List_append(list, item);
    }
    return Object_gc(&listTrait, GC_Object_init(list));
  }
  case INDEX_AST: {
    // printf("INDEX_AST %d\n", expr->index_ast.expr->type);
    Object _list = Expr_eval(expr->index_ast.expr, scope);
    if (_list.tp->tp != LIST_OBJ) {
      printf("Expect a list.");
      exit(-1);
    }
    // puts("Hello!");
    // printf("Rc list: %d\n", _list.gcObj->rc);
    List *list = (List *)_list.gcObj->obj;
    Object _index = Expr_eval(expr->index_ast.index, scope);
    if (_index.tp->tp != INT_OBJ) {
      printf("Expect an integer index.");
      exit(-1);
    }
    long long index = _index.intObj;
    if (index < 0 || index >= list->size) {
      printf("Index out of range.");
      exit(-1);
    }
    // if (list->val[index].tp->need_gc) {
    //   printf("Rc list: %d\n", _list.gcObj->rc);
    //   printf("Rc res: %d\n", list->val[index].gcObj->rc);
    // }
    Object res = Object_pass(&list->val[index], 1);
    // if (res.tp->need_gc) {
    //   printf("Rc list: %d\n", _list.gcObj->rc);
    //   printf("Rc res: %d\n", res.gcObj->rc);
    // }
    Object_free(&_list);
    Object_free(&_index);
    // if (res.tp->need_gc) {
    //   printf("Rc list: %d\n", _list.gcObj->rc);
    //   printf("Rc res: %d\n", res.gcObj->rc);
    // }
    return res;
  }
  default:
    printf("Unknown ast type %d.", expr->type);
    exit(-1);
  }
}

RunSignal RunSignal_new(RSignalType signal, Object ret_val) {
  RunSignal run_signal = {signal, ret_val};
  return run_signal;
}

RunSignal Stmt_run(Stmt *stmt, Scope *scope) {
  // printf("Stmt_run: %d\n", stmt->type);
  switch (stmt->type) {
  case VARDECL_AST: {
    for (size_t i = 0; i < stmt->vardecl_ast.nvars; i++) {
      Object val = Expr_eval(stmt->vardecl_ast.varvals[i], scope);
      Scope_define(scope, stmt->vardecl_ast.varnames[i]->val, val);
    }
    return RunSignal_new(NONE_SIGNAL, Object_int(0));
  }
  case FUNCDEF_AST: {
    Func *func = (Func *)malloc(sizeof(Func));
    if (func == NULL) {
      printf("Failed to malloc");
      exit(-1);
    }
    func->nparams = stmt->funcdef_ast.nparams;
    func->params = stmt->funcdef_ast.params;
    func->body = stmt->funcdef_ast.body;
    func->closure = Scope_pass(scope);
    Scope_define(scope, stmt->funcdef_ast.name->val,
                 Object_gc(&funcTrait, GC_Object_init(func)));
    return RunSignal_new(NONE_SIGNAL, Object_int(0));
  }
  case IF_AST: {
    Scope *new_scope = Scope_new(scope);
    Object cond = Expr_eval(stmt->if_ast.cond, new_scope);
    RunSignal res;
    if ((*cond.tp->toBooler)(&cond)) {
      res = Stmt_run(stmt->if_ast.t, new_scope);
    } else if (stmt->if_ast.f != NULL) {
      res = Stmt_run(stmt->if_ast.f, new_scope);
    } else {
      res = RunSignal_new(NONE_SIGNAL, Object_int(0));
    }
    Object_free(&cond);
    Scope_free(new_scope);
    return res;
  }
  case WHILE_AST: {
    Scope *new_scope = Scope_new(scope);
    Object cond = Expr_eval(stmt->while_ast.cond, scope);
    RunSignal res = RunSignal_new(NONE_SIGNAL, Object_int(0));
    while ((*cond.tp->toBooler)(&cond)) {
      RunSignal signal = Stmt_run(stmt->while_ast.body, new_scope);
      if (signal.signal == RETURN_SIGNAL) {
        res = signal;
        break;
      } else if (signal.signal == BREAK_SIGNAL) {
        break;
      }
      Object_free(&cond);
      cond = Expr_eval(stmt->while_ast.cond, scope);
    }
    Object_free(&cond);
    Scope_free(new_scope);
    return res;
  }
  case RETURN_AST: {
    Object ret_val = Expr_eval(stmt->expr_ast, scope);
    return RunSignal_new(RETURN_SIGNAL, ret_val);
  }
  case BREAK_AST:
    return RunSignal_new(BREAK_SIGNAL, Object_int(0));
  case CONTINUE_AST:
    return RunSignal_new(CONTINUE_SIGNAL, Object_int(0));
  case BLOCK_AST: {
    for (size_t i = 0; i < stmt->block_ast.nstmts; i++) {
      RunSignal res = Stmt_run(stmt->block_ast.stmts[i], scope);
      if (res.signal != NONE_SIGNAL) {
        return res;
      }
    }
    return RunSignal_new(NONE_SIGNAL, Object_int(0));
  }
  case EXPR_AST: {
    Object res = Expr_eval(stmt->expr_ast, scope);
    Object_free(&res);
    return RunSignal_new(NONE_SIGNAL, Object_int(0));
  }
  case NOOP_AST:
    return RunSignal_new(NONE_SIGNAL, Object_int(0));
  case ASSIGN_AST: {
    Expr *left = stmt->assign_ast.left;
    Object right = Expr_eval(stmt->assign_ast.right, scope);
    if (left->type == ID_AST) {
      Object *var = Scope_find(scope, left->str_ast->val);
      if (!var) {
        printf("Undefine variable '%s'.", left->str_ast->val);
        exit(-1);
      }
      *var = right;
    } else if (left->type == INDEX_AST) {
      Object base = Expr_eval(left->index_ast.expr, scope);
      if (base.tp->tp != LIST_OBJ) {
        printf("Expect a list.");
        exit(-1);
      }
      List *list = base.gcObj->obj;
      Object _index = Expr_eval(left->index_ast.index, scope);
      if (_index.tp->tp != INT_OBJ) {
        printf("Expect an integer index.");
        exit(-1);
      }
      long long index = _index.intObj;
      if (index < 0 || index >= list->size) {
        printf("Index out of range.");
        exit(-1);
      }
      Object_free(&list->val[index]);
      list->val[index] = right;
      Object_free(&base);
      Object_free(&_index);
    } else {
      printf("Expect an l-value.");
      exit(-1);
    }
    return RunSignal_new(NONE_SIGNAL, Object_int(0));
  }
  }
}
