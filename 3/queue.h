/*3:5*/

#ifndef _QUEUE_H
#define _QUEUE_H

#include "object.h"

/*
 * 本次的问题中，Queue不再是一个需要抽象的内容
 * 因此我们直接构造Queue，不使用Sequence
 */

struct Queue;
typedef struct Queue Queue;
typedef struct Queue *PQueue;
struct ListNode;
typedef struct ListNode ListNode;
typedef struct ListNode *PListNode;

struct Queue
{
  Object base;
  PCClassDesc type; /* 表示Queue中元素的类型 */
  int size;
  PListNode head;
  PListNode tail;
};

extern const ClassDesc Queue_class;

/* 创建一个Queue，元素类型为type */
PQueue Queue_create(PQueue ret, PCClassDesc type);

/*
 * 向Queue中插入一个元素val，
 * 如果该元素不是type类型则失败并返回false
 * 成功返回true
 */
bool Queue_push(PQueue self, PObject val);
PObject Queue_front(PQueue self);

/*
 * 移出队列首的元素。
 * 注意Queue*不会*删除这个元素。
 */
void Queue_pop(PQueue self);
int Queue_size(PQueue self);
PCClassDesc Queue_template(PQueue self);

#endif

