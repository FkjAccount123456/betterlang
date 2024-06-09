#ifndef B_FUNC_H
#define B_FUNC_H

#include "b_scope.h"
#include "ast.h"

typedef struct Func {
  Scope *closure;
  Stmt *body;
  String **params;
  size_t nparams;
} Func;

Func *Func_new(Scope *closure, Stmt *body, String **params, size_t nparams);
void *Func_pass(void *_func, size_t rc_offset);
void Func_free(void *_func);

#endif // B_FUNC_H
