#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "array.h"

static BOOL Array_append(PSequence self_seq, int val);
static void Array_free(PSequence self_seq);
static PIterator Array_begin(PSequence self_seq);

static PArrayIt ArrayIt_create(int* ptr, int* end);
static BOOL ArrayIt_isEnd(PIterator self_it);
static int ArrayIt_getVal(PIterator self_it);
static void ArrayIt_getNext(PIterator self_it);
static void ArrayIt_free(PIterator self_it);

PArray Array_create(int capcity)
{
  PArray ret = (PArray)malloc(sizeof(Array));
  ret->capcity = capcity;
  ret->data = (int*)malloc(sizeof(int) * capcity);
  ret->base.size = 0;

  ret->base.begin = Array_begin;
  ret->base.append = Array_append;
  ret->base.free = Array_free;
  
  return ret;
}

BOOL Array_append(PSequence self_seq, int val)
{
  PArray self = (PArray)self_seq;
  if(self->base.size >= self->capcity)
  {
    return FALSE;
  }
  else
  {
    self->data[self->base.size] = val;
    self->base.size ++;
    return TRUE;
  }
}

void Array_free(PSequence self_seq)
{
  PArray self = (PArray)self_seq;
  free(self->data);
  free(self);
}

PArrayIt ArrayIt_create(int* ptr, int* end)
{
  PArrayIt ret = (PArrayIt)malloc(sizeof(ArrayIt));
  ret->ptr = ptr;
  ret->end = end;
  ret->base.getVal = ArrayIt_getVal;
  ret->base.getNext = ArrayIt_getNext;
  ret->base.isEnd = ArrayIt_isEnd;
  ret->base.free = ArrayIt_free;
  return ret;
}

PIterator Array_begin(PSequence self_seq)
{
  PArray self = (PArray)self_seq;
  PArrayIt ret = ArrayIt_create(self->data, self->data + self->base.size);
  return &ret->base;
}

BOOL ArrayIt_isEnd(PIterator self_it)
{
  PArrayIt self = (PArrayIt)self_it;
  return self->ptr == self->end;
}

int ArrayIt_getVal(PIterator self_it)
{
  PArrayIt self = (PArrayIt)self_it;
  return *self->ptr;
}

void ArrayIt_getNext(PIterator self_it)
{
  PArrayIt self = (PArrayIt)self_it;
  self->ptr ++;
}

void ArrayIt_free(PIterator self_it)
{
  free(self_it);
}

