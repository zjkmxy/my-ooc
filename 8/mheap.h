/*8:2*/

#ifndef _MHEAP_H
#define _MHEAP_H

#include "object.h"

/* 托管堆垃圾回收算法 */
void MH_collect();

/* 托管堆内存分配 */
PObject MH_alloc(size_t siz);

/* 托管堆对象句柄分配 */
Handle MH_newHandle(PObject obj);

/* 增加栈上句柄 */
void MH_addStackVar(Handle handle);

/* 进入新的函数 */
void MH_enter();

/* 离开函数 */
void MH_leave();

/* 初始化托管堆 */
void MH_init(size_t buflen, size_t var_count);

/* 销毁托管堆 */
void MH_final();

/* 扩展对象 */
void MH_expandObject(Handle obj, size_t new_siz);

#endif
