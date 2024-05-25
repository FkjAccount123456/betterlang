#include "b_dict.h"
#include <corecrt.h>

Dict *Dict_new() {
  Dict *dict = (Dict *)b_alloc(sizeof(Dict));
  dict->children = (Dict **)b_alloc(sizeof(Dict *) * 128);
  for (size_t i = 0; i < 128; i++) {
    dict->children[i] = NULL;
  }
  dict->obj = NULL;
  return dict;
}

Object *Dict_find(Dict *dict, char *key) {
  if (*key == 0)
    return dict->obj;
  if (dict->children[*key])
    return Dict_find(dict->children[*key], key + 1);
  return NULL;
}

Object *Dict_insert(Dict *dict, char *key, Object obj) {
  // printf("Dict_insert %s %d\n", key, *key);
  if (*key == 0) {
    if (dict->obj)
      Object_free(dict->obj);
    else
      dict->obj = (Object *)b_alloc(sizeof(Object));
    *dict->obj = obj;
    return dict->obj;
  }
  if (!dict->children[*key])
    dict->children[*key] = Dict_new();
  return Dict_insert(dict->children[*key], key + 1, obj);
}

void *Dict_copy(void *base) {
  Dict *dict = Dict_new();
  for (size_t i = 0; i < 128; i++) {
    if (((Dict *)base)->children[i]) {
      dict->children[i] = Dict_copy(((Dict *)base)->children[i]);
    }
  }
  dict->obj = (Object *)b_alloc(sizeof(Object));
  *dict->obj = Object_copy(((Dict *)base)->obj);
  return dict;
}

void Dict_free(void *dict) {
  for (size_t i = 0; i < 128; i++) {
    if (((Dict *)dict)->children[i]) {
      Dict_free(((Dict *)dict)->children[i]);
    }
  }
  if (((Dict *)dict)->obj) {
    Object_free(((Dict *)dict)->obj);
  }
}

ObjTrait *Dict_ObjTrait() {
  ObjTrait *objtrait = (ObjTrait *)b_alloc(sizeof(ObjTrait));
  objtrait->copier = Dict_copy;
  objtrait->destructor = Dict_free;
  return objtrait;
}
