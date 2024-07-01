#include "vm.h"
#include "gc.h"

VMCode VMCode_new(ByteCode head) {
  VMCode vc;
  vc.head = head;
  return vc;
}

void VMCode_print(VMCode code) {
  switch (code.head) {
  case EXIT:
    printf("EXIT\n");
    break;
  case PUSH_I:
    printf("PUSH_I %lld\n", code.i);
    break;
  case PUSH_F:
    printf("PUSH_F %lf\n", code.f);
    break;
  case PUSH_N:
    printf("PUSH_N\n");
    break;
  case PUSH_FN:
    printf("PUSH_FN\n");
    break;
  case LOAD_V:
    printf("LOAD_V %llu %llu\n", code.l >> 32, code.l % (1ull << 32));
    break;
  case SET_V:
    printf("SET_V %llu %llu\n", code.l >> 32, code.l % (1ull << 32));
    break;
  case ADD_V:
    printf("ADD_V\n");
    break;
  case POP:
    printf("POP\n");
    break;
  case BUILD_LIST:
    printf("BUILD_LIST %llu\n", code.l);
    break;
  case ADD:
    printf("ADD\n");
    break;
  case SUB:
    printf("SUB\n");
    break;
  case MUL:
    printf("MUL\n");
    break;
  case DIV:
    printf("DIV\n");
    break;
  case EQ:
    printf("EQ\n");
    break;
  case NE:
    printf("NE\n");
    break;
  case GT:
    printf("GT\n");
    break;
  case LT:
    printf("LT\n");
    break;
  case GE:
    printf("GE\n");
    break;
  case LE:
    printf("LE\n");
    break;
  case AND:
    printf("AND\n");
    break;
  case OR:
    printf("OR\n");
    break;
  case XOR:
    printf("XOR\n");
    break;
  case POS:
    printf("POS\n");
    break;
  case NEG:
    printf("NEG\n");
    break;
  case NOT:
    printf("NOT\n");
    break;
  case JMP:
    printf("JMP %llu\n", code.l);
    break;
  case JZ:
    printf("JZ %llu\n", code.l);
    break;
  case JNZ:
    printf("JNZ %llu\n", code.l);
    break;
  case CALL:
    printf("CALL %llu\n", code.l);
    break;
  case RET:
    printf("RET\n");
    break;
  case NTH:
    printf("NTH\n");
    break;
  case SET_NTH:
    printf("SET_NTH\n");
    break;
  default:
    printf("Unknown: %d\n", code.head);
  }
}

