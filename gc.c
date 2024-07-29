#include "gc.h"
#include "b_stdlib.h"

gc_Children *gc_Children_new(gc_ObjNode *ch) {
  gc_Children *r = b_malloc(sizeof(gc_Children));
  r->ch = ch;
  r->prev = r->next = NULL;
  return r;
}

void gc_Children_append(gc_Children **node, gc_ObjNode *ch) {
  if (*node == NULL) {
    *node = gc_Children_new(ch);
  } else {
    (*node)->prev->next = gc_Children_new(ch);
    (*node)->prev = (*node)->prev->next;
  }
}

void gc_Children_remove(gc_Children *node) {
  if (node->prev && node->next) {
    node->prev->next = node->next;
    node->next->prev = node->prev;
  } else if (node->prev) {
    node->prev->next = NULL;
  } else if (node->next) {
    node->next->prev = NULL;
  }
  b_free(node);
}

void gc_Children_free(gc_Children *node) {
  if (!node)
    return;
  while (node->prev)
    node = node->prev;
  while (node) {
    gc_Children *next = node->next;
    b_free(node);
    node = next;
  }
}

gc_ObjNode *gc_ObjNode_new(void *ptr, gc_free_t freer) {
  gc_ObjNode *node = b_malloc(sizeof(gc_ObjNode));
  node->ptr = ptr;
  node->freer = freer;
  node->prev = node->next = NULL;
  node->chs = NULL;
  node->vis = false;
  return node;
}

void gc_ObjNode_append(gc_ObjNode **base, gc_ObjNode *node) {
  if (!*base) {
    *base = node;
  } else {
    (*base)->prev->next = node;
    (*base)->prev = (*base)->prev->next;
  }
}

void gc_ObjNode_remove(gc_ObjNode *node) {
  node->prev->next = node->next;
  node->next->prev = node->prev;
  node->freer(node->ptr);
  b_free(node);
}

void gc_ObjNode_recursive(gc_ObjNode *node) {
  node->vis = true;
  gc_Children *ch = node->chs;
  while (ch) {
    gc_ObjNode_recursive(ch->ch);
    ch = ch->next;
  }
}

void gc_ObjNode_collect(gc_ObjNode *gc) {
  gc_ObjNode *cur = gc;
  while (cur) {
    if (!cur->vis) {
      gc_ObjNode *next = cur->prev;
      gc_ObjNode_remove(cur);
      cur = next;
    } else {
      cur->vis = false;
      cur = cur->next;
    }
  }
}

gc_ObjNode *gc;

void gc_init() {
  gc = gc_ObjNode_new(NULL, NULL);
}

void gc_collect() {
  gc_ObjNode_recursive(gc);
  gc_ObjNode_collect(gc->next);
}
