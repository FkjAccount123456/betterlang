#include "gc.h"
#include "b_stdlib.h"
#include <stdio.h>

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
    gc_Children *new_node = gc_Children_new(ch);
    new_node->prev = (*node)->prev;
    new_node->next = *node;
    if ((*node)->prev)
      (*node)->prev->next = new_node;
    (*node)->prev = new_node;
  }
}

void _gc_Children_remove(gc_Children *node) {
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

void gc_Children_remove(gc_Children *chs, gc_ObjNode *target) {
  while (chs) {
    if (chs->ch == target) {
      _gc_Children_remove(chs);
      return;
    }
    chs = chs->next;
  }
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

gc_Object *gc_Object_new(void *ptr, gc_free_t freer) {
  gc_Object *obj = b_malloc(sizeof(gc_Object));
  obj->ptr = ptr;
  obj->freer = freer;
  obj->next = gc.objs;
  obj->prev = NULL;
  obj->vis = false;
  gc.objs = obj;
  return obj;
}

void gc_Object_remove(gc_Object *node) {
  if (node->prev)
    node->prev->next = node->next;
  if (node->next)
    node->next->prev = node->prev;
  node->freer(node->ptr);
  node->ptr = NULL;
}

gc_ObjNode *gc_ObjNode_new(gc_Object *obj) {
  gc_ObjNode *node = b_malloc(sizeof(gc_ObjNode));
  node->obj = obj;
  node->prev = node->next = NULL;
  node->chs = NULL;
  gc_ObjNode_append(&gc.gcmap, node);
  return node;
}

void gc_ObjNode_append(gc_ObjNode **base, gc_ObjNode *node) {
  if (!*base) {
    *base = node;
  } else {
    if ((*base)->prev)
      (*base)->prev->next = node;
    node->prev = (*base)->prev;
    node->next = *base;
    (*base)->prev = node;
  }
}

void gc_ObjNode_remove(gc_ObjNode *node) {
  if (node->prev)
    node->prev->next = node->next;
  if (node->next)
    node->next->prev = node->prev;
  b_free(node);
}

void gc_ObjNode_recursive(gc_ObjNode *node) {
  if (node->obj)
    node->obj->vis = true;
  gc_Children *ch = node->chs;
  while (ch) {
    gc_ObjNode_recursive(ch->ch);
    ch = ch->next;
  }
}

GC gc;

void gc_init() {
  gc.gcmap = gc_ObjNode_new(NULL);
  gc.objs = NULL;
}

void gc_collect() {
  gc_ObjNode_recursive(gc.gcmap);

  gc_Object *cur = gc.objs;
  while (cur) {
    printf("%p ", cur);
    if (!cur->vis) {
      if (!cur->prev)
        gc.objs = cur->next;
      gc_Object_remove(cur);
      puts("collected");
    } else {
      cur->vis = false;
      puts("reserved");
    }
    cur = cur->next;
  }

  gc_ObjNode *curnode = gc.gcmap->next;
  while (cur) {
    gc_ObjNode *next = curnode->next;
    if (!curnode->obj->ptr) {
      b_free(curnode->obj);
      gc_ObjNode_remove(curnode);
    }
    curnode = next;
  }
}