void VMCode_run(VMCode *code) {
  size_t pc = 0;
  List *stack = List_new();
  GC_active_add(stack->gc_base);
  VMFrame *frame = VMFrame_new(NULL);
  GC_active_add(frame->gc_base);
  NewSeq(VMFrame *, frame_stack);
  NewSeq(size_t, pc_stack);

  while (code[pc].head != EXIT) {
    printf("%lld: %d stack.size: %llu\n", pc, code[pc].head, stack->size);
    ByteCode head = code[pc].head;
    switch (head) {
    case PUSH_I:
      List_append(stack, Object_int(code[pc].i));
      break;
    case PUSH_F:
      List_append(stack, Object_float(code[pc].f));
      break;
    case PUSH_S:
      {
        List_append(stack, Object_String(String_new(code[pc].s->val)));
        // printf("%llx %llx %llu\n", code, stack->items, stack->size);
        // stack->items[0] = Object_int(0);
        // Object tmp = Object_String(String_new(code[pc].s->val));
        // printf("%llx %llx\n", stack->items, code);
        // GC_obj_add_ch(stack->gc_base, Object_get_gcval(tmp));
        // stack->items[stack->size++] = Object_none();
        // printf("PUSH_S %s %llx base %llx\n", code[pc].s->val,
        // stack->items[stack->size - 1].s, code[pc].s);
        break;
      }
    case PUSH_FN:
      List_append(stack, Object_Func(Func_new(frame, pc + 2)));
      break;
    case LOAD_V:
      {
        unsigned int fcnt = code[pc].l >> 32, vcnt = code[pc].l % (1ull << 32);
        printf("LOAD_V %llu %u %u\n", code[pc].l, fcnt, vcnt);
        VMFrame *curf = frame;
        for (; fcnt; curf = curf->parent, fcnt--)
          ;
        List_append(stack, curf->varlist->items[vcnt]);
        // puts("LOAD_V");
        break;
      }
    case SET_V:
      {
        unsigned int fcnt = code[pc].l >> 32, vcnt = code[pc].l % (1ull << 32);
        VMFrame *curf = frame;
        for (; fcnt; curf = curf->parent, fcnt--)
          ;
        curf->varlist->items[vcnt] = stack->items[--stack->size];
        break;
      }
    case ADD_V:
      {
        Object obj = stack->items[--stack->size];
        // puts("Hello!");
        Object_disconnect(stack->gc_base, obj);
        List_append(frame->varlist, obj);
        break;
      }
    case POP:
      {
        Object tmp = stack->items[--stack->size];
        printf("POP: ");
        Object_print(tmp);
        puts("");
        Object_disconnect(stack->gc_base, tmp);
        break;
      }
    case BUILD_LIST:
      {
        List *list = List_new();
        for (size_t i = stack->size - code[pc].l; i < stack->size; i++) {
          List_append(list, stack->items[i]);
          Object_disconnect(stack->gc_base, stack->items[i]);
        }
        stack->size -= code[pc].i;
        List_append(stack, Object_List(list));
        break;
      }
    case ADD:
    case SUB:
    case MUL:
    case DIV:
    case EQ:
    case NE:
    case GT:
    case LT:
    case GE:
    case LE:
    case AND:
    case OR:
    case XOR:
      {
        Object b = stack->items[--stack->size];
        Object a = stack->items[--stack->size];
        Object res = Object_binaryop(head, a, b);
        Object_disconnect(stack->gc_base, a);
        Object_disconnect(stack->gc_base, b);
        List_append(stack, res);
        break;
      }
    case POS:
    case NEG:
    case NOT:
      {
        Object a = stack->items[--stack->size];
        Object res = Object_unaryop(head, a);
        Object_disconnect(stack->gc_base, a);
        List_append(stack, res);
        break;
      }
    case JMP:
      pc = code[pc].l;
      break;
    case JZ:
      {
        Object cond = stack->items[--stack->size];
        if (Object_to_bool(cond))
          pc = code[pc].l;
        Object_disconnect(stack->gc_base, cond);
        break;
      }
    case JNZ:
      {
        Object cond = stack->items[--stack->size];
        if (!Object_to_bool(cond))
          pc = code[pc].l;
        Object_disconnect(stack->gc_base, cond);
        break;
      }
    case CALL:
      {
        printf("CALL %llu", code[pc].l);
        Object func = stack->items[stack->size - code[pc].l - 1];
        if (func.tp->tp == FUNC_OBJ) {
          Func *fn = func.fn;
          SeqAppend(VMFrame *, frame_stack, frame);
          frame = malloc(sizeof(VMFrame));
          *frame = *fn->frame;
          frame->gc_base = GC_objs_add(gc.G_bases[frame->gc_base]);
          GC_active_add(frame->gc_base);
          GC_obj_add_ch(frame->gc_base, fn->frame->gc_base);
          GC_obj_add_ch(fn->frame->gc_base, frame->gc_base);
          SeqAppend(size_t, pc_stack, pc);
          pc = fn->pc;
          stack->size -= code[pc].l;
        } else if (func.tp->tp == BUILTIN_OBJ) {
          Builtin fn = func.builtin;
          stack->size -= code[pc].l;
          Object res = fn(code[pc].l, stack->items + stack->size);
          stack->size--;
          List_append(stack, res);
        } else {
          printf("Expect a function to be called");
        }
        Object_disconnect(stack->gc_base, func);
        break;
      }
    case RET:
      {
        pc = SeqPop(pc_stack);
        GC_active_remove(frame->gc_base);
        frame = SeqPop(frame_stack);
        stack->items[stack->size - 2] = stack->items[stack->size - 1];
        stack->size--;
        break;
      }
    case NTH:
      {
        Object index = stack->items[--stack->size];
        Object base = stack->items[--stack->size];
        if (base.tp->tp == DICT_OBJ && index.tp->tp == STR_OBJ) {
          String *str_index = index.s;
          Object *r = Dict_find(base.d, str_index->val);
          if (!r) {
            printf("Undefined attr '%s'", str_index->val);
            exit(-1);
          }
          List_append(stack, *r);
        } else if (base.tp->tp == STR_OBJ && index.tp->tp == INT_OBJ) {
          String *str_base = base.s;
          List_append(stack, Object_int(str_base->val[index.i]));
        } else if (base.tp->tp == LIST_OBJ && index.tp->tp == INT_OBJ) {
          List *list_base = base.l;
          List_append(stack, list_base->items[index.i]);
        } else {
          printf("Wrong base or index type");
          exit(-1);
        }
        Object_disconnect(stack->gc_base, index);
        Object_disconnect(stack->gc_base, base);
        break;
      }
    case SET_NTH:
      {
        Object index = stack->items[--stack->size];
        Object base = stack->items[--stack->size];
        Object val = stack->items[--stack->size];
        if (base.tp->tp == DICT_OBJ && index.tp->tp == STR_OBJ) {
          String *str_index = index.s;
          Dict_insert(base.d, str_index->val, val);
        } else if (base.tp->tp == STR_OBJ && index.tp->tp == INT_OBJ) {
          String *str_base = base.s;
          if (val.tp != INT_OBJ) {
            printf("Wrong value type");
            exit(-1);
          }
          str_base->val[index.i] = val.i;
        } else if (base.tp->tp == LIST_OBJ && index.tp->tp == INT_OBJ) {
          List *list_base = base.l;
          Object_disconnect(base.l->gc_base, list_base->items[index.i]);
          list_base->items[index.i] = val;
        } else {
          printf("Wrong base or index type");
          exit(-1);
        }
        Object_disconnect(stack->gc_base, base);
        Object_disconnect(stack->gc_base, index);
        Object_disconnect(stack->gc_base, val);
        break;
      }
    default:
      printf("Unknown bytecode '%d'.", head);
      exit(-1);
    }
    // if (pc == 3) {
    //   free(code);
    //   exit(0);
    // }
    pc++;
    puts("OK");
  }
  GC_active_remove(stack->gc_base);
  GC_active_remove(frame->gc_base);
  FreeSeq(frame_stack);
}

