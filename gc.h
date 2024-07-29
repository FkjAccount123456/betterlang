#include <stdbool.h>

typedef void (*gc_free_t)(void *);

typedef struct gc_ObjNode gc_ObjNode;

typedef struct gc_Children {
  struct gc_Children *prev, *next;
  gc_ObjNode *ch;
} gc_Children;

gc_Children *gc_Children_new(gc_ObjNode *);
// 用**是因为它可能为NULL，下同
void gc_Children_append(gc_Children **, gc_ObjNode *);
void gc_Children_remove(gc_Children *);
void gc_Children_free(gc_Children *);

typedef struct gc_ObjNode {
  gc_ObjNode *prev, *next;
  gc_Children *chs;
  void *ptr;
  gc_free_t freer;
  bool vis;
} gc_ObjNode;

gc_ObjNode *gc_ObjNode_new(void *, gc_free_t);
void gc_ObjNode_append(gc_ObjNode **, gc_ObjNode *);
// 将obj移除，同时将它free掉
void gc_ObjNode_remove(gc_ObjNode *);

void gc_ObjNode_recursive(gc_ObjNode *);
void gc_ObjNode_collect(gc_ObjNode *);

extern gc_ObjNode *gc;

void gc_init();
void gc_collect();
