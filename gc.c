#include "gc.h"

GCList *GCList_copy(GCList *l) {
  GCList *res = NULL;
  for (GCList *cur = l; cur; cur = cur->next) {
    GCList *old = res;
    res = malloc(sizeof(GCList));
    res->next = old;
    res->cur = cur->cur;
  }
  return res;
}

void GCList_removefirst(GCList **l) {
  GCList *next = (*l)->next;
  free(*l);
  *l = next;
}

void GCList_removenext(GCList *l) {
  GCList *next = l->next->next;
  free(l->next);
  l->next = next;
}

void GCList_append(GCList **l, GCPtr *cur) {
  GCList *res = malloc(sizeof(GCList));
  res->cur = cur;
  res->next = *l;
  *l = res;
}

void GCList_free(GCList *l) {
  while (l) {
    GCList *next = l->next;
    free(l);
    l = next;
  }
}

GCLeaf *GCLeaf_new(GCTrait *trait, void *ptr) {
  GCLeaf *l = malloc(sizeof(GCList));
  l->trait = trait;
  l->ptr = ptr;
  l->flag = false;
  GC_add_ptr(l);
  return l;
}

void GCLeaf_free(GCLeaf *l) {
  l->trait->dstcor(l->ptr);
  free(l);
}

GCPtr *GCPtr_new(GCLeaf *ptr) {
  GCPtr *res = malloc(sizeof(GCPtr));
  res->ptr = ptr;
  res->chs = NULL;
  res->fas = NULL;
  return res;
}

void GCPtr_free(GCPtr *ptr) {
  for (GCList *cur = ptr->chs; cur; cur = cur->next) {
    GCPtr_remfa(cur->cur, ptr);
  }
  for (GCList *cur = ptr->fas; cur; cur = cur->next) {
    GCPtr_remch(cur->cur, ptr);
  }
  GCList_free(ptr->chs);
  GCList_free(ptr->fas);
  free(ptr);
}

void GCPtr_addch(GCPtr *base, GCPtr *ch) {
  GCList_append(&base->chs, ch);
  GCList_append(&ch->fas, base);
}

void GCPtr_remch(GCPtr *base, GCPtr *ch) {
  if (base->chs->cur == ch)
    GCList_removefirst(&base->chs);
  else {
    for (GCList *cur = base->chs; cur->next; cur = cur->next) {
      if (cur->next->cur == ch) {
        GCList_removenext(cur);
        return;
      }
    }
  }
}

void GCPtr_remfa(GCPtr *ch, GCPtr *base) {
  if (ch->fas->cur == base)
    GCList_removefirst(&ch->fas);
  else {
    for (GCList *cur = ch->fas; cur->next; cur = cur->next) {
      if (cur->next->cur == base) {
        GCList_removenext(cur);
        return;
      }
    }
  }
}

GCPtr *GCPtr_pass(GCPtr *ptr) {
  GCPtr *res = malloc(sizeof(GCPtr));
  res->ptr = ptr->ptr;
  res->chs = GCList_copy(ptr->chs);
  return res;
}

GCPtr *GCPtr_copy(GCPtr *ptr) {
  return ptr->ptr->trait->copier(ptr->ptr);
}

GCRoot gc;

void GCRoot_init() {
  gc.cnt = 0;
  gc.max = 8;
  gc.ptrs = malloc(sizeof(GCLeaf *) * gc.max);
  gc.root = GCPtr_new(NULL);
}

void GC_add_ptr(GCLeaf *ptr) {
  if (gc.cnt == gc.max) {
    gc.max *= 2;
    gc.ptrs = realloc(gc.ptrs, sizeof(GCLeaf *) * gc.max);
  }
  gc.ptrs[gc.cnt++] = ptr;
}

void GC_recursive(GCPtr *cur) {
  if (cur->ptr)
    cur->ptr->flag = true;
  for (GCList *l = cur->chs; l; l = l->next) {
    GC_recursive(l->cur);
  }
}

void GC_collect() {
  GC_recursive(gc.root);
  for (size_t i = 0; i < gc.cnt; i++) {
    if (gc.ptrs[i] && !gc.ptrs[i]->flag) {
      GCLeaf_free(gc.ptrs[i]);
      gc.ptrs[i] = NULL;
    } else if (gc.ptrs[i]) {
      gc.ptrs[i]->flag = false;
    }
  }
}

GCLeaf *GC_malloc(size_t size) {
  GCLeaf *res = malloc(sizeof(GCLeaf));
  res->trait = NULL;
  res->ptr = malloc(size);
  res->flag = false;
  GC_add_ptr(res);
  return res;
} 

void GC_realloc(GCLeaf *ptr, size_t size) {
  ptr->ptr = realloc(ptr->ptr, size);
}
