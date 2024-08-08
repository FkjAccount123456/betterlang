#include "compile.h"
#include "b_stdlib.h"
#include <stdio.h>
#include <stdlib.h>

// 还是字典树好写啊
VarDict *VarDict_new() {
  VarDict *dict = b_malloc(sizeof(VarDict));
  dict->has = false;
  dict->chs = b_calloc(128, sizeof(VarDict *));
  return dict;
}

void VarDict_insert(VarDict *dict, char *key, unsigned int var) {
  if (!*key) {
    if (dict->has) {
      printf("NameError: redefination of variable");
      exit(-1);
    }
    dict->has = true;
    dict->v = var;
    return;
  }
  if (!dict->chs[*key])
    dict->chs[*key] = VarDict_new();
  VarDict_insert(dict->chs[*key], key + 1, var);
}

unsigned int *VarDict_find(VarDict *dict, char *key) {
  if (!*key) {
    if (dict->has)
      return &dict->v;
    return NULL;
  }
  if (!dict->chs[*key])
    return NULL;
  return VarDict_find(dict->chs[*key], key + 1);
}

void VarDict_free(VarDict *dict) {
  for (size_t i = 0; i < 128; i++)
    if (dict->chs[i])
      VarDict_free(dict->chs[i]);
  b_free(dict);
}

VarScope *VarScope_new(VarScope *parent, VarDict *vars) {
  VarScope *scope = b_malloc(sizeof(VarScope));
  scope->parent = parent;
  scope->vars = vars;
  return scope;
}

VarPos *VarScope_find(VarScope *scope, char *name) {
  VarScope *cur = scope;
  unsigned int scope_cnt = 0;
  while (cur) {
    unsigned int *pos = VarDict_find(cur->vars, name);
    if (pos) {
      // 希望我能想起来回收
      VarPos *p = b_malloc(sizeof(VarPos));
      p->scope = scope_cnt;
      p->pos = *pos;
      return p;
    }
    scope_cnt += 1;
    cur = cur->parent;
  }
  return NULL;
}

void VarScope_free(VarScope *scope) {
  VarDict_free(scope->vars);
  b_free(scope);
}

Compiler *Compiler_new() {
  Compiler *c = b_malloc(sizeof(Compiler));
  c->code = SeqNew(ByteCodeList);
  c->w_begin = SeqNew(PcList);
  c->w_jmps = SeqNew(PcList);
  return c;
}

void Compiler_free(Compiler *c) {
  
  SeqFree(c->w_begin);
  SeqFree(c->w_jmps);
  b_free(c);
}

typedef struct Seq(String *) IdList;

// 虽然但是，还是有变量提升
void _compile_hoist(ASTNode *node, IdList *output) {
  if (node->type == BlockStmt) {
    for (size_t i = 0; i < node->listAST.len; i++)
      _compile_hoist(node->listAST.v[i], output);
  } else if (node->type == VarDeclStmt) {
    for (size_t i = 0; i < node->listAST.len; i += 2)
      SeqAppend(*output, node->listAST.v[i]->stringAST);
  } else if (node->type == FuncDefStmt) {
    SeqAppend(*output, node->listAST.v[0]->stringAST);
  }
}

VarPos _compile_find_var(Compiler *c, char *name) {
  VarPos *var = VarScope_find(c->scope, name);
  if (!var) {
    printf("NameError: undefined variable '%s'", name);
    exit(-1);
  }
  VarPos r = *var;
  b_free(var);
  return r;
}

