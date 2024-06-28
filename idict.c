#include "idict.h"

IDict *IDict_new() {
  IDict *d = malloc(sizeof(IDict));
  d->ival = d->hasval = 0;
  d->chs = malloc(sizeof(IDict *) * 128);
  return d;
}

unsigned int *IDict_find(IDict *d, char *k) {
  if (!*k) {
    if (d->hasval)
      return d->ival;
    else
      return NULL;
  }
  if (d->chs[*k])
    return IDict_find(d->chs[*k], k + 1);
  else
    return NULL;
}

void IDict_insert(IDict *d, char *k, unsigned int v) {
  if (!*k) {
    d->hasval = true, d->ival = v;
    return;
  }
  if (!d->chs[*k])
    d->chs[*k] = IDict_new();
  IDict_insert(d->chs[*k], k + 1, v);
}

void IDict_free(IDict *d) {
  for (size_t i = 0; i < 128; i++)
    if (d->chs[i])
      IDict_free(d->chs[i]);
  free(d);
}
