#ifndef B_STRING_H
#define B_STRING_H

#include <stddef.h>
#include "b_object.h"

typedef struct String {
  size_t size, max;
  char *val;
} String;

char *str_copy(char *base);

String *String_new(char *base);
void String_append(String *str, char ch);
void String_cat(String *str, String *other);
void String_free(void *str);
void *String_copy(void *str);

ObjTrait *String_ObjTrait();

#endif // B_STRING_H
