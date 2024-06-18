#ifndef B_GC_H
#define B_GC_H

#include "b_includes.h"

typedef struct GC_Node GC_Node;
typedef struct GC_Root GC_Root;

typedef struct GC_NodeList {
  GC_Node *cur;
  struct GC_NodeList *next;
} GC_NodeList;

GC_NodeList *GC_NodeList_new(GC_Node *cur);
void GC_NodeList_free(GC_NodeList *nl);
void GC_NodeList_append(GC_NodeList **nl, GC_Node *cur);
void GC_NodeList_remove_next(GC_NodeList *nl);
void GC_NodeList_remove_first(GC_NodeList **nl);

typedef struct GC_Node {
  struct GC_Node *parent;
  void *ptr;
  GC_NodeList *children;
  bool flag;
  size_t cnt;
} GC_Node;

GC_Node *GC_Node_new(void *ptr, GC_Root *gc);
GC_Node *GC_malloc(size_t bsize, GC_Root *gc);
GC_Node *GC_realloc(GC_Node *n, size_t bsize, GC_Root *gc);
void GC_Node_connect(GC_Node *base, GC_Node *tgt);
void GC_Node_dennect(GC_Node *base, GC_Node *tgt);
void GC_Node_free(GC_Node *n);

typedef struct GC_Root {
  GC_NodeList *gc_nodes;
  GC_Node *gc_rootnode;
} GC_Root;

GC_Root GC_new();
void GC_add_obj(GC_Root *gc, GC_Node *n);
void GC_collect(GC_Root *gc_root);

#endif // B_GC_H
