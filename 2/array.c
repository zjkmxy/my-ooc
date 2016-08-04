/*2:6*/

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

  /*
   * 值得注意的是，Sequence的size使用了int类型，int默认是有符号的；
   * 而在C/C++标准库中，人们更倾向于使用无符号的size_t，因为长度不可能是负数。
   * 传统的程序设计思想倾向于严格控制内存使用，能用char就不用short。
   * 但是随着内存容量的增加，小规模内存的占用显得不是那么重要。
   * int某种程度上给了我们调试错误的可能：如果我们发现负数的长度，说明出现了问题。
   * 而且至少我们不用担心无符号数比较的差异，以及i --的时候是否会爆0。
   * 实际设计中需要权衡处理。特别是当我们不知道是否有人用了超过2^31大小的数组时。
   */

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

  /*
   * 这里通过函数返回值判断是否出错是最基本的方式。
   * 大部分的OO语言给我们提供了不同的异常处理机制。
   * 本系列并不打算深入探讨这一问题。
   */
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

