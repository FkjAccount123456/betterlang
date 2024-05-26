#include "b_string.h"

char* str_copy(char* base) {
  char* str = (char*)malloc(sizeof(char) * (strlen(base) + 1));
  if (str == NULL) {
    printf("Failed to malloc.");
    exit(-1);
  }
  strcpy(str, base);
  return str;
}

String* String_new(char* base) {
  String* str = (String*)malloc(sizeof(String));
  if (str == NULL) {
    printf("Failed to malloc.");
    exit(-1);
  }
  str->size = strlen(base);
  if (str->size <= 8)
    str->max = 8;
  else
    str->max = str->size;
  str->val = (char*)malloc(sizeof(char) * (str->max + 1));
  if (str->val == NULL) {
    printf("Failed to malloc.");
    exit(-1);
  }
  strcpy(str->val, base);
  return str;
}

void String_free(void* str) {
  free(((String*)str)->val);
  free(str);
}

void* String_copy(void* base) {
  return String_new(((String*)base)->val);
}
