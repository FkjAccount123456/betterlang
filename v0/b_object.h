#ifndef B_OBJECT_H
#define B_OBJECT_H

typedef void Destructor(void *);
typedef void *Copier(void *);

typedef struct ObjTrait {
  Destructor *destructor;
  Copier *copier;
} ObjTrait;

#endif // B_OBJECT_H
