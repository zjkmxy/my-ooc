#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "list.h"

static BOOL List_append(PSequence self_seq, int val);
static void List_free(PSequence self_seq);
static PIterator List_begin(PSequence self_seq);

static PListNode ListNode_create(int val);
static PListIt ListIt_create(PListNode ptr);
static BOOL ListIt_isEnd(PIterator self_it);
static int ListIt_getVal(PIterator self_it);
static void ListIt_getNext(PIterator self_it);
static void ListIt_free(PIterator self_it);

PList List_create()
{
  PList ret = (PList)malloc(sizeof(List));
  ret->head = NULL;
  ret->tail = NULL;
  ret->base.size = 0;

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

  for(cur = self->head; cur; cur = self->head)
  {
    self->head = cur->next;
    free(cur);
  }
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

