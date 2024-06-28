#ifndef IDICT_H
#define IDICT_H

#include "includes.h"

typedef struct IDict {
  unsigned int ival;
  bool hasval;
  struct IDict **chs;
} IDict;

IDict *IDict_new();
unsigned int *IDict_find(IDict *d, char *k);
void IDict_insert(IDict *d, char *k, unsigned int v);
void IDict_free(IDict *d);

#endif // IDICT_H
