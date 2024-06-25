#include "obj.h"

GCTrait str_trait, list_trait, dict_trait, vmframe_trait, func_trait;

void GCTraits_init() {
  str_trait.tp = STR_OBJ;
  str_trait.copier = (Copier)String_copy;
  str_trait.dstcor = (Dstcor)String_free;

  list_trait.tp = LIST_OBJ;
  list_trait.copier = (Copier)List_copy;
  list_trait.dstcor = (Dstcor)List_free;

  dict_trait.tp = DICT_OBJ;
  dict_trait.copier = (Copier)Dict_copy;
  dict_trait.dstcor = (Dstcor)Dict_free;

  vmframe_trait.tp = VMFRAME_OBJ;
  vmframe_trait.copier = NULL;
  vmframe_trait.dstcor = (Dstcor)VMFrame_free;

  func_trait.tp = FUNC_OBJ;
  func_trait.copier = NULL;
  func_trait.dstcor = (Dstcor)Func_free;
}

Object Object_Null() {
  Object obj;
  obj.tp = NULL_OBJ;
  return obj;
}

Object Object_int(long long int_obj) {
  Object obj;
  obj.tp = INT_OBJ;
  obj.int_obj = int_obj;
  return obj;
}

Object Object_float(double float_obj) {
  Object obj;
  obj.tp = FLOAT_OBJ;
  obj.float_obj = float_obj;
  return obj;
}

Object Object_builtin(Builtin builtin) {
  Object obj;
  obj.tp = BUILTIN_OBJ;
  obj.builtin_obj = builtin;
  return obj;
}

Object Object_gc(GCPtr *gc_obj) {
  Object obj;
  obj.tp = gc_obj->ptr->trait->tp;
  obj.gc_obj = gc_obj;
  return obj;
}

void Object_free(Object obj) {
  if (obj.tp > NULL_OBJ) {
    GCPtr_free(obj.gc_obj);
  }
}

Object Object_pass(Object base) {
  Object obj = base;
  if (obj.tp > NULL_OBJ) {
    obj.gc_obj = GCPtr_pass(obj.gc_obj);
  }
  return obj;
}

Object Object_copy(Object base) {
  Object obj = base;
  if (obj.tp > NULL_OBJ) {
    obj.gc_obj = GCPtr_copy(obj.gc_obj);
  }
  return obj;
}

void Object_connect(GCPtr *base, Object obj) {
  if (obj.tp > NULL_OBJ) {
    GCPtr_addch(base, obj.gc_obj);
  }
}

void Object_disconnect(GCPtr *base, Object obj) {
  if (obj.tp > NULL_OBJ) {
    GCPtr_remch(base, obj.gc_obj);
  }
}

String *_String_new(char *base) {
  String *str = malloc(sizeof(String));
  str->size = strlen(base);
  str->max = 8;
  while (str->max < str->size)
    str->max *= 2;
  str->val = malloc(sizeof(char) * (str->max + 1));
  strcpy(str->val, base);
  return str;
}

GCPtr *String_new(char *base) {
  return GCPtr_new(GCLeaf_new(&str_trait, _String_new(base)));
}

GCPtr *String_copy(String *base) { return String_new(base->val); }

void String_append(String *str, char c) {
  if (str->size == str->max) {
    str->max *= 2;
    str->val = realloc(str->val, sizeof(char) * (str->max + 1));
  }
  str->val[str->size++] = c;
  str->val[str->size] = '\0';
}

void String_free(String *str) {
  free(str->val);
  free(str);
}

List *_List_new() {
  List *list = malloc(sizeof(List));
  list->size = 0;
  list->max = 8;
  list->items = malloc(sizeof(Object) * list->max);
  return list;
}

GCPtr *List_new() {
  return GCPtr_new(GCLeaf_new(&list_trait, _List_new()));
}

GCPtr *List_copy(List *base) {
  List *list = malloc(sizeof(List));
  list->size = base->size;
  list->max = base->max;
  list->items = malloc(sizeof(Object) * list->max);
  for (int i = 0; i < list->size; i++) {
    list->items[i] = Object_copy(base->items[i]);
  }
  return GCPtr_new(GCLeaf_new(&list_trait, list));
}

