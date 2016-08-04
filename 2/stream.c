/*2:8*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "stream.h"

/* 未使用的参数 */
#define UNUSED(param) (void)(param)

/*
 * 取得成员偏移，即(&var->member - var)按字节数计量
 */
#define OFFSETOF(type, member) ((size_t) &((type*)0)->member)

/*
 * 通过成员地址取得所在结构体基地址
 * 相当于已知ptr = &var->member，求var
 */
#define CONTAINER_OF(ptr, type, member) \
  (type*)((char*)(ptr) - OFFSETOF(type, member))

/* 以上两个宏在Linux系统内核中被广泛使用 */

static BOOL Stream_append(PSequence self_seq, int val);
static void Stream_free(PSequence self_seq);
static PIterator Stream_begin(PSequence self_seq);

/* Stream既是Sequence，又是Iterator */
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
  /* Stream长度无限，内容固定，无法添加元素 */
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
  /* Stream把自己复制一份 */
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
  /* 通过Iterator得到Stream一定需要CONTAINER_OF宏，因为base_it不是第一个成员 */
  PStream self = CONTAINER_OF(self_it, Stream, base_it);
  return self->x;
}

void Stream_getNext(PIterator self_it)
{
  PStream self = CONTAINER_OF(self_it, Stream, base_it);
  /* 当然我们可以写y=y+x;x=y-x;
   * 然而代码是给人读的，不是给机器读的
   */
  int z = self->x + self->y;
  self->x = self->y;
  self->y = z;
}

void StreamIt_free(PIterator self_it)
{
  PStream self = CONTAINER_OF(self_it, Stream, base_it);
  free(self);
}

