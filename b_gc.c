#include "b_gc.h"

GC_NodeList *GC_NodeList_new(GC_Node *cur) {
  GC_NodeList *nl = malloc(sizeof(GC_NodeList));
  nl->cur = cur;
  nl->next = NULL;
  return nl;
}

void GC_NodeList_free(GC_NodeList *nl) {
  GC_NodeList *cur = nl;
  while (cur) {
    GC_NodeList *last = cur;
    cur = cur->next;
    free(last);
  }
}

void GC_NodeList_append(GC_NodeList **nl, GC_Node *cur) {
  GC_NodeList *old = *nl;
  *nl = GC_NodeList_new(cur);
  (*nl)->next = old;
}

void GC_NodeList_remove_next(GC_NodeList *nl) {
  if (nl->next) {
    GC_NodeList *nn = nl->next->next;
    free(nl->next);
    nl->next = nn;
  }
}

void GC_NodeList_remove_first(GC_NodeList **nl) {
  GC_NodeList *n = (*nl)->next;
  free(*nl);
  *nl = n;
}

GC_Node *GC_Node_new(void *ptr, GC_Root *gc) {
  GC_Node *n = malloc(sizeof(GC_Node));
  n->cnt = 1;
  n->children = NULL;
  n->ptr = ptr;
  n->parent = NULL;
  GC_add_obj(gc, n);
  return n;
}

GC_Node *GC_malloc(size_t bsize, GC_Root *gc) {
  return GC_Node_new(malloc(bsize), gc);
}

GC_Node *GC_realloc(GC_Node *n, size_t bsize, GC_Root *gc) {
  n->ptr = realloc(n->ptr, bsize);
  return n;
}

void GC_Node_free(GC_Node *n) {
  GC_NodeList_free(n->children);
  free(n);
}

void GC_Node_connect(GC_Node *base, GC_Node *tgt) {
  tgt->parent = base;
  if (!base->children)
    base->children = GC_NodeList_new(tgt);
  else
    GC_NodeList_append(&base->children, tgt);
}

void GC_Node_dennect(GC_Node *base, GC_Node *tgt) {
  // printf("GC_Node_dennect %llx %llx %llu\n", base, tgt, tgt->cnt);
  assert(tgt);
  tgt->cnt--;
  if (tgt->cnt > 0)
    return;
  tgt->parent = NULL;
  if (base->children) {
    if (base->children->cur == tgt) {
      GC_NodeList_remove_first(&base->children);
    } else {
      for (GC_NodeList *pre = base->children; pre->next; pre = pre->next) {
        if (pre->next->cur == tgt) {
          GC_NodeList_remove_next(pre);
          return;
        }
      }
    }
  }
}

GC_Root GC_new() {
  GC_Root root;
  root.gc_nodes = NULL;
  GC_Node *n = malloc(sizeof(GC_Node));
  n->cnt = 1;
  n->children = NULL;
  n->ptr = NULL;
  n->parent = NULL;
  root.gc_rootnode = n;
  return root;
}

void _GC_recurse_tag(GC_Node *node) {
  if (node->flag)
    return;
  node->flag = true;
  for (GC_NodeList *nl = node->children; nl; nl = nl->next) {
    _GC_recurse_tag(nl->cur);
  }
}

void GC_add_obj(GC_Root *gc, GC_Node *n) {
  GC_NodeList_append(&gc->gc_nodes, n);
}

void GC_collect(GC_Root *root) {
  printf("GC_collect\n");
  _GC_recurse_tag(root->gc_rootnode);
  for (GC_NodeList *nl = root->gc_nodes; nl; nl = nl->next) {
    printf("ptr: %llx cnt: %llu\n", nl->cur->ptr, nl->cur->cnt);
    if (!nl->cur->flag && !nl->cur->cnt) {
      printf("free %llx\n", nl->cur->ptr);
      free(nl->cur->ptr);
      GC_Node_free(nl->cur);
    }
  }
}
