/*2:4*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

/* 
 * List类的成员方法/虚方法
 *
 * 每个List类的实例中，其base的所有函数指针指向的都是这些方法，
 * 即使它们是被作为Sequence对待。
 * 这些方法“覆盖”（override）了基类对应的虚方法。
 * （当然在我们的例子中，基类并没有实现任何虚方法）
 *
 * 函数为了和基类的声明相兼容，只能采用PSequence的参数，函数体内部再转化成PList。
 * 在运行时刻，一个List对象不仅是List类的实例，也是Sequence类的实例。
 * 并且即使成为了Sequence类的实例，当需要调用虚方法的时候，调用的依然是下面这些List
 * 类的方法实现。这种现象称为“多态”。
 * 之后我们会看到，在不同的OO模型和不同语言中，多态一词具有不同的内涵。
 * 不过对于C++/Java系语言来说，多态一定包含这样的基本含义。
 */
static BOOL List_append(PSequence self_seq, int val);
static void List_free(PSequence self_seq);
static PIterator List_begin(PSequence self_seq);

/*
 * C源文件中的static修饰符可以防止其他文件使用改方法
 * 因此所有在这里声明的方法都是私有（private）的
 * 公有（public）的方法在头文件中不带static声明
 */

static PListNode ListNode_create(int val);
static PListIt ListIt_create(PListNode ptr);
static BOOL ListIt_isEnd(PIterator self_it);
static int ListIt_getVal(PIterator self_it);
static void ListIt_getNext(PIterator self_it);
static void ListIt_free(PIterator self_it);

PList List_create()
{
  /* 分配空间 */
  PList ret = (PList)malloc(sizeof(List));

  /* 调用基类构造函数 */
  /* Sequence类并没有构造函数，如果有的话，它不应该分配任何空间，
   * 而是应该用我们分配的ret来初始化基类的成员 */

  /* 初始化成员变量，包括基类的成员变量，如果有必要的话 */
  ret->head = NULL;
  ret->tail = NULL;
  ret->base.size = 0;

  /* 
   * 显式方法重载
   * 
   * 也许我们可以使用这样的宏：
   * #define OVERRIDE(ptr, class, method) \
   *     ptr->base.method = class##_##method
   * 然后写OVERRIDE(ret, List, begin);
   * 但是个人感觉C语言滥用宏不好，就没这么写。
   * 这种把一个复杂实现转化为简单语法的工作称为“语法糖”
   */
  ret->base.begin = List_begin;
  ret->base.free = List_free;
  ret->base.append = List_append;

  return ret;
}

PListNode ListNode_create(int val)
{
  PListNode ret = (PListNode)malloc(sizeof(ListNode));
  ret->next = NULL;
  ret->val = val;
  return ret;
}

PListIt ListIt_create(PListNode ptr)
{
  PListIt ret = (PListIt)malloc(sizeof(ListIt));
  ret->ptr = ptr;

  ret->base.getVal = ListIt_getVal;
  ret->base.isEnd = ListIt_isEnd;
  ret->base.getNext = ListIt_getNext;
  ret->base.free = ListIt_free;

  return ret;
}

BOOL List_append(PSequence self_seq, int val)
{
  /*
   * 因为List的第一个成员就是Sequence，因此self->base和self地址相同
   * 所以当我们确定一个基类指针指向的是子类的对象的时候，
   * 可以直接把基类的指针强制转化成子类的指针。
   */
  PList self = (PList)self_seq;
  PListNode cur = ListNode_create(val);

  if(!self->head)
  {
    self->head= self->tail = cur;
  }
  else
  {
    self->tail->next = cur;
    self->tail = cur;
  }
  self->base.size ++;
  return TRUE;
}

void List_free(PSequence self_seq)
{
  PList self = (PList)self_seq;
  PListNode cur;

  /* 释放所有节点 */
  for(cur = self->head; cur; cur = self->head)
  {
    self->head = cur->next;
    free(cur);
  }

  /* 调用基类析构函数 */

  /* 释放自身内存 */
  free(self);
}

PIterator List_begin(PSequence self_seq)
{
  PList self = (PList)self_seq;
  return (PIterator)ListIt_create(self->head);
}

BOOL ListIt_isEnd(PIterator self_it)
{
  PListIt self = (PListIt)self_it;
  return !self->ptr;
}

int ListIt_getVal(PIterator self_it)
{
  PListIt self = (PListIt)self_it;
  return self->ptr->val;
}

void ListIt_getNext(PIterator self_it)
{
  PListIt self = (PListIt)self_it;
  self->ptr = self->ptr->next;
}

void ListIt_free(PIterator self_it)
{
  free(self_it);
}

