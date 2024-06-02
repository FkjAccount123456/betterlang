#ifndef B_SCOPE_H
#define B_SCOPE_H

#include "gc.h"
#include "b_dict.h"

typedef struct Scope {
  struct Scope *parent;
  Dict *vars;
  size_t rc;
} Scope;

Scope *Scope_new(Scope *parent);
void Scope_free(Scope *scope);
Scope *Scope_pass(Scope *base);
Object *Scope_find(Scope *scope, char *name);
void Scope_define(Scope *scope, char *name, Object obj);

#endif // B_SCOPE_H
