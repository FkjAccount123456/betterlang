#include "vm.h"
#include "b_lex.h"
#include "b_object.h"
#include "builtins.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

ByteCode ByteCode_new(OpCode head) {
  ByteCode code;
  code.head = head;
  return code;
}

void ByteCode_free(ByteCode code) {
  if (code.head == PushS) {
    String_free(code.s);
  }
}

void ByteCodeList_free(ByteCodeList list) {
  for (size_t i = 0; i < list.len; i++) {
    ByteCode_free(list.v[i]);
  }
  SeqFree(list);
}

bool _int_cmp(TokenType op, long long l, long long r) {
  switch (op) {
  case EqToken:
    return l == r;
  case NeToken:
    return l != r;
  case GtToken:
    return l > r;
  case LtToken:
    return l < r;
  case GeToken:
    return l >= r;
  case LeToken:
    return l <= r;
  default:
    printf("Unknown operator at binary operation");
    exit(-1);
  }
}

long long _int_binary(TokenType op, long long l, long long r) {
  switch (op) {
  case AddToken:
    return l + r;
  case SubToken:
    return l - r;
  case MulToken:
    return l * r;
  case DivToken:
    return l / r;
  case ModToken:
    return l % r;
  case LshToken:
    return l >> r;
  case RshToken:
    return l << r;
  case BitAndToken:
    return l & r;
  case BitOrToken:
    return l | r;
  case XorToken:
    return l ^ r;
  default:
    printf("Unknown operator at binary operation");
    exit(-1);
  }
}

bool _float_cmp(TokenType op, double l, double r) {
  switch (op) {
  case EqToken:
    return l == r;
  case NeToken:
    return l != r;
  case GtToken:
    return l > r;
  case LtToken:
    return l < r;
  case GeToken:
    return l >= r;
  case LeToken:
    return l <= r;
  default:
    printf("Unknown operator at binary operation");
    exit(-1);
  }
}

double _float_binary(TokenType op, double l, double r) {
  switch (op) {
  case AddToken:
    return l + r;
  case SubToken:
    return l - r;
  case MulToken:
    return l * r;
  case DivToken:
    return l / r;
  default:
    printf("Unknown operator at binary operation");
    exit(-1);
  }
}

bool _string_cmp(TokenType op, char *l, char *r) {
  switch (op) {
  case EqToken:
    return strcmp(l, r) == 0;
  case NeToken:
    return strcmp(l, r) != 0;
  case GtToken:
    return strcmp(l, r) > 0;
  case LtToken:
    return strcmp(l, r) < 0;
  case GeToken:
    return strcmp(l, r) >= 0;
  case LeToken:
    return strcmp(l, r) <= 0;
  default:
    printf("Unknown operator at binary operation");
    exit(-1);
  }
}

bool _object_tobool(Object v) {
  switch (v.tp->tp) {
  case IntObj:
    return v.intVal;
  case FloatObj:
    return v.floatVal;
  case StringObj:
    return v.stringVal->len;
  case ListObj:
    return v.listVal->len;
  case FuncObj:
  case BuiltinObj:
  case MethodObj:
  case DictObj:
    return true;
  case NoneObj:
    return false;
  }
}

// op不得是&&/||，这应在编译时处理
Object Object_binary(TokenType op, Object l, Object r) {
  if (op == EqToken && l.tp != r.tp)
    return Object_int(0);
  if (op == NeToken && l.tp != r.tp)
    return Object_int(1);
  if (EqToken <= op && op <= LeToken && l.tp == r.tp) {
    if (l.tp == &int_trait)
      return Object_int(_int_cmp(op, l.intVal, r.intVal));
    if (l.tp == &float_trait)
      return Object_int(_float_cmp(op, l.floatVal, r.floatVal));
    if (l.tp == &string_trait)
      return Object_int(_string_cmp(op, l.stringVal->v, r.stringVal->v));
  }
  if ((l.tp == &int_trait || l.tp == &float_trait) &&
      (r.tp == &int_trait || r.tp == &float_trait)) {
    if (l.tp == &float_trait || r.tp == &float_trait) {
      double lv = l.tp == &int_trait ? l.intVal : l.floatVal;
      double rv = r.tp == &int_trait ? r.intVal : r.floatVal;
      return Object_float(_float_binary(op, lv, rv));
    }
    return Object_int(_int_binary(op, l.intVal, r.intVal));
  }
  printf("TypeError: unsupported binary operation");
  exit(-1);
}

