#include "b_object.h"
#include "b_stdlib.h"
#include <string.h>

String *String_new(char *base) {
  String *str = b_malloc(sizeof(String));
  str->len = strlen(base);
  str->max = 8;
  while (str->max < str->len)
    str->max *= 2;
  str->v = b_malloc(sizeof(char) * (str->max + 1));
  strcpy(str->v, base);
  return str;
}

String *String_copy(String *str) {
  return String_new(str->v);
}

void String_append(String *str, char ch) {
  if (str->len == str->max) {
    str->max *= 2;
    str->v = b_realloc(str->v, sizeof(char) * (str->max + 1));
  }
  str->v[str->len++] = ch;
  str->v[str->len] = '\0';
}

void String_cat(String *str, String *other) {
  size_t new_len = str->len + other->len;
  if (new_len > str->max) {
    while (new_len > str->max)
      str->max *= 2;
    str->v = b_realloc(str->v, sizeof(char) * (str->max + 1));
  }
  strcpy(str->v + str->len, other->v);
  str->len = new_len;
}

void String_free(String *str) {
  b_free(str->v);
  b_free(str);
}

char *str_copy(char *base) {
  char *str = b_malloc(sizeof(char) * (strlen(base) + 1));
  strcpy(str, base);
  return str;
}

size_t str_hash(char *str) {
  size_t hash = 0;
  for (char *i = str; *i; i++) {
    hash *= 131;
    hash += *i;
  }
  return hash;
}

List *List_new() {
  List *list = b_malloc(sizeof(List));
  list->gcobj = gc_ObjNode_new(list, (gc_free_t)List_free);
  list->len = 0;
  list->max = 8;
  list->v = b_malloc(sizeof(Object) * list->max);
  return list;
}

void List_append(List *list, Object obj) {
  if (list->max == list->len) {
    list->max *= 2;
    list->v = b_realloc(list->v, sizeof(Object) * list->max);
  }
  list->v[list->len++] = obj;
  if (obj.tp->getter)
    gc_ObjNode_append(&list->gcobj, obj.tp->getter(&obj));
}

void List_free(List *list) {
  b_free(list->v);
  b_free(list);
}

gc_ObjNode *List_get(Object *list) { return list->listVal->gcobj; }

Dict *Dict_new() {
  Dict *dict = b_malloc(sizeof(Dict));
  dict->gcobj = gc_ObjNode_new(dict, (gc_free_t)Dict_free);
  dict->len = 0;
  dict->max = 8;
  dict->indices = b_malloc(sizeof(size_t) * dict->max * 2);
  dict->entries = b_calloc(dict->max, sizeof(DictEntry));
  return dict;
}
