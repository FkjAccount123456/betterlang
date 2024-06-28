#ifndef GC_H
#define GC_H

#include "includes.h"

// free一个对象的时候，只需要把它的主体free掉并将其从active中清除即可，不需要管子对象
typedef void (*GC_Dstcor)(void *);

typedef struct GC_Object {
  void *ptr;
  GC_Dstcor dstcor;
} GC_Object;

GC_Object *GC_Object_new(void *ptr, GC_Dstcor dstcor);

// GC就是一个图
// 所以我还需要写邻接表
typedef struct GC_GTable {
  size_t **G;
  GC_Object **G_bases;
  size_t *G_sizes, *G_maxs;
  // 对象是否为stack中出现的
  bool *G_isrefed;
  size_t size, max;
} GC_GTable;

extern GC_GTable gc;

// 这么多size_t容易乱
void GC_init();
size_t GC_objs_add(GC_Object *obj);
void GC_active_add(size_t pos);
void GC_active_remove(size_t ch_pos);
void GC_obj_add_ch(size_t obj_pos, size_t ch);
void GC_obj_remove_ch(size_t obj_pos, size_t ch);
void GC_collect();
void GC_quit();

#endif
