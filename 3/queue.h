#ifndef _QUEUE_H
#define _QUEUE_H

#include "object.h"

struct Queue;
typedef struct Queue Queue;
typedef struct Queue *PQueue;
struct ListNode;
typedef struct ListNode ListNode;
typedef struct ListNode *PListNode;

struct Queue
{
  Object base;
  PCClassDesc type;
  int size;
  PListNode head;
  PListNode tail;
};

extern const ClassDesc Queue_class;

PQueue Queue_create(PQueue ret, PCClassDesc type);
bool Queue_push(PQueue self, PObject val);
PObject Queue_front(PQueue self);
void Queue_pop(PQueue self);
int Queue_size(PQueue size);

#endif

