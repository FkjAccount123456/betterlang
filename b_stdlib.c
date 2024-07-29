#include "b_stdlib.h"
#include <stdio.h>
#include <stdlib.h>

void *b_malloc(size_t size) {
  void *r = malloc(size);
  if (size & !r) {
    printf("CoreError: failed to malloc");
    exit(-1);
  }
  return r;
}

void *b_calloc(size_t nobjs, size_t objsize) {
  void *r = calloc(nobjs, objsize);
  if (nobjs & !r) {
    printf("CoreError: failed to calloc");
    exit(-1);
  }
  return r;
}

void *b_realloc(void *base, size_t new_size) {
  void *r = realloc(base, new_size);
  if (new_size & !r) {
    printf("CoreError: failed to calloc");
    exit(-1);
  }
  return r;
}

void (*b_free)(void *) = free;
