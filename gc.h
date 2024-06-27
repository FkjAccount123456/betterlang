#ifndef GC_H
#define GC_H

#include "includes.h"

typedef void (*GC_Dstcor)(void *);

typedef struct GC_Object {
  void *ptr;
  GC_Dstcor dstcor;
} GC_Object;

// 所以我还需要写邻接表
typedef struct GC_GTable {
  GC_Object **G;
  size_t *G_sizes, *G_maxs;
  size_t size, max;
} GC_GTable;

GC_GTable *GC_new();
void GC_add_obj(GC_GTable *gc, GC_Object obj);
void GC
void GC_free(GC_GTable *gc);

#endif
