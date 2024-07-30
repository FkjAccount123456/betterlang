#include "b_object.h"
#include "b_stdlib.h"
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

ObjTrait none_trait, int_trait, float_trait,
         string_trait, list_trait, dict_trait;

void _none_print(Object *obj, bool istop) {
  printf("none");
}

void _int_print(Object *obj, bool istop) {
  printf("%lld", obj->intVal);
}

void _float_print(Object *obj, bool istop) {
  printf("%lf", obj->floatVal);
}

void _string_print(Object *obj, bool istop) {
  if (istop) {
    printf("%s", obj->stringVal->v);
  } else {
    printf("\"");
    for (size_t i = 0; i < obj->stringVal->len; i++) {
      char ch = obj->stringVal->v[i];
      if (ch == '\r') {
        printf("\\r");
      } else if (ch == 't') {
        printf("\\t");
      } else if (ch == 'a') {
        printf("\\a");
      } else if (ch == 'f') {
        printf("\\f");
      } else if (ch == 'v') {
        printf("\\v");
      } else if (ch == 'b') {
        printf("\\b");
      } else if (ch == 'n') {
        printf("\\n");
      } else if (ch == '\\') {
        printf("\\\\");
      } else if (ch == '\"') {
        printf("\"");
      } else if (isprint(ch)) {
        putchar(ch);
      } else {
        printf("\\x%02x", ch);
      }
    }
    printf("\"");
  }
}

void _list_print(Object *obj, bool istop) {
  List *list = obj->listVal;
  printf("[");
  if (list->len) {
    Object_print(&list->v[0], false);
    for (size_t i = 1; i < list->len; i++) {
      printf(", ");
      Object_print(&list->v[i], false);
    }
  }
  printf("]");
}

void _dict_print(Object *obj, bool istop) {
  Dict *dict = obj->dictVal;
  printf("{");
  bool isfirst = true;
  for (size_t i = 0; i < DICT_ENTRIES; i++) {
    if (dict->entries[i]) {
      DictEntry *entry = dict->entries[i];
      while (entry) {
        if (!isfirst) {
          printf(", ");
        } else {
          isfirst = false;
        }
        printf("%s: ", entry->key);
        Object_print(&entry->val, false);
        entry = entry->next;
      }
    }
  }
  printf("}");
}

void init_traits() {
  none_trait.tp = NoneObj;
  none_trait.freer = NULL;
  none_trait.getter = NULL;
  none_trait.printer = _none_print;

  int_trait.tp = IntObj;
  int_trait.freer = NULL;
  int_trait.getter = NULL;
  int_trait.printer = _int_print;

  float_trait.tp = FloatObj;
  float_trait.freer = NULL;
  float_trait.getter = NULL;
  float_trait.printer = _float_print;

  string_trait.tp = StringObj;
  string_trait.freer = (gc_free_t)String_free;
  string_trait.getter = NULL;
  string_trait.printer = _string_print;

  list_trait.tp = ListObj;
  list_trait.freer = (gc_free_t)List_free;
  list_trait.getter = (gc_obj_get_t)List_get;
  list_trait.printer = (obj_print_t)_list_print;

  dict_trait.tp = DictObj;
  dict_trait.freer = (gc_free_t)Dict_free;
  dict_trait.getter = (gc_obj_get_t)Dict_get;
  dict_trait.printer = (obj_print_t)_dict_print;
}

Object Object_none() {
  Object obj;
  obj.tp = &none_trait;
  return obj;
}

Object Object_int(long long intVal) {
  Object obj;
  obj.tp = &int_trait;
  obj.intVal = intVal;
  return obj;
}

Object Object_float(double floatVal) {
  Object obj;
  obj.tp = &float_trait;
  obj.floatVal = floatVal;
  return obj;
}

Object Object_string(String *stringVal) {
  Object obj;
  obj.tp = &string_trait;
  obj.stringVal = stringVal;
  return obj;
}

Object Object_list(List *listVal) {
  Object obj;
  obj.tp = &list_trait;
  obj.listVal = listVal;
  return obj;
}

Object Object_dict(Dict *dictVal) {
  Object obj;
  obj.tp = &dict_trait;
  obj.dictVal = dictVal;
  return obj;
}

void Object_print(Object *obj, bool istop) {
  obj->tp->printer(obj, istop);
}

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
  list->gcobj = gc_ObjNode_new(gc_Object_new(list, (gc_free_t)List_free));
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
    gc_Children_append(&list->gcobj->chs, obj.tp->getter(&obj));
}

void List_free(List *list) {
  b_free(list->v);
  b_free(list);
}

gc_ObjNode *List_get(Object *list) { return list->listVal->gcobj; }

Object *DictEntry_find(DictEntry *entry, char *key) {
  while (entry) {
    if (!strcmp(entry->key, key)) {
      return &entry->val;
    }
    entry = entry->next;
  }
  return NULL;
}

void DictEntry_append(DictEntry **entry, char *key, Object val) {
  if (DictEntry_find(*entry, key)) {
    printf("DictError: key '%s' exists", key);
    exit(-1);
  }
  if (!*entry) {
    (*entry) = b_malloc(sizeof(DictEntry));
    (*entry)->next = NULL;
    (*entry)->key = str_copy(key);
    (*entry)->val = val;
  } else {
    DictEntry *next = (*entry)->next;
    (*entry)->next = b_malloc(sizeof(DictEntry));
    (*entry)->next->next = next;
    (*entry)->next->key = str_copy(key);
    (*entry)->next->val = val;
  }
}

Dict *Dict_new() {
  Dict *dict = b_malloc(sizeof(Dict));
  dict->gcobj = gc_ObjNode_new(gc_Object_new(dict, (gc_free_t)Dict_free));
  dict->entries = b_calloc(DICT_ENTRIES, sizeof(DictEntry *));
  return dict;
}

void Dict_insert(Dict *dict, char *key, Object val) {
  if (val.tp->getter)
    gc_Children_append(&dict->gcobj->chs, val.tp->getter(&val));
  size_t hash = str_hash(key) % DICT_ENTRIES;
  DictEntry_append(&dict->entries[hash], key, val);
}

Object *Dict_find(Dict *dict, char *key) {
  size_t hash = str_hash(key) % DICT_ENTRIES;
  return DictEntry_find(dict->entries[hash], key);
}

void Dict_set(Dict *dict, char *key, Object val) {
  size_t hash = str_hash(key) % DICT_ENTRIES;
  Object *obj = DictEntry_find(dict->entries[hash], key);
  if (!obj) {
    printf("DictError: key '%s' is not existed", key);
    exit(-1);
  }
  if (obj->tp->getter)
    gc_Children_remove(dict->gcobj->chs, obj->tp->getter(obj));
  if (val.tp->getter)
    gc_Children_append(&dict->gcobj->chs, val.tp->getter(&val));
  *obj = val;
}

void DictEntry_free(DictEntry *entry) {
  while (entry) {
    DictEntry *next = entry->next;
    b_free(entry->key);
    b_free(entry);
    entry = next;
  }
}

void Dict_free(Dict *dict) {
  for (size_t i = 0; i < DICT_ENTRIES; i++) {
    if (dict->entries[i])
      DictEntry_free(dict->entries[i]);
  }
  b_free(dict->entries);
}

gc_ObjNode *Dict_get(Object *dict) {
  return dict->dictVal->gcobj;
}
