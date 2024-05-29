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
void String_append(String* str, char ch);
void String_cat(String* str, String* other);

#endif // B_STRING_H
