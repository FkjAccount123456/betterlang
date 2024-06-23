#include "vm.h"

VMCode VMCode_new(ByteCode head) {
  VMCode vc;
  vc.head = head;
  return vc;
}

void VMCode_run(VMCode *code, GC_Root *gc) {
  size_t pc;
  GC_Node *stack_node = List_new();
  List *stack = stack_node->ptr;
  Object *stack_items = stack->items->ptr;

  while (code[pc].head != EXIT) {
    ByteCode head = code[pc].head;
    switch (head) {
    case PUSH_I:
      List_append(stack, Object_int(code[pc].i), gc);
      break;
    case PUSH_F:
      List_append(stack, Object_float(code[pc].f), gc);
      break;
    case PUSH_S:
      List_append(stack, Object_gc(STR_OBJ, code[pc].s), gc);
      break;
    case POP:
      stack->size--;
      Object_free(stack_items[stack->size]);
      break;
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
      Object b = stack_items[--stack->size];
      Object a = stack_items[--stack->size];
      Object res = Object_binaryop(head, a, b);
      Object_free(a);
      Object_free(b);
      List_append(stack, res);
      break;
    }
    case POS:
    case NEG:
    case NOT:
    {
      Object a = stack_items[--stack->size];
      Object res = Object_unaryop(head, a);
      Object_free(a);
      List_append(stack, res);
      break;
    }
    case JMP:
      pc = code[pc].l;
      break;
    case JZ:
      Object cond = stack_items[--stack->size];
      if (Object_to_bool(cond))
        pc = code[pc].l;
      Object_free(cond);
      break;
    case JNZ:
      Object cond = stack_items[--stack->size];
      if (!Object_to_bool(cond))
        pc = code[pc].l;
      Object_free(cond);
      break;
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
  if ((a.tp == INT_OBJ || a.tp == FLOAT_OBJ) && (b.tp == INT_OBJ || b.tp == FLOAT_OBJ)) {
    if (a.tp == FLOAT_OBJ || b.tp == FLOAT_OBJ) {
      return Object_float(_float_binaryop(op, a.float_obj, b.float_obj));
    }
    return Object_int(_int_binaryop(op, a.int_obj, b.int_obj));
  }
  if (op == AND)
    return Object_to_bool(a) && Object_to_bool(b);
  if (op == OR)
    return Object_to_bool(a) || Object_to_bool(b);
  if (op == XOR)
    return Object_to_bool(a) ^ Object_to_bool(b);
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
