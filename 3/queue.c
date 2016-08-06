#include "queue.h"
#include <string.h>
#include <stdlib.h>

struct ListNode
{
  PObject val;
  PListNode next;
};

static void Queue_destroy(PObject self_obj)
{
  PListNode cur;
  PQueue self = (PQueue)self_obj;

  for(cur = self->head; cur; cur = self->head)
  {
    self->head = cur->next;
    /* 目前容器不处理内部元素的释放，以后我们会设法处理 */
    free(cur);
  }

  self_obj->class_desc->super_class->destroy(self_obj);
}

const ClassDesc Queue_class = {
  {&Class_class},
  sizeof(Queue),
  Queue_destroy,
  &Object_class
};

PQueue Queue_create(PQueue ret, PCClassDesc type)
{
  ret->type = type;
  ret->head = ret->tail = NULL;
  ret->size = 0;

  return ret;
}

bool Queue_push(PQueue self, PObject val)
{
  PListNode cur;

  if(!Obj_instOf(val, self->type))
    return false;

  cur = (PListNode)malloc(sizeof(ListNode));
  cur->next = NULL;
  cur->val = val;
  if(!self->head)
  {
    self->head = self->tail = cur;
  }
  else
  {
    self->tail->next = cur;
    self->tail = cur;
  }
  self->size ++;

  return true;
}

PObject Queue_front(PQueue self)
{
  assert(self && self->head);
  return self->head->val;
}

void Queue_pop(PQueue self)
{
  assert(self && self->head);
  PListNode cur = self->head;

  self->head = cur->next;
  free(cur);
  self->size --;
}

int Queue_size(PQueue self)
{
  return self->size;
}

PCClassDesc Queue_template(PQueue self)
{
  return self->type;
}