Object Object_unary(TokenType op, Object v) {
  if (op == AddToken) {
    if (v.tp == &int_trait)
      return Object_int(v.floatVal);
    if (v.tp == &float_trait)
      return Object_float(v.floatVal);
  } else if (op == SubToken) {
    if (v.tp == &int_trait)
      return Object_int(-v.floatVal);
    if (v.tp == &float_trait)
      return Object_float(-v.floatVal);
  } else if (op == NotToken) {
    return Object_int(!_object_tobool(v));
  } else if (op == InvToken) {
    if (v.tp == &int_trait) {
      return Object_int(~v.intVal);
    }
  }
  printf("TypeError: unsupported unary operation");
  exit(-1);
}

void run(ByteCodeList bytecodelist, size_t reserve) {
  typedef struct Seq(Scope *) ScopeStack;
  typedef struct Seq(size_t) PcStack;

  ByteCode *bytecode = bytecodelist.v;
  size_t pc = 0;
  List *stack = List_new();
  gc_Children_append(gc.gcmap->chs, stack->gcobj);
  Scope *scope = Scope_new(NULL);
  scope->varlist = List_new();
  gc_Children_append(gc.gcmap->chs, scope->varlist->gcobj);
  gc_Children_append(gc.gcmap->chs, scope->gcobj);
  ScopeStack scopestack = SeqNew(ScopeStack);
  PcStack pcstack = SeqNew(PcStack);

  List_append(scope->varlist, Object_builtin(_b_print));
  List_append(scope->varlist, Object_builtin(_b_println));
  List_append(scope->varlist, Object_builtin(_b_getchar));
  List_append(scope->varlist, Object_builtin(_b_putchar));
  List_append(scope->varlist, Object_builtin(_b_ord));
  List_append(scope->varlist, Object_builtin(_b_chr));
  List_append(scope->varlist, Object_builtin(_b_len));
  List_append(scope->varlist, Object_builtin(_b_append));
  List_append(scope->varlist, Object_none());

  while (scope->varlist->len < reserve)
    List_append(scope->varlist, Object_none());

  while (pc < bytecodelist.len && bytecode[pc].head != Exit) {
    ByteCode code = bytecode[pc];
    // ByteCode_print(code);
    // puts("");
    switch (code.head) {
    case Exit:
      break;
    case NoOp:
      break;
    case PushI:
      List_append(stack, Object_int(code.i));
      break;
    case PushF:
      List_append(stack, Object_float(code.f));
      break;
    case PushN:
      List_append(stack, Object_none());
      break;
    case PushS:
      List_append(stack, Object_string(String_copy(code.s)));
      break;
    case Pop: {
      Object obj = stack->v[--stack->len];
      // printf("%llu\n", stack->len);
      if (obj.tp->getter)
        gc_Children_remove(stack->gcobj->chs, obj.tp->getter(&obj));
      break;
    }
    case Binary: {
      Object r = stack->v[--stack->len];
      Object l = stack->v[--stack->len];
      if (l.tp->getter)
        gc_Children_remove(stack->gcobj->chs, l.tp->getter(&l));
      if (r.tp->getter)
        gc_Children_remove(stack->gcobj->chs, r.tp->getter(&r));
      List_append(stack, Object_binary(code.op, l, r));
      break;
    }
    case Unary: {
      Object v = stack->v[--stack->len];
      if (v.tp->getter)
        gc_Children_remove(stack->gcobj->chs, v.tp->getter(&v));
      List_append(stack, Object_unary(code.op, v));
      break;
    }
    case BuildList: {
      List *l = List_new();
      for (size_t i = stack->len - code.l; i < stack->len; i++) {
        Object v = stack->v[i];
        if (v.tp->getter)
          gc_Children_remove(stack->gcobj->chs, v.tp->getter(&v));
        List_append(l, v);
      }
      stack->len -= code.l;
      List_append(stack, Object_list(l));
      break;
    }
    case BuildDict: {
      Dict *d = Dict_new();
      for (size_t i = stack->len - 2 * code.l; i < stack->len; i += 2) {
        Object k = stack->v[i], v = stack->v[i + 1];
        if (k.tp->getter)
          gc_Children_remove(stack->gcobj->chs, k.tp->getter(&k));
        if (v.tp->getter)
          gc_Children_remove(stack->gcobj->chs, v.tp->getter(&v));
        // Object_print(&k, false);
        // Object_print(&v, true);
        Dict_insert(d, k.stringVal->v, v);
      }
      stack->len -= code.l * 2;
      List_append(stack, Object_dict(d));
      break;
    }
    case BuildFunc: {
      List_append(stack, Object_func(Func_new(pc + 2, code.l, scope)));
      break;
    }
    case Index: {
      Object index = stack->v[--stack->len];
      Object base = stack->v[--stack->len];
      if (index.tp->getter)
        gc_Children_remove(stack->gcobj->chs, index.tp->getter(&index));
      if (base.tp->getter)
        gc_Children_remove(stack->gcobj->chs, base.tp->getter(&base));
      if (base.tp == &list_trait && index.tp == &int_trait) {
        List_append(stack, base.listVal->v[index.intVal]);
      } else if (base.tp == &dict_trait && index.tp == &string_trait) {
        Object *v = Dict_find(base.dictVal, index.stringVal->v);
        if (!v) {
          printf("KeyError: '%s'", index.stringVal->v);
          exit(-1);
        }
        if (v->tp == &builtin_trait || v->tp == &func_trait)
          List_append(stack, Object_method(Method_new(base, *v)));
        else
          List_append(stack, *v);
      } else {
        printf("TypeError: unsupported index operation");
        exit(-1);
      }
      break;
    }
    case SetIndex: {
      Object val = stack->v[--stack->len];
      Object index = stack->v[--stack->len];
      Object base = stack->v[--stack->len];
      if (val.tp->getter)
        gc_Children_remove(stack->gcobj->chs, val.tp->getter(&val));
      if (index.tp->getter)
        gc_Children_remove(stack->gcobj->chs, index.tp->getter(&index));
      if (base.tp->getter)
        gc_Children_remove(stack->gcobj->chs, base.tp->getter(&base));
      if (base.tp == &list_trait && index.tp == &int_trait) {
        // 你还是把chat.txt从.gitignore里面去掉吧，不然我每次都找不到、、、
        if (index.intVal >= base.listVal->len) {
          printf("ValueError: index out of range");
          exit(-1);
        }
        Object v = base.listVal->v[index.intVal];
        if (v.tp->getter)
          gc_Children_remove(base.listVal->gcobj->chs, v.tp->getter(&v));
        if (val.tp->getter)
          gc_Children_append(base.listVal->gcobj->chs, val.tp->getter(&val));
        base.listVal->v[index.intVal] = val;
      } else if (base.tp == &dict_trait && index.tp == &string_trait) {
        Object *v = Dict_find(base.dictVal, index.stringVal->v);
        if (!v) {
          Dict_insert(base.dictVal, index.stringVal->v, val);
        } else {
          if (v->tp->getter)
            gc_Children_remove(base.dictVal->gcobj->chs, v->tp->getter(v));
          if (val.tp->getter)
            gc_Children_append(base.dictVal->gcobj->chs, val.tp->getter(&val));
          *v = val;
        }
      } else {
        printf("TypeError: unsupported index operation");
        exit(-1);
      }
      break;
    }
    case Call: {
      size_t arg_cnt = code.l;
      Object funcobj = stack->v[--stack->len];
      if (funcobj.tp->getter)
        gc_Children_remove(stack->gcobj->chs, funcobj.tp->getter(&funcobj));
      List *varlist = List_new();
      while (funcobj.tp == &method_trait) {
        List_append(varlist, funcobj.methodVal->obj);
        funcobj = funcobj.methodVal->func;
      }
      if (funcobj.tp == &func_trait) {
        Func *func = funcobj.funcVal;
        // 感觉不如编译时处理
        // 虽然但是，编译时也没处理
        /* if (arg_cnt != func->param_cnt) {
          printf("TypeError: wrong parameters");
          exit(-1);
        } */
        Scope *new_scope = Scope_new(func->closure);
        new_scope->varlist = varlist;
        gc_Children_append(new_scope->gcobj->chs, new_scope->varlist->gcobj);
        for (size_t i = stack->len - arg_cnt; i < stack->len; i++) {
          Object val = stack->v[i];
          if (val.tp->getter)
            gc_Children_remove(stack->gcobj->chs, val.tp->getter(&val));
          List_append(new_scope->varlist, val);
        }
        while (new_scope->varlist->len < func->reserve) {
          List_append(new_scope->varlist, Object_none());
        }
        stack->len -= arg_cnt;
        SeqAppend(scopestack, scope);
        SeqAppend(pcstack, pc);
        scope = new_scope;
        gc_Children_append(gc.gcmap->chs, scope->gcobj);
        pc = func->pc;
      } else if (funcobj.tp == &builtin_trait) {
        Builtin func = funcobj.builtinVal;
        for (size_t i = stack->len - arg_cnt; i < stack->len; i++) {
          Object val = stack->v[i];
          if (val.tp->getter)
            gc_Children_remove(stack->gcobj->chs, val.tp->getter(&val));
          List_append(varlist, val);
        }
        stack->len -= code.l;
        Object res = func(varlist->len, varlist->v);
        List_append(stack, res);
      } else {
        printf("TypeError: expect a function");
        exit(-1);
      }
      break;
    }
    case Ret: {
      if (!pcstack.len) {
        printf("TypeError: return from program");
        exit(-1);
      }
      gc_Children_remove(gc.gcmap->chs, scope->gcobj);
      pc = SeqPop(pcstack);
      scope = SeqPop(scopestack);
      break;
    }
    case Jmp: {
      pc = code.l;
      break;
    }
    case Jz: {
      Object cond = stack->v[--stack->len];
      if (cond.tp->getter)
        gc_Children_remove(stack->gcobj->chs, cond.tp->getter(&cond));
      if (_object_tobool(cond)) {
        pc = code.l;
      }
      break;
    }
    case Jnz: {
      Object cond = stack->v[--stack->len];
      if (cond.tp->getter)
        gc_Children_remove(stack->gcobj->chs, cond.tp->getter(&cond));
      if (!_object_tobool(cond)) {
        pc = code.l;
      }
      break;
    }
    case JzNoPop: {
      if (_object_tobool(stack->v[--stack->len]))
        pc = code.l;
      break;
    }
    case JnzNoPop: {
      if (!_object_tobool(stack->v[--stack->len]))
        pc = code.l;
      break;
    }
    case LoadV: {
      Scope *curscope = scope;
      for (unsigned int i = 0; i < code.v.scope; i++)
        curscope = curscope->parent;
      List_append(stack, curscope->varlist->v[code.v.pos]);
      break;
    }
    case SetV: {
      Object val = stack->v[--stack->len];
      if (val.tp->getter)
        gc_Children_remove(stack->gcobj->chs, val.tp->getter(&val));
      Scope *curscope = scope;
      for (unsigned int i = 0; i < code.v.scope; i++)
        curscope = curscope->parent;
      Object old = curscope->varlist->v[code.v.pos];
      if (old.tp->getter)
        gc_Children_remove(curscope->varlist->gcobj->chs, old.tp->getter(&old));
      curscope->varlist->v[code.v.pos] = val;
      if (val.tp->getter)
        gc_Children_append(curscope->varlist->gcobj->chs, val.tp->getter(&val));
      break;
    }
    }
    pc++;
  }

  gc_Children_remove(gc.gcmap->chs, scope->gcobj);
  gc_Children_remove(gc.gcmap->chs, stack->gcobj);
  gc_collect();
}

