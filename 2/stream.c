/*2:8*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "stream.h"

#define UNUSED(param) (void)(param)
#define OFFSETOF(type, member) ((size_t) &((type*)0)->member)
#define CONTAINER_OF(ptr, type, member) \
  (type*)((char*)(ptr) - OFFSETOF(type, member))

static BOOL Stream_append(PSequence self_seq, int val);
static void Stream_free(PSequence self_seq);
static PIterator Stream_begin(PSequence self_seq);

static BOOL Stream_isEnd(PIterator self_it);
static int Stream_getVal(PIterator self_it);
static void Stream_getNext(PIterator self_it);
static void StreamIt_free(PIterator self_it);

PStream Stream_create(int x, int y)
{
  PStream ret = (PStream)malloc(sizeof(Stream));
  ret->x = x;
  ret->y = y;
  ret->base_seq.size = INT_MAX;

  ret->base_seq.begin = Stream_begin;
  ret->base_seq.append = Stream_append;
  ret->base_seq.free = Stream_free;

  ret->base_it.free = StreamIt_free;
  ret->base_it.isEnd = Stream_isEnd;
  ret->base_it.getVal = Stream_getVal;
  ret->base_it.getNext = Stream_getNext;

  return ret;
}

BOOL Stream_append(PSequence self_seq, int val)
{
  UNUSED(self_seq);
  UNUSED(val);
  return FALSE;
}

void Stream_free(PSequence self_seq)
{
  free(self_seq);
}

PIterator Stream_begin(PSequence self_seq)
{
  PStream self = (PStream)self_seq;
  return &Stream_create(self->x, self->y)->base_it;
}

BOOL Stream_isEnd(PIterator self_it)
{
  UNUSED(self_it);
  return FALSE;
}

int Stream_getVal(PIterator self_it)
{
  PStream self = CONTAINER_OF(self_it, Stream, base_it);
  return self->x;
}

void Stream_getNext(PIterator self_it)
{
  PStream self = CONTAINER_OF(self_it, Stream, base_it);
  int z = self->x + self->y;
  self->x = self->y;
  self->y = z;
}

void StreamIt_free(PIterator self_it)
{
  PStream self = CONTAINER_OF(self_it, Stream, base_it);
  free(self);
}

