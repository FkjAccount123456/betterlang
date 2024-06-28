#include "obj.h"

ObjectTrait none_trait, int_trait, float_trait, str_trait, list_trait, dict_trait;

void ObjectTrait_init() {
  none_trait.tp = NONE_OBJ;
  none_trait.dstcor = NULL;

  int_trait.tp = INT_OBJ;
  int_trait.dstcor = NULL;

  float_trait.tp = FLOAT_OBJ;
  float_trait.dstcor = NULL;

  str_trait.tp = STR_OBJ;
  str_trait.dstcor = (GC_Dstcor)String_free;

  list_trait.tp = LIST_OBJ;
  list_trait.dstcor = (GC_Dstcor)List_free;
  
  dict_trait.tp = DICT_OBJ;
  dict_trait.dstcor = (GC_Dstcor)Dict_free;
}

Object Object_none() {
  Object obj;
  obj.tp = &none_trait;
  return obj;
}

Object Object_int(long long i) {
  Object obj;
  obj.tp = &int_trait;
  obj.i = i;
  return obj;
}

Object Object_float(double f) {
  Object obj;
  obj.tp = &float_trait;
  obj.f = f;
  return obj;
}

Object Object_String(String *s) {
  Object obj;
  obj.tp = &str_trait;
  obj.s = s;
  return obj;
}

Object Object_List(List *l) {
  Object obj;
  obj.tp = &list_trait;
  obj.l = l;
  return obj;
}

Object Object_Dict(Dict *d) {
  Object obj;
  obj.tp = &dict_trait;
  obj.d = d;
  return obj;
}

size_t Object_get_gcval(Object obj) {
  switch (obj.tp->tp) {
    case INT_OBJ:
    case FLOAT_OBJ:
    case NONE_OBJ:
      return 0;
    case STR_OBJ:
      return obj.s->gc_base;
    case LIST_OBJ:
      return obj.l->gc_base;
    case DICT_OBJ:
      return obj.d->gc_base;
  }
}

String *String_new(char *base) {
  String *str = malloc(sizeof(String));
  str->gc_base = GC_objs_add(GC_Object_new(str, (GC_Dstcor)String_free));
  str->size = strlen(base);
  str->max = 8;
  while (str->max < str->size)
    str->max *= 2;
  str->val = malloc(sizeof(char) * (str->max + 1));
  strcpy(str->val, base);
  return str;
}

void String_append(String *str, char ch) {
  if (str->size == str->max) {
    str->max *= 2;
    str->val = realloc(str->val, sizeof(char) * (str->max + 1));
  }
  str->val[str->size++] = ch;
  str->val[str->size] = 0;
}

void String_cat(String *str, char *other) {
  size_t new_size = str->size + strlen(other);
  if (str->max < new_size) {
    while (str->max < new_size)
      str->max *= 2;
    str->val = realloc(str->val, sizeof(char) * (str->max + 1));
  }
  strcpy(str->val + str->size, other);
  str->size = new_size;
}

void String_free(String *str) {
  printf("String_free %s\n", str->val);
  free(str->val);
  free(str);
}

List *List_new() {
  List *list = malloc(sizeof(List));
  list->gc_base = GC_objs_add(GC_Object_new(list, (GC_Dstcor)List_free));
  list->size = 0;
  list->max = 8;
  list->items = malloc(sizeof(Object) * list->max);
  return list;
}

void List_append(List *list, Object obj) {
  if (list->size == list->max) {
    list->max *= 2;
    list->items = realloc(list->items, sizeof(Object) * list->max);
  }
  size_t gcval = Object_get_gcval(obj);
  if (gcval)
    GC_obj_add_ch(list->gc_base, gcval);
  list->items[list->size++] = obj;
}

void List_free(List *list) {
  puts("List_free");
  free(list->items);
  free(list);
}

DictBase *_Dict_new() {
  DictBase *dict = malloc(sizeof(DictBase));
  dict->has_obj = false;
  dict->chs = malloc(sizeof(DictBase *) * 128);
  for (size_t i = 0; i < 128; i++)
    dict->chs[i] = NULL;
  return dict;
}

Object *_Dict_find(DictBase *dict, char *key) {
  if (!*key) {
    if (dict->has_obj)
      return &dict->obj;
    else
      return NULL;
  }
  if (!dict->chs[*key])
    return NULL;
  return _Dict_find(dict->chs[*key], key + 1);
}

size_t _Dict_insert(DictBase *dict, char *key, Object obj) {
  if (!*key) {
    if (dict->has_obj) {
      size_t tmp = Object_get_gcval(dict->obj);
      dict->obj = obj;
      return tmp;
    }
    dict->has_obj = true;
    dict->obj = obj;
    return 0;
  }
  if (!dict->chs[*key]) {
    dict->chs[*key] = _Dict_new();
  }
  return _Dict_insert(dict->chs[*key], key + 1, obj);
}

void _Dict_free(DictBase *dict) {
  for (size_t i = 0; i < 128; i++)
    if (dict->chs[i]) {
      _Dict_free(dict->chs[i]);
    }
  free(dict);
}

Dict *Dict_new() {
  Dict *dict = malloc(sizeof(Dict));
  DictBase *base = _Dict_new();
  dict->gc_base = GC_objs_add(GC_Object_new(dict, (GC_Dstcor)Dict_free));
  dict->val = base;
  return dict;
}

Object *Dict_find(Dict *dict, char *key) {
  return _Dict_find(dict->val, key);
}

void Dict_insert(Dict *dict, char *key, Object obj) {
  size_t need_disconnect = _Dict_insert(dict->val, key, obj);
  if (need_disconnect != 0)
    GC_obj_remove_ch(dict->gc_base, need_disconnect);
}

void Dict_free(Dict *dict) {
  puts("Dict_free");
  _Dict_free(dict->val);
  free(dict);
}
