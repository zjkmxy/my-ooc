/*2:3*/

#ifndef _LIST_H
#define _LIST_H

#include "sequence.h"

/* 
 * 同样对于链表来说，我们也把迭代器和节点区别开来，
 * 这样迭代器的构造、析构和修改都是独立的，不会影响节点。
 */

struct List;
typedef struct List List;
typedef struct List *PList;
struct ListNode;
typedef struct ListNode ListNode;
typedef struct ListNode *PListNode;
struct ListIt;
typedef struct ListIt ListIt;
typedef struct ListIt *PListIt;

/* 链表迭代器 */
struct ListIt
{
  /* 
   * 迭代器基类，值得注意的是这里没有使用指针
   * 此外，所有的基类应该作为第一个成员出现
   * 原因之后再提
   */
  Iterator base;

  /* 指向具体节点的指针 */
  PListNode ptr;
};

/* 链表节点 */
struct ListNode
{
  int val;
  PListNode next;
};

/* 链表类 */
struct List
{
  /* 序列基类 */
  Sequence base;
  PListNode head;
  PListNode tail;
};

/* 链表的构造函数 */
PList List_create();

#endif
