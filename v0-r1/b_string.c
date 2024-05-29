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

void String_append(String* str, char ch) {
  if (str->max == str->size) {
    str->max *= 2;
    char* p = (char*)realloc(str->val, sizeof(char) * (str->max + 1));
    if (p == NULL) {
      printf("Failed to realloc.");
      exit(-1);
    }
    str->val = p;
  }
  str->val[str->size++] = ch;
  str->val[str->size] = '\0';
}

void String_cat(String* str, String* other) {
  size_t new_size = str->size + other->size;
  if (new_size > str->max) {
    str->max = new_size;
    char* p = (char*)realloc(str->val, sizeof(char) * (str->max + 1));
    if (p == NULL) {
      printf("Failed to realloc.");
      exit(-1);
    }
    str->val = p;
  }
  strcpy(str->val + str->size, other->val);
  str->size = new_size;
}
