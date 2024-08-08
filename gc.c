/*
好的设计是成功的一半
*/

#include "gc.h"
#include "b_stdlib.h"
#include <stdio.h>
#include <stdlib.h>
// #include "b_object.h"

gc_Children *gc_Children_new(gc_ObjNode *obj) {
  gc_Children *ch = b_malloc(sizeof(gc_Children));
  ch->ch = obj;
  ch->next = NULL;
  return ch;
}

void gc_Children_append(gc_Children *chs, gc_ObjNode *node) {
  gc_Children *next = gc_Children_new(node);
  next->next = chs->next;
  chs->next = next;
}

void _gc_Children_remove_next(gc_Children *prev) {
  gc_Children *next = prev->next->next;
  b_free(prev->next);
  prev->next = next;
}

void gc_Children_remove(gc_Children *chs, gc_ObjNode *node) {
  gc_Children *cur = chs;
  while (cur->next) {
    if (cur->next->ch == node) {
      _gc_Children_remove_next(cur);
      return;
    }
    cur = cur->next;
  }
}

gc_Object *gc_Object_new(void *ptr, gc_free_t freer) {
  gc_Object *obj = b_malloc(sizeof(gc_Object));
  gc_add_obj(obj);
  obj->ptr = ptr;
  obj->freer = freer;
  obj->vis = false;
  return obj;
}

gc_ObjNode *gc_ObjNode_new(gc_Object *obj) {
  gc_ObjNode *node = b_malloc(sizeof(gc_ObjNode));
  node->obj = obj;
  node->chs = gc_Children_new(NULL);
  return node;
}

void gc_ObjNode_recursive(gc_ObjNode *node) {
  if (node->obj && !node->obj->vis) {
    node->obj->vis = true;
    gc_Children *cur = node->chs->next;
    while (cur) {
      gc_ObjNode_recursive(cur->ch);
      cur = cur->next;
    }
  }
}

GC gc;

void gc_init() {
  gc.objs = b_malloc(sizeof(gc_Object));
  gc.objs->freer = gc.objs->ptr = gc.objs->next = NULL;
  gc.objs->vis = NULL;
  // 图的一号节点（
  gc.gcmap = gc_ObjNode_new(gc.objs);
}

// 反正链表我当无序结构用，插前面有何不可？
void gc_add_obj(gc_Object *obj) {
  obj->next = gc.objs->next;
  gc.objs->next = obj;
}

void gc_collect() {
  gc_ObjNode_recursive(gc.gcmap);
  gc_Object *cur = gc.objs;
  while (cur && cur->next) {
    // printf("%p ", cur->next);
    if (!cur->next->vis) {
      cur->next->freer(cur->next->ptr);
      gc_Object *next = cur->next->next;
      b_free(cur->next);
      cur->next = next;
      // puts("collected");
    } else {
      cur->next->vis = false;
      // puts("reserved");
      // printf("%s\n", ((String *)cur->next->ptr)->v);
      cur = cur->next;
    }
  }
}
