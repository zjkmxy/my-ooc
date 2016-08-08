/*4:2*/

#include "msgqueue.h"
#include "queue.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * 回调函数链表节点
 * 如果可能的话，哈希表会更好
 */
struct MsgQueue_CBNode;
typedef struct MsgQueue_CBNode MsgQueue_CBNode;
typedef struct MsgQueue_CBNode *PMsgQueue_CBNode;

/*
 * 回调事件队列的节点
 */
struct MsgQueue_EventNode;
typedef struct MsgQueue_EventNode MsgQueue_EventNode;
typedef struct MsgQueue_EventNode *PMsgQueue_EventNode;

struct MsgQueue
{
  Object base;

  /* 回调函数的注册表(链表) */
  PMsgQueue_CBNode cb_head;
  PMsgQueue_CBNode cb_tail;

  /* 回调事件的队列 */
  PQueue event_queue;
};

/* 消息队列的析构函数，消灭消息 */
void MsgQueue_destroy(PObject self_obj);

/* 根据消息的类型检出回调函数，失败返回NULL */
PCallback MsgQueue_translate(PMsgQueue self, PCClassDesc type);

const ClassDesc MsgQueue_class =
{
  {&Class_class},
  sizeof(MsgQueue),
  MsgQueue_destroy,
  &Object_class
};

struct MsgQueue_CBNode
{
  PMsgQueue_CBNode next;
  PCClassDesc type;
  PCallback func;
};

struct MsgQueue_EventNode
{
  Object base;

  /* 消息本体，托管内存释放 */
  PObject msg;

  /* 
   * 这里备存一下回调函数，这样当回调方法修改的时候，
   * 以前收到的消息不回受影响。
   */
  PCallback func;
};

/* 消息事件的析构 */
void MsgQueue_EventNode_destroy(PObject self_obj);

/* 消息事件的构造函数 */
PMsgQueue_EventNode MsgQueue_EventNode_create(
  PMsgQueue_EventNode ret,
  PObject msg,
  PCallback func
);

/* 派发一个消息事件 */
void MsgQueue_EventNode_dispatch(PMsgQueue_EventNode self);

const ClassDesc MsgQueue_EventNode_class =
{
  {&Class_class},
  sizeof(MsgQueue_EventNode),
  MsgQueue_EventNode_destroy,
  &Object_class
};

/****************************************************/

PMsgQueue MsgQueue_create(PMsgQueue ret)
{
  ret->cb_head = ret->cb_tail = NULL;
  ret->event_queue = Queue_create(new(Queue), &MsgQueue_EventNode_class);
  return ret;
}

void MsgQueue_register(PMsgQueue self, PCClassDesc type, PCallback func)
{
  PMsgQueue_CBNode cur;
  
  /* 如果当前类型已经有了回调，就覆盖掉 */
  for(cur = self->cb_head; cur; cur = cur->next)
  {
    if(cur->type == type)
    {
      cur->func = func;
      return ;
    }
  }

  /* 否则就创建新的并加入 */
  cur = (PMsgQueue_CBNode)malloc(sizeof(MsgQueue_CBNode));
  cur->type = type;
  cur->func = func;
  cur->next = NULL;

  if(self->cb_head)
  {
    self->cb_tail->next = cur;
    self->cb_tail = cur;
  }
  else
  {
    self->cb_head = self->cb_tail = cur;
  }
}

void MsgQueue_destroy(PObject self_obj)
{
  PMsgQueue self = (PMsgQueue)self_obj;
  PMsgQueue_CBNode cur;
  PObject event;

  /* 清理所有的回调注册节点 */
  for(cur = self->cb_head; cur; cur = self->cb_head)
  {
    self->cb_head = cur->next;
    free(cur);
  }

  /* 因为Queue不托管其元素，故需要人工释放未处理完的消息 */
  while(Queue_size(self->event_queue) > 0)
  {
    event = Queue_front(self->event_queue);
    delete(event);
    Queue_pop(self->event_queue);
  }

  delete(self->event_queue);
}

PCallback MsgQueue_translate(PMsgQueue self, PCClassDesc type)
{
  PMsgQueue_CBNode cur;
  
  if(!type)
    return false;

  /* 先找type，如果存在则返回，不存在则找type的基类，直到根 */
  for(cur = self->cb_head; cur; cur = cur->next)
  {
    if(cur->type == type)
    {
      return cur->func;
    }
  }
  return MsgQueue_translate(self, Class_getBase(type));
}

bool MsgQueue_post(PMsgQueue self, PObject msg)
{
  PCallback func = MsgQueue_translate(
      self,
      Obj_classOf(msg));
  if(func)
  {
    /* 如果存在处理函数，就加入队列，否则退信 */
    Queue_push(
        self->event_queue,
        (PObject)MsgQueue_EventNode_create(
          new(MsgQueue_EventNode),
          msg,
          func)
        );
    return true;
  }
  else
    return false;
}

void MsgQueue_spinOnce(PMsgQueue self)
{
  PMsgQueue_EventNode event;
  while(Queue_size(self->event_queue) > 0)
  {
    event = (PMsgQueue_EventNode)Queue_front(self->event_queue);
    MsgQueue_EventNode_dispatch(event);
    delete(event);
    Queue_pop(self->event_queue);
  }
}

/****************************************************/

void MsgQueue_EventNode_destroy(PObject self_obj)
{
  PMsgQueue_EventNode self = (PMsgQueue_EventNode)self_obj;
  delete(self->msg);
}

PMsgQueue_EventNode MsgQueue_EventNode_create(
  PMsgQueue_EventNode ret,
  PObject msg,
  PCallback func)
{
  ret->msg = msg;
  ret->func = func;
  return ret;
}

void MsgQueue_EventNode_dispatch(PMsgQueue_EventNode self)
{
  self->func(self->msg);
}
