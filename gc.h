#ifndef GC_H
#define GC_H

#include "includes.h"

typedef struct GCPtr GCPtr;

typedef GCPtr *(*Copier)(void *);
typedef void (*Dstcor)(void *);

typedef enum ObjType {
  INT_OBJ,
  FLOAT_OBJ,
  BUILTIN_OBJ,
  NULL_OBJ,
  STR_OBJ,
  LIST_OBJ,
  DICT_OBJ,
  VMFRAME_OBJ,
  FUNC_OBJ,
} ObjType;

typedef struct GCTrait {
  ObjType tp;
  Copier copier;
  Dstcor dstcor;
} GCTrait;

void GCTraits_init();

typedef struct GCList {
  struct GCList *next;
  GCPtr *cur;
} GCList;

GCList *GCList_copy(GCList *l);
void GCList_removefirst(GCList **l);
void GCList_removenext(GCList *l);
void GCList_append(GCList **l, GCPtr *cur);
void GCList_free(GCList *l);

typedef struct GCLeaf {
  GCTrait *trait;
  void *ptr;
  bool flag;
} GCLeaf;

GCLeaf *GCLeaf_new(GCTrait *trait, void *ptr);
void GCLeaf_free(GCLeaf *l);

typedef struct GCPtr {
  GCLeaf *ptr;
  GCList *chs;
  GCList *fas;
} GCPtr;

GCPtr *GCPtr_new(GCLeaf *ptr);
void GCPtr_free(GCPtr *ptr);
void GCPtr_addch(GCPtr *base, GCPtr *ch);
void GCPtr_remch(GCPtr *base, GCPtr *ch);
void GCPtr_remfa(GCPtr *ch, GCPtr *base);
GCPtr *GCPtr_pass(GCPtr *ptr);
GCPtr *GCPtr_copy(GCPtr *ptr);

typedef struct GCRoot {
  size_t cnt, max;
  GCLeaf **ptrs;
  GCPtr *root;
} GCRoot;

extern GCRoot gc;
void GCRoot_init();
void GC_add_ptr(GCLeaf *ptr);
void GC_recursive(GCPtr *cur);
void GC_collect();

GCLeaf *GC_malloc(size_t size);
void GC_realloc(GCLeaf *ptr, size_t size);

#endif // GC_H
