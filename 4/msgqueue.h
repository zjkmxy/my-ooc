/*4:1*/

#ifndef _MSGQUEUE_H
#define _MSGQUEUE_H

#include "object.h"

struct MsgQueue;
typedef struct MsgQueue MsgQueue;
typedef struct MsgQueue *PMsgQueue;

/* 回调函数的类型 */
typedef void (*PCallback)(PObject msg);

extern const ClassDesc MsgQueue_class;

/* 创建新的消息队列 */
PMsgQueue MsgQueue_create(PMsgQueue ret);

/* 注册回调方法，目前还不能重复注册 */
void MsgQueue_register(PMsgQueue self, PCClassDesc type, PCallback func);

/* 向消息队列中存入元素，并托管其内存管理 */
bool MsgQueue_post(PMsgQueue self, PObject msg);

/* 处理消息 */
void MsgQueue_spinOnce(PMsgQueue self);

#endif
