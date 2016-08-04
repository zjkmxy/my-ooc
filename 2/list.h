#ifndef _LIST_H
#define _LIST_H

#include "sequence.h"

struct List;
typedef struct List List;
typedef struct List *PList;
struct ListNode;
typedef struct ListNode ListNode;
typedef struct ListNode *PListNode;
struct ListIt;
typedef struct ListIt ListIt;
typedef struct ListIt *PListIt;

struct ListIt
{
  Iterator base;
  PListNode ptr;
};

struct ListNode
{
  int val;
  PListNode next;
};

struct List
{
  Sequence base;
  PListNode head;
  PListNode tail;
};

PList List_create();

#endif