void ByteCode_print(ByteCode code) {
  switch (code.head) {
  case NoOp:
    printf("NoOp");
    break;
  case PushI:
    printf("PushI %lld", code.i);
    break;
  case PushF:
    printf("PushF %f", code.f);
    break;
  case PushN:
    printf("PushN");
    break;
  case PushS:
    printf("PushS %s", code.s->v);
    break;
  case Pop:
    printf("Pop");
    break;
  case Binary:
    printf("Binary %d", code.op);
    break;
  case Unary:
    printf("Unary %d", code.op);
    break;
  case BuildList:
    printf("BuildList %lld", code.l);
    break;
  case BuildDict:
    printf("BuildDict %lld", code.l);
    break;
  case BuildFunc:
    printf("BuildFunc %lld", code.l);
    break;
  case Index:
    printf("Index");
    break;
  case SetIndex:
    printf("SetIndex");
    break;
  case Call:
    printf("Call %lld", code.l);
    break;
  case Ret:
    printf("Ret");
    break;
  case Jmp:
    printf("Jmp %lld", code.l);
    break;
  case Jz:
    printf("Jz %lld", code.l);
    break;
  case Jnz:
    printf("Jnz %lld", code.l);
    break;
  case JzNoPop:
    printf("JzNoPop %lld", code.l);
    break;
  case JnzNoPop:
    printf("JnzNoPop %lld", code.l);
    break;
  case LoadV:
    printf("LoadV %u:%u", code.v.scope, code.v.pos);
    break;
  case SetV:
    printf("SetV %u:%u", code.v.scope, code.v.pos);
    break;
  case Exit:
    printf("Exit");
    break;
  }
}
