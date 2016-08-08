/*4:1*/

/*
 * 很多情况下，我们需要在某个事件被触发的时候执行某些动作。
 * 这个事件的发生通常不是本模块所能控制的。比如说：希望文件
 * 下载完毕后调用我们提供的解压函数，但是文件的下载是其他
 * 线程/进程执行的，主线程会在下载的时候执行其他动作，不可能
 * 一直浪费时间等待下载完成。
 *
 * 这种“将某个函数交给其他模块，并要求其在特定事件触发时被调用”
 * 的函数称为“回调函数”。回调函数机制在各个程序中被广泛使用，用于
 * 处理不同模块之间的耦合，需要耗费时间完成的事，以及不确定发生的事。
 *
 * 几个例子如下：
 * 1.调用CppCheck检查代码，要求其每一个错误后就调用设定好的报告函数。
 *   （模块之间的耦合，不确定的事件）
 * 2.文件读写和网络通信，要求连接建立和传输完成后调用特定的函数。
 *   （耗费时间完成的事）
 * 3.制作UI界面时，用户的输入发生时调用。
 *   （不确定的事件）
 *
 * 特别的如果事件发生的模块和回调模块不在一个线程里，为了避免回调函数打断
 * 原本线程的执行，通常还会选择不立刻执行回调，而是将需要执行的事件回调
 * 放入一个队列中，等待主线程检查这个队列的时候再调用。这种机制称为“消息队列”
 *
 * 消息队列在Windows、XWindow和ROS中使用广泛，关于它的优缺点可以自行搜索。
 * Actor模型是完全建立在消息队列上的一种并发模型。
 */

#ifndef _MSGQUEUE_H
#define _MSGQUEUE_H

#include "object.h"

struct MsgQueue;
typedef struct MsgQueue MsgQueue;
typedef struct MsgQueue *PMsgQueue;

/* 
 * 回调函数的类型
 * msg: 收到的消息
 */
typedef void (*PCallback)(PObject msg);

extern const ClassDesc MsgQueue_class;

/* 创建新的消息队列 */
PMsgQueue MsgQueue_create(PMsgQueue ret);

/* 
 * 注册回调方法，目前还不能重复注册
 * type: 对应的消息类型
 * func: 注册的回调函数
 */
void MsgQueue_register(PMsgQueue self, PCClassDesc type, PCallback func);

/* 
 * 向消息队列中存入元素，并托管其内存管理
 * 注意：消息注册成功后不再需要人工释放内存
 * 但是失败时需要人工释放
 */
bool MsgQueue_post(PMsgQueue self, PObject msg);

/* 
 * 主线程检查消息队列并处理消息
 */
void MsgQueue_spinOnce(PMsgQueue self);

#endif