long long _int_binaryop(ByteCode op, long long a, long long b) {
  switch (op) {
  case ADD:
    return a + b;
  case SUB:
    return a - b;
  case MUL:
    return a * b;
  case DIV:
    return a / b;
  case EQ:
    return a == b;
  case NE:
    return a != b;
  case GT:
    return a > b;
  case LT:
    return a < b;
  case GE:
    return a >= b;
  case LE:
    return a <= b;
  case AND:
    return a && b;
  case OR:
    return a || b;
  case XOR:
    return (bool)a ^ (bool)b;
  default:
    printf("Unknown operator.");
    exit(-1);
  }
}

double _float_binaryop(ByteCode op, double a, double b) {
  switch (op) {
  case ADD:
    return a + b;
  case SUB:
    return a - b;
  case MUL:
    return a * b;
  case DIV:
    return a / b;
  case EQ:
    return a == b;
  case NE:
    return a != b;
  case GT:
    return a > b;
  case LT:
    return a < b;
  case GE:
    return a >= b;
  case LE:
    return a <= b;
  case AND:
    return a && b;
  case OR:
    return a || b;
  case XOR:
    return (bool)a ^ (bool)b;
  default:
    printf("Unknown operator.");
    exit(-1);
  }
}

Object Object_binaryop(ByteCode op, Object a, Object b) {
  if ((a.tp->tp == INT_OBJ || a.tp->tp == FLOAT_OBJ) &&
      (b.tp->tp == INT_OBJ || b.tp->tp == FLOAT_OBJ)) {
    if (a.tp->tp == FLOAT_OBJ || b.tp->tp == FLOAT_OBJ) {
      return Object_float(_float_binaryop(op, a.f, b.f));
    }
    return Object_int(_int_binaryop(op, a.i, b.i));
  }
  if (op == AND)
    return Object_int(Object_to_bool(a) && Object_to_bool(b));
  if (op == OR)
    return Object_int(Object_to_bool(a) || Object_to_bool(b));
  if (op == XOR)
    return Object_int(Object_to_bool(a) ^ Object_to_bool(b));
  if (op == EQ && a.tp != b.tp)
    return Object_int(0);
  if (op == NE && a.tp != b.tp)
    return Object_int(1);
  if (op == EQ)
    return Object_int(Object_cmp(a, b) == 0);
  if (op == NE)
    return Object_int(Object_cmp(a, b) != 0);
  if (op == GT)
    return Object_int(Object_cmp(a, b) > 0);
  if (op == LT)
    return Object_int(Object_cmp(a, b) < 0);
  if (op == GE)
    return Object_int(Object_cmp(a, b) >= 0);
  if (op == LE)
    return Object_int(Object_cmp(a, b) <= 0);
  printf("Unsupported binary operation.");
  exit(-1);
}

long long _int_unaryop(ByteCode op, long long a) {
  switch (op) {
  case POS:
    return +a;
  case NEG:
    return -a;
  case NOT:
    return !a;
  default:
    printf("Unknown operator.");
    exit(-1);
  }
}

double _float_unaryop(ByteCode op, double a) {
  switch (op) {
  case POS:
    return +a;
  case NEG:
    return -a;
  default:
    printf("Unknown operator.");
    exit(-1);
  }
}

Object Object_unaryop(ByteCode op, Object a) {
  if (op == NOT && a.tp->tp == FLOAT_OBJ)
    return Object_int(!a.f);
  if (a.tp->tp == INT_OBJ)
    return Object_int(_int_unaryop(op, a.i));
  if (a.tp->tp == FLOAT_OBJ)
    return Object_float(_float_unaryop(op, a.f));
  if (op == NOT)
    return Object_int(!Object_to_bool(a));
  printf("Unsupported unary operation.");
  exit(-1);
}
