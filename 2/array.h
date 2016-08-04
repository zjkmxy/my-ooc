#ifndef _ARRAY_H
#define _ARRAY_H

#include "sequence.h"

struct Array;
typedef struct Array Array;
typedef struct Array *PArray;
struct ArrayIt;
typedef struct ArrayIt ArrayIt;
typedef struct ArrayIt *PArrayIt;

struct ArrayIt
{
  Iterator base;
  int *ptr;
  int *end;
};

struct Array
{
  Sequence base;
  int capcity;
  int *data;
};

PArray Array_create(int capcity);

#endif
