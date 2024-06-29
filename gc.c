#include "gc.h"

GC_Object *GC_Object_new(void *ptr, GC_Dstcor dstcor) {
  GC_Object *obj = malloc(sizeof(GC_Object));
  obj->ptr = ptr;
  obj->dstcor = dstcor;
  return obj;
}

GC_GTable gc;

void GC_init() {
  gc.size = 0;
  gc.max = 16;
  gc.G = malloc(sizeof(size_t *) * gc.max);
  gc.G_bases = malloc(sizeof(GC_Object) * gc.max);
  gc.G_sizes = malloc(sizeof(size_t) * gc.max);
  gc.G_maxs = malloc(sizeof(size_t) * gc.max);
  gc.G_isrefed = malloc(sizeof(bool) * gc.max);
  gc.G_sizes[0] = 0;
  GC_objs_add(GC_Object_new(NULL, NULL));
}

size_t GC_objs_add(GC_Object *obj) {
  if (gc.size == gc.max) {
    gc.max *= 2;
    gc.G = realloc(gc.G, sizeof(size_t *) * gc.max);
    gc.G_bases = realloc(gc.G_bases, sizeof(GC_Object) * gc.max);
    gc.G_sizes = realloc(gc.G_sizes, sizeof(size_t) * gc.max);
    gc.G_maxs = realloc(gc.G_maxs, sizeof(size_t) * gc.max);
    gc.G_isrefed = realloc(gc.G_isrefed, sizeof(bool) * gc.max);
  }
  gc.G_sizes[gc.size] = 0;
  gc.G_maxs[gc.size] = 1;
  gc.G[gc.size] = malloc(sizeof(size_t) * gc.G_maxs[gc.size]);
  gc.G_bases[gc.size] = obj;
  gc.G_isrefed[gc.size] = 0;
  return gc.size++;
}

void GC_active_add(size_t pos) { GC_obj_add_ch(0, pos); }

void GC_active_remove(size_t ch_pos) { GC_obj_remove_ch(0, ch_pos); }

void GC_obj_add_ch(size_t obj_pos, size_t ch) {
  // printf("GC_obj_add_ch %llu %llu gc.size: %llu, gc.G_size: %llu, gc.G_max: %llu\n",
        //  obj_pos, ch, gc.size, gc.G_sizes[obj_pos], gc.G_maxs[obj_pos]);
  if (gc.G_sizes[obj_pos] == gc.G_maxs[obj_pos]) {
    gc.G_maxs[obj_pos] *= 2;
    gc.G[obj_pos] = realloc(gc.G[obj_pos], sizeof(size_t) * gc.G_maxs[obj_pos]);
  }
  gc.G[obj_pos][gc.G_sizes[obj_pos]++] = ch;
  // printf("Succeed\n");
}

void GC_obj_remove_ch(size_t obj_pos, size_t ch) {
  size_t pos;
  for (pos = 0; pos < gc.G_sizes[obj_pos]; pos++)
    if (gc.G[obj_pos][pos] == ch)
      break;
  for (size_t i = pos; i < gc.G_sizes[obj_pos] - 1; i++)
    gc.G[obj_pos][i] = gc.G[obj_pos][i + 1];
  gc.G_sizes[obj_pos]--;
}

void _GC_recursive(size_t obj_pos) {
  if (gc.G_isrefed[obj_pos])
    return;
  printf("_GC_recursive %llu\n", obj_pos);
  gc.G_isrefed[obj_pos] = true;
  for (size_t i = 0; i < gc.G_sizes[obj_pos]; i++) {
    _GC_recursive(gc.G[obj_pos][i]);
  }
}

void GC_collect() {
  puts("GC_collect");
  gc.G_isrefed[0] = 0;
  _GC_recursive(0);
  for (size_t i = 1; i < gc.size; i++) {
    printf("%llu\n", i);
    if (!gc.G_isrefed[i] && gc.G_bases[i]->ptr) {
      gc.G_bases[i]->dstcor(gc.G_bases[i]->ptr);
      gc.G_bases[i]->ptr = NULL;
    }
    gc.G_isrefed[i] = false;
  }
}

void GC_quit() {
  for (size_t obj_pos = 0; obj_pos < gc.size; obj_pos++) {
    free(gc.G[obj_pos]);
  }
  free(gc.G);
  free(gc.G_maxs);
  free(gc.G_sizes);
  free(gc.G_isrefed);
  free(gc.G_bases);
}
