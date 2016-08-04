/*2:5*/

#ifndef _ARRAY_H
#define _ARRAY_H

#include "sequence.h"

struct Array;
typedef struct Array Array;
typedef struct Array *PArray;
struct ArrayIt;
typedef struct ArrayIt ArrayIt;
typedef struct ArrayIt *PArrayIt;

/* 
 * 数组的实现并没有太多值得一提的地方
 * 迭代器ArrayIt里的end也可以换成指向Array的指针，然后由Array判断是否到达了终点。
 */

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
