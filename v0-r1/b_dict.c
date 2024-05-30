#include "b_dict.h"

Dict *Dict_new() {
  Dict *dict = (Dict *)malloc(sizeof(Dict));
  if (dict == NULL) {
    printf("Failed to malloc.");
    exit(-1);
  }
  dict->children = (Dict **)calloc(128, sizeof(Dict *));
  dict->val = NULL;
  return dict;
}

void Dict_free(void *_dict) {
  Dict *dict = (Dict *)_dict;
  for (size_t i = 0; i < 128; i++)
    if (dict->children[i])
      free(dict->children[i]), dict->children[i] = NULL;
  if (dict->val)
    free(dict->val);
  free(dict);
}

void *Dict_pass(void *_dict) {
  Dict *dict = (Dict *)_dict;
  for (size_t i = 0; i < 128; i++)
    if (dict->children[i])
      dict->children[i] = (Dict *)Dict_pass(dict->children[i]);
  if (dict->val)
    *dict->val = Object_pass(dict->val);
  return dict;
}

void *Dict_copy(void *_base) {
  Dict *base = (Dict *)_base;
  Dict *dict = Dict_new();
  for (size_t i = 0; i < 128; i++) {
    if (base->children[i]) {
      dict->children[i] = (Dict *)Dict_copy(base->children[i]);
    }
  }
  if (base->val) {
    dict->val = (Object *)malloc(sizeof(Object));
    if (dict->val == NULL) {
      printf("Failed to malloc.");
      exit(-1);
    }
    *dict->val = Object_copy(dict->val);
  }
  return dict;
}
