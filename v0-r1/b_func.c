#include "b_func.h"

Func *Func_new(Scope *closure, Stmt *body, String **params, size_t nparams) {
  Func *func = (Func *)malloc(sizeof(Func));
  func->nparams = nparams;
  func->params = params;
  func->body = body;
  func->closure = closure;
  return func;
}

void *Func_pass(void *_func, size_t rc_offset) {
  Func *func = (Func *)_func;
  if (func->closure) {
    func->closure = Scope_pass(func->closure);
  }
  return func;
}

void Func_free(void *_func) {
  Func *func = (Func *)_func;
  Scope_free(func->closure);
  free(func);
}
