/*
看到了tree-sitter带检查的malloc/free，大为震悚
自己的当然也要有这种安全的机制
*/
#include <stddef.h>

void *b_malloc(size_t);
void *b_calloc(size_t, size_t);
void *b_realloc(void *, size_t);
extern void (*b_free)(void *);
