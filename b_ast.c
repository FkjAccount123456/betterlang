#include "b_ast.h"
#include <stdio.h>

/* ASTList *ASTList_new(size_t max) {
  ASTList *list = b_malloc(sizeof(ASTList));
  list->len = 0;
  list->max = max;
  list->nodes = b_malloc(sizeof(ASTNode *) * list->max);
  return list;
}

void ASTList_append(ASTList *list, ASTNode *node) {
  if (list->len == list->max) {
    list->max *= 2;
    list->nodes = b_realloc(list->nodes, sizeof(ASTNode *) * list->max);
  }
  list->nodes[list->len++] = node;
}

void ASTList_free(ASTList *list) {
  for (int i = 0; i < list->len; i++) {
    ASTNode_free(list->nodes[i]);
  }
  b_free(list->nodes);
  b_free(list);
} */

ASTNode *ASTNode_new(ASTType type) {
  ASTNode *node = b_malloc(sizeof(ASTNode));
  node->type = type;
  return node;
}

void ASTNode_free(ASTNode *node) {
  switch (node->type) {
  case NoOpStmt:
  case BreakStmt:
  case ContinueStmt:
  case NoneExpr:
  case IntExpr:
  case FloatExpr:
  case OpTerminal:
    break;
  case StringExpr:
  case VarExpr:
    // String_free(node->stringAST);
    gc_Children_remove(gc.gcmap->chs, node->stringAST->gcobj);
    break;
  case ExprStmt:
  case ReturnStmt:
    ASTNode_free(node->nodeAST);
    break;
  default:
    for (size_t i = 0; i < node->listAST.len; i++) {
      ASTNode_free(node->listAST.v[i]);
    }
    SeqFree(node->listAST);
    break;
  }
}

void ASTNode_print(ASTNode *node, size_t indent) {
  for (size_t i = 0; i < indent; i++) {
    printf("  ");
  }
#define PL                                                                     \
  for (size_t i = 0; i < node->listAST.len; i++) {                             \
    ASTNode_print(node->listAST.v[i], indent + 1);                             \
    if (i != node->listAST.len - 1)                                            \
      printf("\n");                                                            \
  }
  switch (node->type) {
  case NoOpStmt:
    printf("noop");
    break;
  case BlockStmt:
    printf("block\n");
    PL break;
  case ExprStmt:
    printf("exprstmt\n");
    ASTNode_print(node->nodeAST, indent + 1);
    break;
  case AssignStmt:
    printf("assign\n");
    break;
  case IfStmt:
    printf("if\n");
    PL break;
  case WhileStmt:
    printf("while\n");
    PL break;
  case VarDeclStmt:
    printf("vardecl\n");
    PL break;
  case FuncDefStmt:
    printf("funcdef\n");
    PL break;
  case ReturnStmt:
    printf("return\n");
    ASTNode_print(node->nodeAST, indent + 1);
    break;
  case BreakStmt:
    printf("break");
    break;
  case ContinueStmt:
    printf("continue");
    break;
  case NoneExpr:
    printf("none");
    break;
  case IntExpr:
    printf("int %lld", node->intAST);
    break;
  case FloatExpr:
    printf("float %lf", node->floatAST);
    break;
  case StringExpr:
    printf("string %s", node->stringAST->v);
    break;
  case VarExpr:
    printf("var %s", node->stringAST->v);
    break;
  case ExprExpr:
    printf("expr\n");
    PL break;
  case UnaryExpr:
    printf("unary\n");
    PL break;
  case TernaryExpr:
    printf("ternary\n");
    PL break;
  case BuildListExpr:
    printf("buildlist\n");
    PL break;
  case BuildDictExpr:
    printf("builddict\n");
    PL break;
  case LambdaExpr:
    printf("lambda\n");
    PL break;
  case CallExpr:
    printf("call\n");
    PL break;
  case AttrExpr:
    printf("attr\n");
    PL break;
  case IndexExpr:
    printf("index\n");
    PL break;
  case OpTerminal:
    printf("op %d", node->opAST);
    break;
  default:
    printf("unknown");
    break;
  }
#undef PL
}
