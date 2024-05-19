#include "b_string.h"
#include "b_alloc.h"
#include <string.h>

char *str_copy(char *base) {
  char *str = (char *)b_alloc(sizeof(char) * (strlen(base)));
  strcpy(str, base);
  return str;
}

String *String_new(char *base) {
  String *str = (String *)b_alloc(sizeof(String));
  str->size = str->max = strlen(base);
  str->val = str_copy(base);
  return str;
}

void String_append(String *str, char ch) {
  if (str->max == str->size) {
    str->max *= 2;
    str->val = (char *)b_realloc(str->val, sizeof(char) * (str->max + 1));
  }
  str->val[str->size++] = ch;
  str->val[str->size] = '\0';
}

void String_cat(String *str, String *other) {
  size_t new_size = str->size + other->size;
  if (new_size > str->max) {
    str->max = new_size;
    str->val = (char *)b_realloc(str->val, sizeof(char) * (str->max + 1));
  }
  strcpy(str->val + str->size, other->val);
  str->size = new_size;
}

void String_free(void *str) {
  b_free(((String *)str)->val);
  b_free(str);
}

void *String_copy(void *base) {
  String *str = String_new(((String *)base)->val);
  return str;
}

ObjTrait *String_ObjTrait() {
  ObjTrait *objtrait = (ObjTrait *)b_alloc(sizeof(ObjTrait));
  objtrait->copier = &String_copy;
  objtrait->destructor = &String_free;
  return objtrait;
}
