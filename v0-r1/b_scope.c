#include "b_scope.h"

Scope *Scope_new(Scope *parent) {
  Scope *scope = (Scope *)malloc(sizeof(Scope));
  if (scope == NULL) {
    printf("Failed to malloc.");
    exit(-1);
  }
  scope->vars = Dict_new();
  scope->parent = parent;
  scope->rc = 1;
  return scope;
}

void Scope_free(Scope *scope) {
  scope->rc--;
  if (scope->parent)
    Scope_free(scope->parent);
  if (!scope->rc) {
    Dict_free(scope->vars);
    free(scope);
  }
}

Scope *Scope_pass(Scope *base) {
  base->rc++;
  if (scope->parent) {
    Scope_pass(scope->parent);
  }
  return base;
}

Object *Scope_find(Scope *scope, char *name) {
  Object *tmp = Dict_find(scope->vars, name);
  if (tmp)
    return tmp;
  else if (scope->parent)
    return Scope_find(scope->parent, name);
  else
    return NULL;
}

void Scope_define(Scope *scope, char *name, Object obj) {
  Dict_insert(scope->vars, name, obj);
}
