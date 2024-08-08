#pragma once

#include <stdbool.h>
#include <stddef.h>
#include "vm.h"
#include "b_ast.h"

typedef struct VarPos {
  unsigned int scope, pos;
} VarPos;

typedef struct VarDict {
  unsigned int v;
  bool has;
  struct VarDict **chs;
} VarDict;

VarDict *VarDict_new();
void VarDict_insert(VarDict *dict, char *key, unsigned int var);
unsigned int *VarDict_find(VarDict *dict, char *key);
void VarDict_free(VarDict *dict);

typedef struct VarScope {
  VarDict *vars;
  struct VarScope *parent;
} VarScope;

VarScope *VarScope_new(VarScope *parent, VarDict *vars);
VarPos *VarScope_find(VarScope *scope, char *name);
void VarScope_free(VarScope *scope);

typedef struct Seq(size_t) PcList;

typedef struct Compiler {
  VarScope *scope;
  ByteCodeList code;
  PcList w_jmps;
  PcList w_begin;
} Compiler;

Compiler *Compiler_new();
void Compiler_free(Compiler *c);
size_t compile_program(Compiler *c, ASTNode *node);
