#include "vm.h"
#include "gc.h"

VMCode VMCode_new(ByteCode head) {
  VMCode vc;
  vc.head = head;
  return vc;
}

void VMCode_free(VMCode *code) {
  if (code->head == PUSH_S) {
    String_free(code->s);
  }
}

void VMCode_run(VMCode *code) {
  size_t pc;
  Object stack = Object_gc(List_new());
  List *stack_val = stack.gc_obj->ptr->ptr;
  Object frame = VMFrame_new(NULL);
  VMFrame *frame_val = frame.gc_obj->ptr->ptr;
  NewSeq(Object, frame_stack);
  NewSeq(size_t, pc_stack);

  while (code[pc].head != EXIT) {
    ByteCode head = code[pc].head;
    switch (head) {
      case PUSH_I:
        List_append(stack, Object_int(code[pc].i));
        break;
      case PUSH_F:
        List_append(stack, Object_float(code[pc].f));
        break;
      case PUSH_S:
        List_append(stack, Object_gc(String_copy(code[pc].s)));
        break;
      case LOAD_V:
      {
        unsigned int fcnt = code[pc].l >> 32,
                     vcnt = code[pc].l % (1ull << 32);
        VMFrame *curf = frame_val;
        for (;;curf = curf->parent, fcnt--);
        List_append(stack, curf->varlist->items[vcnt]);
        break;
      }
      case SET_V:
      {
        unsigned int fcnt = code[pc].l >> 32,
                     vcnt = code[pc].l % (1ull << 32);
        VMFrame *curf = frame_val;
        for (;;curf = curf->parent, fcnt--);
        curf->varlist->items[vcnt] = stack_val->items[--stack_val->size];
        break;
      }
      case POP:
        Object tmp = stack_val->items[--stack_val->size];
        Object_disconnect(stack.gc_obj, tmp);
        break;
      case BUILD_LIST:
      {
        Object list = Object_gc(List_new());
        for (size_t i = stack_val->size - code[pc].l; i < stack_val->size; i++) {
          List_append(list, stack_val->items[i]);
          Object_disconnect(stack.gc_obj, stack_val->items[i]);
        }
        stack_val->size -= code[pc].i;
        List_append(stack, list);
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
        Object b = stack_val->items[--stack_val->size];
        Object a = stack_val->items[--stack_val->size];
        Object res = Object_binaryop(head, a, b);
        Object_disconnect(stack.gc_obj, a);
        Object_disconnect(stack.gc_obj, b);
        List_append(stack, res);
        break;
      }
      case POS:
      case NEG:
      case NOT:
      {
        Object a = stack_val->items[--stack_val->size];
        Object res = Object_unaryop(head, a);
        Object_disconnect(stack.gc_obj, a);
        List_append(stack, res);
        break;
      }
      case JMP:
        pc = code[pc].l;
        break;
      case JZ:
        Object cond = stack_val->items[--stack_val->size];
        if (Object_to_bool(cond))
          pc = code[pc].l;
        Object_disconnect(stack.gc_obj, cond);
        break;
      case JNZ:
      {
        Object cond = stack_val->items[--stack_val->size];
        if (!Object_to_bool(cond))
          pc = code[pc].l;
        Object_disconnect(stack.gc_obj, cond);
        break;
      }
      case CALL:
      {
        Object func = stack_val->items[stack_val->size--];
        for (size_t i = stack_val->size - code[pc].l; i < stack_val->size; i++) {
          stack_val->items[i - 1] = stack_val->items[i];
        }
        stack_val->size--;
        if (func.tp == FUNC_OBJ) {
          Func *fn = func.gc_obj->ptr->ptr;
          SeqAppend(Object, frame_stack, frame);
          frame = Object_pass(fn->frame);
          SeqAppend(size_t, pc_stack, pc);
          pc = fn->pc;
        } else if (func.tp == BUILTIN_OBJ) {
          Builtin fn = func.builtin_obj;
          Object res = fn(code[pc].l, stack_val->items - code[pc].l);
          stack_val->size -= code[pc].l;
          List_append(stack, res);
        } else {
          printf("Expect a function to be called");
        }
        break;
      }
      case RET:
      {
        pc = SeqPop(pc_stack);
        Object_free(frame);
        frame = SeqPop(frame_stack);
        break;
      }
      case NTH:
      {
        Object index = stack_val->items[--stack_val->size];
        Object base = stack_val->items[--stack_val->size];
        if (base.tp == DICT_OBJ && index.tp == STR_OBJ) {
          String *str_index = index.gc_obj->ptr->ptr;
          List_append(stack, Dict_find(base.gc_obj->ptr->ptr, str_index->val));
        } else if (base.tp == STR_OBJ && index.tp == INT_OBJ) {
          String *str_base = base.gc_obj->ptr->ptr;
          List_append(stack, Object_int(str_base->val[index.int_obj]));
        } else if (base.tp == LIST_OBJ && index.tp == INT_OBJ) {
          List *list_base = base.gc_obj->ptr->ptr;
          List_append(stack, list_base->items[index.int_obj]);
        } else {
          printf("Wrong base or index type");
          exit(-1);
        }
        Object_disconnect(stack.gc_obj, index);
        Object_disconnect(stack.gc_obj, base);
        break;
      }
      case SET_NTH:
      {
        Object index = stack_val->items[--stack_val->size];
        Object base = stack_val->items[--stack_val->size];
        Object val = stack_val->items[--stack_val->size];
        if (base.tp == DICT_OBJ && index.tp == STR_OBJ) {
          String *str_index = index.gc_obj->ptr->ptr;
          Dict_insert(base, str_index->val, val);
        } else if (base.tp == STR_OBJ && index.tp == INT_OBJ) {
          String *str_base = base.gc_obj->ptr->ptr;
          if (val.tp != INT_OBJ) {
            printf("Wrong value type");
            exit(-1);
          }
          str_base->val[index.int_obj] = val.int_obj;
        } else if (base.tp == LIST_OBJ && index.tp == INT_OBJ) {
          List *list_base = base.gc_obj->ptr->ptr;
          Object_disconnect(base.gc_obj, list_base->items[index.int_obj]);
          list_base->items[index.int_obj] = val;
        } else {
          printf("Wrong base or index type");
          exit(-1);
        }
        Object_disconnect(stack.gc_obj, base);
        Object_disconnect(stack.gc_obj, index);
        Object_disconnect(stack.gc_obj, val);
        break;
      }
      default:
        printf("Unknown bytecode '%d'.", head);
        exit(-1);
    }
  }
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
  if ((a.tp == INT_OBJ || a.tp == FLOAT_OBJ) &&
    (b.tp == INT_OBJ || b.tp == FLOAT_OBJ)) {
    if (a.tp == FLOAT_OBJ || b.tp == FLOAT_OBJ) {
      return Object_float(_float_binaryop(op, a.float_obj, b.float_obj));
    }
    return Object_int(_int_binaryop(op, a.int_obj, b.int_obj));
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
  if (op == NOT && a.tp == FLOAT_OBJ)
    return Object_int(!a.float_obj);
  if (a.tp == INT_OBJ)
    return Object_int(_int_unaryop(op, a.int_obj));
  if (a.tp == FLOAT_OBJ)
    return Object_float(_float_unaryop(op, a.float_obj));
  if (op == NOT)
    return Object_int(!Object_to_bool(a));
  printf("Unsupported unary operation.");
  exit(-1);
}