void List_append(Object obj, Object item) {
  List *list = obj.gc_obj->ptr->ptr;
  if (list->size == list->max) {
    list->max *= 2;
    list->items = realloc(list->items, sizeof(Object) * list->max);
  }
  list->items[list->size++] = obj;
  GCPtr_addch(obj.gc_obj, item.gc_obj);
}

void List_free(List *list) {
  for (int i = 0; i < list->size; i++) {
    Object_free(list->items[i]);
  }
  free(list->items);
  free(list);
}

Dict *_Dict_new() {
  Dict *dict = malloc(sizeof(Dict));
  dict->children = calloc(128, sizeof(Dict *));
  dict->has_val = false;
  return dict;
}

GCPtr *Dict_new() {
  return GCPtr_new(GCLeaf_new(&dict_trait, _Dict_new()));
}

Dict *_Dict_copy(Dict *base) {
  Dict *dict = malloc(sizeof(Dict));
  dict->children = calloc(128, sizeof(Dict *));
  dict->has_val = base->has_val;
  if (dict->has_val) {
    dict->val = Object_copy(base->val);
  }
  for (int i = 0; i < 128; i++) {
    if (base->children[i]) {
      dict->children[i] = _Dict_copy(base->children[i]);
    }
  }
  return dict;
}

GCPtr *Dict_copy(Dict *base) {
  return GCPtr_new(GCLeaf_new(&dict_trait, _Dict_copy(base)));
}

Object *Dict_find(Dict *dict, char *key) {
  if (!*key) {
    if (dict->has_val)
      return &dict->val;
    else
      return NULL;
  }
  if (dict->children[*key])
    return Dict_find(dict->children[*key], key + 1);
  else
    return NULL;
}

void _Dict_insert(Dict *dict, char *key, Object val) {
  if (!*key) {
    if (dict->has_val) {
      Object_free(dict->val);
    } else {
      dict->has_val = true;
    }
    dict->val = val;
    return;
  }
  if (!dict->children[*key])
    dict->children[*key] = _Dict_new();
  _Dict_insert(dict->children[*key], key + 1, val);
}

void Dict_insert(Object dict, char *key, Object val) {
  _Dict_insert(dict.gc_obj->ptr->ptr, key, val);
  GCPtr_addch(dict.gc_obj, val.gc_obj);
}

void Dict_free(Dict *dict) {
  if (dict->has_val) {
    Object_free(dict->val);
  }
  for (int i = 0; i < 128; i++) {
    if (dict->children[i]) {
      Dict_free(dict->children[i]);
    }
  }
  free(dict->children);
  free(dict);
}

size_t Dict_count_size(Object dict) {
  size_t count = 0;
  for (GCList *ch = dict.gc_obj->chs; ch; ch = ch->next) {
    count++;
  }
  return count;
}

VMFrame *_VMFrame_new(VMFrame *parent) {
  VMFrame *frame = malloc(sizeof(VMFrame));
  frame->parent = parent;
  frame->varlist = _List_new();
  return frame;
}

Object VMFrame_new(VMFrame *parent) {
  return Object_gc(GCPtr_new(GCLeaf_new(&vmframe_trait, parent)));
}

void VMFrame_free(VMFrame *frame) {
  List_free(frame->varlist);
  free(frame);
}

bool Object_to_bool(Object o) {
  switch (o.tp) {
  case NULL_OBJ:
    return 0;
  case INT_OBJ:
    return o.int_obj;
  case FLOAT_OBJ:
    return o.float_obj;
  case STR_OBJ:
    return ObjTypePtr(String, o)->size;
  case LIST_OBJ:
    return ObjTypePtr(List, o)->size;
  case DICT_OBJ:
    return Dict_count_size(o);
  default:
    printf("Unknown object type.");
    exit(-1);
  }
}

char Object_cmp(Object a, Object b) {
  switch (a.tp) {
  case STR_OBJ:
    return strcmp(ObjTypePtr(String, a)->val, ObjTypePtr(String, b)->val);
  default:
    printf("Unknown object type.");
    exit(-1);
  }
}

GCPtr *Func_new(size_t pc, Object frame) {
  Func *func = malloc(sizeof(Func));
  func->frame = Object_pass(frame);
  func->pc = pc;
  return GCPtr_new(GCLeaf_new(&func_trait, func));
}

void Func_free(Func *f) {
  Object_free(f->frame);
  free(f);
}
