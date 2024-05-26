#ifndef B_STRING_H
#define B_STRING_H

#include "includes.h"

typedef struct String {
  size_t size, max;
  char* val;
} String;

char* str_copy(char* base);
String* String_new(char* base);
void String_free(void* str);
void* String_copy(void* base);

#endif  // B_STRING_H
