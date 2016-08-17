/*8:1*/

#ifndef _OBJECT_H
#define _OBJECT_H

#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include "system_export.h"

struct Slot
{
  Symbol name;
  Handle val;
};

struct Object
{
  /* 对象大小 */
  size_t size;

  /* 执行体，供闭包用 */
  MsgFunc exec;

  /* 唯一的句柄 */
  Handle handle;

  /* 访问标志 */
  bool access;

  /* 额外数据 */
  int extra_data;

  /* 槽位个数(空白槽也算) */
  size_t slot_cap;

  /*
  * 槽位数组
  * 特殊槽位如下：
  * type     : 对象的类型
  * proto    : 该对象的原形
  * clone    : 克隆操作
  * slotCnt  : 槽的个数
  * slotNames: 数组，表示每个槽的符号(未实现)
  * getSlot  : 返回某个槽
  * setSlot  : 设置某个槽
  * ident    : 返回自己
  *
  * 槽位不够时，自动申请倍增扩展
  * 槽位小于四分之一且垃圾回收时，自动半减
  * 有些槽的设定和Io差别很大，比如proto和type
  * 在Io里，它们是方法槽；而这里它们是数据槽
  */
  Slot slots[1];
};

size_t Obj_slotCnt(PObject obj);
size_t Obj_compressSlot(PObject obj);
size_t Obj_calcSize(size_t slotCnt);

Handle Obj_getSlot(PObject obj, Symbol slot);
void Obj_setSlot(PObject obj, Symbol slot, Handle val);
Handle Obj_clone(PObject obj);

Handle Obj_exec(PObject obj, Handle receptor, ...);
Handle Obj_identMsgFunc(Handle obj, Handle msg_func, va_list args);

#endif