void compile(Compiler *c, ASTNode *node) {
  // puts("compile");
  // ASTNode_print(node, 0);
  // puts("");
  switch (node->type) {
  // Stmts的编译
  case NoOpStmt:
    break;
  case BlockStmt:
    for (size_t i = 0; i < node->listAST.len; i++)
      compile(c, node->listAST.v[i]);
    break;
  case ExprStmt:
    compile(c, node->nodeAST);
    SeqAppend(c->code, ByteCode_new(Pop));
    break;
  case AssignStmt: {
    ASTNode *l = node->listAST.v[0];
    ASTNode *r = node->listAST.v[1];
    if (l->type == VarExpr) {
      compile(c, r);
      ByteCode code = ByteCode_new(SetV);
      VarPos var = _compile_find_var(c, l->stringAST->v);
      code.v.pos = var.pos;
      code.v.scope = var.scope;
      SeqAppend(c->code, code);
    } else if (l->type == IndexExpr) {
      compile(c, l->listAST.v[0]);
      compile(c, l->listAST.v[1]);
      compile(c, r);
      SeqAppend(c->code, ByteCode_new(SetIndex));
    } else {
      printf("TypeError: Wrong l-value at assign-stmt");
      exit(-1);
    }
    break;
  }
  case IfStmt: {
    // 和Rain1.1一样照搬的betterlang v1
    // 不得不说比上一版还是优美了很多的
    // 上一版一个If的编译得占一屏
    PcList jmps = SeqNew(PcList);
    size_t i;
    size_t jnz = 0;
    for (i = 0; i + 1 < node->listAST.len; i += 2) {
      compile(c, node->listAST.v[i]);
      jnz = c->code.len;
      SeqAppend(c->code, ByteCode_new(Jnz));
      compile(c, node->listAST.v[i + 1]);
      SeqAppend(jmps, c->code.len);
      SeqAppend(c->code, ByteCode_new(Jmp));
      c->code.v[jnz].l = c->code.len - 1;
    }
    if (node->listAST.len % 2) {
      compile(c, node->listAST.v[node->listAST.len - 1]);
    }
    for (size_t i = 0; i < jmps.len; i++)
      c->code.v[jmps.v[i]].l = c->code.len - 1;
    SeqFree(jmps);
    break;
  }
  case WhileStmt: {
    SeqAppend(c->w_jmps, 0);
    SeqAppend(c->w_begin, c->code.len - 1);
    compile(c, node->listAST.v[0]);
    SeqAppend(c->w_jmps, c->code.len);
    SeqAppend(c->code, ByteCode_new(Jnz));
    compile(c, node->listAST.v[1]);
    SeqAppend(c->code, ByteCode_init(Jmp, l, c->w_begin.v[--c->w_begin.len]));
    while (c->w_jmps.len) {
      c->w_jmps.len--;
      if (c->w_jmps.v[c->w_jmps.len] == 0)
        break;
      c->code.v[c->w_jmps.v[c->w_jmps.len]].l = c->code.len - 1;
    }
    break;
  }
  case VarDeclStmt: {
    for (size_t i = 0; i < node->listAST.len; i += 2) {
      compile(c, node->listAST.v[i + 1]);
      ByteCode code = ByteCode_new(SetV);
      VarPos var = _compile_find_var(c, node->listAST.v[i]->stringAST->v);
      code.v.pos = var.pos;
      code.v.scope = var.scope;
      SeqAppend(c->code, code);
    }
    break;
  }
  case FuncDefStmt: {
    IdList new_vars = SeqNew(IdList);
    // 、、、
    for (size_t i = 0; i < node->listAST.v[1]->listAST.len; i++)
      SeqAppend(new_vars, node->listAST.v[1]->listAST.v[i]->stringAST);
    _compile_hoist(node->listAST.v[2], &new_vars);
    SeqAppend(c->code, ByteCode_init(BuildFunc, l, new_vars.len));
    VarPos var = _compile_find_var(c, node->listAST.v[0]->stringAST->v);
    ByteCode code = ByteCode_new(SetV);
    code.v.pos = var.pos;
    code.v.scope = var.scope;
    SeqAppend(c->code, code);
    size_t pos = c->code.len;
    SeqAppend(c->code, ByteCode_new(Jmp));
    VarDict *new_scope = VarDict_new();
    for (size_t i = 0; i < new_vars.len; i++)
      VarDict_insert(new_scope, new_vars.v[i]->v, i);
    c->scope = VarScope_new(c->scope, new_scope);
    compile(c, node->listAST.v[2]);
    c->code.v[pos].l = c->code.len - 1;
    VarScope *fn_scope = c->scope;
    c->scope = c->scope->parent;
    VarScope_free(fn_scope);
    break;
  }
  case ReturnStmt: {
    compile(c, node->nodeAST);
    SeqAppend(c->code, ByteCode_new(Ret));
    break;
  }
  case BreakStmt: {
    SeqAppend(c->w_jmps, c->code.len);
    SeqAppend(c->code, ByteCode_new(Jmp));
    break;
  }
  case ContinueStmt: {
    SeqAppend(c->code, ByteCode_init(Jmp, l, c->w_begin.v[c->w_begin.len - 1]));
    break;
  }
  // Exprs的编译
  case IntExpr:
    SeqAppend(c->code, ByteCode_init(PushI, i, node->intAST));
    break;
  case FloatExpr:
    SeqAppend(c->code, ByteCode_init(PushF, f, node->floatAST));
    break;
  case StringExpr:
    SeqAppend(c->code, ByteCode_init(PushS, s, node->stringAST));
    break;
  case NoneExpr:
    SeqAppend(c->code, ByteCode_new(PushN));
    break;
  case VarExpr: {
    VarPos var = _compile_find_var(c, node->stringAST->v);
    ByteCode code = ByteCode_new(LoadV);
    code.v.scope = var.scope;
    code.v.pos = var.pos;
    SeqAppend(c->code, code);
    break;
  }
  case ExprExpr: {
    // 你说得对，但是ExprExpr指的是后缀表达式，都转换好了的
    for (size_t i = 0; i < node->listAST.len; i++) {
      if (node->listAST.v[i]->type == OpTerminal) {
        SeqAppend(c->code, ByteCode_init(Binary, op, node->listAST.v[i]->opAST));
      } else {
        compile(c, node->listAST.v[i]);
      }
    }
    break;
  }
  case AndExpr: {
    // 其实短路的编译和if没啥区别
    PcList jnz_list = SeqNew(PcList);
    for (size_t i = 0; i < node->listAST.len; i++) {
      if (i != 0) {
        SeqAppend(c->code, ByteCode_new(Pop));
      }
      compile(c, node->listAST.v[i]);
      SeqAppend(jnz_list, node->listAST.len);
      SeqAppend(c->code, ByteCode_new(JnzNoPop));
    }
    for (size_t i = 0; i < jnz_list.len; i++) {
      c->code.v[jnz_list.v[i]].l = c->code.len - 1;
    }
    SeqFree(jnz_list);
    break;
  }
  case OrExpr: {
    PcList jnz_list = SeqNew(PcList);
    for (size_t i = 0; i < node->listAST.len; i++) {
      if (i != 0) {
        SeqAppend(c->code, ByteCode_new(Pop));
      }
      compile(c, node->listAST.v[i]);
      SeqAppend(jnz_list, node->listAST.len);
      SeqAppend(c->code, ByteCode_new(JzNoPop));
    }
    for (size_t i = 0; i < jnz_list.len; i++) {
      c->code.v[jnz_list.v[i]].l = c->code.len - 1;
    }
    SeqFree(jnz_list);
    break;
  }
  case UnaryExpr: {
    compile(c, node->listAST.v[1]);
    SeqAppend(c->code, ByteCode_init(Unary, op, node->opAST));
    break;
  }
  case TernaryExpr: {
    compile(c, node->listAST.v[0]);
    size_t pos = c->code.len;
    SeqAppend(c->code, ByteCode_new(Jnz));
    compile(c, node->listAST.v[1]);
    size_t jmp = c->code.len;
    c->code.v[pos].l = c->code.len;
    SeqAppend(c->code, ByteCode_new(Jmp));
    compile(c, node->listAST.v[2]);
    c->code.v[jmp].l = c->code.len - 1;
    break;
  }
  case BuildListExpr: {
    for (size_t i = 0; i < node->listAST.len; i++)
      compile(c, node->listAST.v[i]);
    SeqAppend(c->code, ByteCode_init(BuildList, l, node->listAST.len));
    break;
  }
  case BuildDictExpr: {
    for (size_t i = 0; i < node->listAST.len; i += 2) {
      SeqAppend(c->code, ByteCode_init(PushS, s, node->listAST.v[i]->stringAST));
      compile(c, node->listAST.v[i+1]);
    }
    SeqAppend(c->code, ByteCode_init(BuildDict, l, node->listAST.len / 2));
    break;
  }
  case LambdaExpr: {
    IdList new_vars = SeqNew(IdList);
    // 、、、
    for (size_t i = 0; i < node->listAST.v[0]->listAST.len; i++)
      SeqAppend(new_vars, node->listAST.v[0]->listAST.v[i]->stringAST);
    _compile_hoist(node->listAST.v[1], &new_vars);
    SeqAppend(c->code, ByteCode_init(BuildFunc, l, new_vars.len));
    SeqAppend(c->code, ByteCode_new(NoOp));
    size_t pos = c->code.len;
    SeqAppend(c->code, ByteCode_new(Jmp));
    VarDict *new_scope = VarDict_new();
    for (size_t i = 0; i < new_vars.len; i++)
      VarDict_insert(new_scope, new_vars.v[i]->v, i);
    c->scope = VarScope_new(c->scope, new_scope);
    compile(c, node->listAST.v[1]);
    c->code.v[pos].l = c->code.len - 1;
    VarScope *fn_scope = c->scope;
    c->scope = c->scope->parent;
    VarScope_free(fn_scope);
    break;
  }
  case CallExpr: {
    for (size_t i = 1; i < node->listAST.len; i++) {
      compile(c, node->listAST.v[i]);
    }
    compile(c, node->listAST.v[0]);
    SeqAppend(c->code, ByteCode_init(Call, l, node->listAST.len - 1));
    break;
  }
  case AttrExpr: {
    compile(c, node->listAST.v[0]);
    SeqAppend(c->code, ByteCode_init(PushS, s, node->listAST.v[1]->stringAST));
    SeqAppend(c->code, ByteCode_new(Index));
    break;
  }
  case IndexExpr: {
    compile(c, node->listAST.v[0]);
    compile(c, node->listAST.v[1]);
    SeqAppend(c->code, ByteCode_new(Index));
    break;
  }
  case OpTerminal: {
    printf("CoreError: unexpected OpTerminal AST");
    exit(-1);
  }
  }
}

size_t compile_program(Compiler *c, ASTNode *node) {
  IdList globals = SeqNew(IdList);
  SeqAppend(globals, String_new("print"));
  SeqAppend(globals, String_new("println"));
  SeqAppend(globals, String_new("getchar"));
  SeqAppend(globals, String_new("putchar"));
  SeqAppend(globals, String_new("ord"));
  SeqAppend(globals, String_new("chr"));
  SeqAppend(globals, String_new("len"));
  SeqAppend(globals, String_new("append"));
  SeqAppend(globals, String_new("none"));

  _compile_hoist(node, &globals);

  VarDict *vars = VarDict_new();
  for (size_t i = 0; i < globals.len; i++) {
    VarDict_insert(vars, globals.v[i]->v, i);
  }
  c->scope = VarScope_new(NULL, vars);
  compile(c, node);

  SeqFree(globals);
  return globals.len;
}
