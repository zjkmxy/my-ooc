/*8:5*/

#include "integer.h"
#include "object.h"
#include "mheap.h"
#include <memory.h>
#include <stdlib.h>

/* g_全局对象 */
Handle g_Object = NULL;
Handle g_Block = NULL;
Handle g_Int = NULL;

/* gp_全局块对象 */
static Handle gp_slotCnt = NULL;
static Handle gp_getSlot = NULL;
static Handle gp_setSlot = NULL;
static Handle gp_clone = NULL;
static Handle gp_exec = NULL;
static Handle gp_ident = NULL;
static Handle gp_blockMake = NULL;

size_t Obj_slotCnt(PObject obj)
{
  size_t i, ret = 0;
  for(i = 0; i < obj->slot_cap; i ++)
  {
    if(obj->slots[i].name != NIL)
      ret ++;
  }
  return ret;
}

size_t Obj_compressSlot(PObject obj)
{
  size_t ret = 0;
  PSlot from, to, end;

  /* 压缩不用的槽 */
  from = to = obj->slots;
  end = obj->slots + obj->slot_cap;
  for(; from < end; from ++)
  {
    if(from->name != NIL)
    {
      if(from != to)
      {
        to->name = from->name;
        to->val = from->val;
      }
      to ++;
      ret ++;
    }
  }
  for(; to < end; to ++)
  {
    to->name = NIL;
    to->val = NULL;
  }

  /* 如果有必要则减半 */
  if(ret * 2 < obj->slot_cap && obj->slot_cap > DEFAULT_SLOT_CNT)
  {
    obj->slot_cap /= 2;
    obj->size = Obj_calcSize(obj->slot_cap);
  }

  return ret;
}

size_t Obj_calcSize(size_t slotCnt)
{
  return sizeof(Object) + sizeof(Slot) * (slotCnt - 1);
}

/* 创建新的空白对象，完成基本初始化和句柄存栈 */
Handle Obj_newBlankObject()
{
  size_t siz = Obj_calcSize(DEFAULT_SLOT_CNT);
  PObject ret = MH_alloc(siz);
  ret->handle = MH_newHandle(ret);

  ret->exec = NULL;
  ret->size = siz;
  ret->slot_cap = DEFAULT_SLOT_CNT;
  memset(ret->slots, 0, sizeof(Slot) * DEFAULT_SLOT_CNT);

  MH_addStackVar(ret->handle);
  return ret->handle;
}

/* 获得槽，会查找其原形 */
Handle Obj_getSlot(PObject obj, Symbol slot)
{
  PSlot cur, end;
  Handle par;

  while(obj)
  {
    par = NULL;
    end = obj->slots + obj->slot_cap;
    for(cur = obj->slots; cur != end; cur ++)
    {
      if(cur->name == slot)
      {
        return cur->val;
      }
      else if(cur->name == SYMBOL_PROTO)
      {
        par = cur->val;
      }
    }
    obj = *par;
  }
  return NULL;
}

/* 设置槽，不会触及原形 */
void Obj_setSlot(PObject obj, Symbol slot, Handle val)
{
  PSlot cur, end, blank;
  Handle handle;

  assert(obj);
  end = obj->slots + obj->slot_cap;
  blank = NULL;
  /* 已经存在，则改写 */
  for(cur = obj->slots; cur != end; cur ++)
  {
    if(cur->name == slot)
    {
      cur->val = val;
      return ;
    }
    else if(cur->name == NIL && !blank)
    {
      blank = cur;
    }
  }
  /* 无空位则扩展 */
  handle = obj->handle;
  if(!blank)
  {
    MH_expandObject(obj->handle, Obj_calcSize(obj->slot_cap * 2));
    obj = *handle;
    end = obj->slots + obj->slot_cap;
    blank = obj->slots + obj->slot_cap / 2;
    for(cur = blank; cur != end; cur ++)
    {
      cur->name = NIL;
      cur->val = NULL;
    }
  }
  /* 有空位则新建 */
  blank->name = slot;
  blank->val = val;
}

Handle Obj_clone(PObject obj)
{
  size_t siz = Obj_calcSize(DEFAULT_SLOT_CNT);
  PObject ret = MH_alloc(siz);
  ret->handle = MH_newHandle(ret);

  /* 
   * 这里我们允许克隆体像原对象一样可以执行
   * 这种情况应该是很少见的，我并没有深入分析优劣
   * 只是很自然地这么做了而已，未必是很好的做法
   */
  ret->exec = obj->exec;
  ret->extra_data = obj->extra_data;
  ret->size = siz;
  ret->slot_cap = DEFAULT_SLOT_CNT;
  memset(ret->slots, 0, sizeof(Slot)* DEFAULT_SLOT_CNT);

  /* 设置原型 */
  ret->slots[0].name = SYMBOL_PROTO;
  ret->slots[0].val = obj->handle;

  MH_addStackVar(ret->handle);
  return ret->handle;
}

Handle Block_make(MsgFunc msg_func, int trap_cnt, ...)
{
  Handle ret;
  va_list ap;
  Symbol slot;
  Handle val;

  /* 克隆并设置执行体 */
  assert(g_Block);
  ret = Obj_clone(*g_Block);
  (*ret)->exec = msg_func;

  /* 把捕获变量装入槽中 */
  va_start(ap, trap_cnt);
  while(trap_cnt --)
  {
    slot = va_arg(ap, Symbol);
    val = va_arg(ap, Handle);
    Obj_setSlot(*ret, slot, val);
  }
  va_end(ap);

  return ret;
}

Handle Obj_exec(PObject obj, Handle receptor, ...)
{
  va_list ap;
  Handle ret;
  assert(obj);
  assert(obj->exec);

  va_start(ap, receptor);
  ret = obj->exec(receptor, obj->handle, ap);
  va_end(ap);
  return ret;
}

Handle send(Handle obj, Symbol msg, ...)
{
  va_list ap;
  Handle ret = Obj_getSlot(*obj, msg);
  va_start(ap, msg);
  /* 如果是块（方法），就执行，并返回其返回值 */
  if(ret && (*ret)->exec != NULL)
  {
    ret = (*ret)->exec(obj, ret, ap);
  }
  va_end(ap);
  return ret;
}

/************************************************************************/
/* 以下是一些基本方法的MsgFunc版                                        */
/************************************************************************/

Handle Obj_slotCntMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(msg_func);
  UNUSED(args);

  assert(g_Int);
  return send(g_Int, SYMBOL_MAKE, Obj_slotCnt(*obj));
}

Handle Obj_getSlotMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(msg_func);
  Symbol slot = va_arg(args, Symbol);
  return Obj_getSlot(*obj, slot);
}

Handle Obj_setSlotMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(msg_func);
  Symbol slot = va_arg(args, Symbol);
  Handle val = va_arg(args, Handle);
  Obj_setSlot(*obj, slot, val);
  return NULL;
}

Handle Obj_cloneMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(msg_func);
  UNUSED(args);
  return Obj_clone(*obj);
}

Handle Obj_execMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(msg_func);
  Handle receptor = va_arg(args, Handle);
  return (*obj)->exec(receptor, (*obj)->handle, args);
}

Handle Obj_identMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(msg_func);
  UNUSED(args);
  return obj;
}

Handle Block_makeMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(obj);
  UNUSED(msg_func);
  UNUSED(args);

  MsgFunc func;
  Handle ret;
  Symbol slot;
  Handle val;
  int trap_cnt;

  func = va_arg(args, MsgFunc);

  /* 克隆并设置执行体 */
  assert(g_Block);
  ret = Obj_clone(*g_Block);
  (*ret)->exec = func;

  /* 把捕获变量装入槽中 */
  trap_cnt = va_arg(args, int);
  while(trap_cnt --)
  {
    slot = va_arg(args, Symbol);
    val = va_arg(args, Handle);
    Obj_setSlot(*ret, slot, val);
  }

  return ret;
}

void Obj_setExtra(Handle obj, int val)
{
  (*obj)->extra_data = val;
}

int Obj_getExtra(Handle obj)
{
  return (*obj)->extra_data;
}

/************************************************************************/
/* 以下是原型系统的初始化和构成                                         */
/************************************************************************/

/* 
 * 创建Object原型对象和Block原型对象
 * 增加它们的基本槽
 */
void Obj_loadObjectAndBlock()
{
  /* 构造对象 */
  g_Object = Obj_newBlankObject();
  g_Block = Obj_clone(*g_Object);
  Obj_setSlot(*g_Object, SYMBOL_TYPE, g_Object);
  Obj_setSlot(*g_Block, SYMBOL_TYPE, g_Block);

  /* 封装基本方法 */
  gp_slotCnt = Block_make(Obj_slotCntMsgFunc, 0);
  gp_getSlot = Block_make(Obj_getSlotMsgFunc, 0);
  gp_setSlot = Block_make(Obj_setSlotMsgFunc, 0);
  gp_clone = Block_make(Obj_cloneMsgFunc, 0);
  gp_exec = Block_make(Obj_execMsgFunc, 0);
  gp_ident = Block_make(Obj_identMsgFunc, 0);
  gp_blockMake = Block_make(Block_makeMsgFunc, 0);

  /* 将方法作为槽加入原形 */
  Obj_setSlot(*g_Object, SYMBOL_SLOTCNT, gp_slotCnt);
  Obj_setSlot(*g_Object, SYMBOL_GETSLOT, gp_getSlot);
  Obj_setSlot(*g_Object, SYMBOL_SETSLOT, gp_setSlot);
  Obj_setSlot(*g_Object, SYMBOL_CLONE, gp_clone);
  Obj_setSlot(*g_Object, SYMBOL_IDENT, gp_ident);

  Obj_setSlot(*g_Block, SYMBOL_EXEC, gp_exec);
  Obj_setSlot(*g_Block, SYMBOL_CLONE, gp_ident); /* Block是单例对象 */
  Obj_setSlot(*g_Block, SYMBOL_MAKE, gp_blockMake);

  /* 
   * 这里关于“单例”的处理有些奇特：
   * 一般来说，我们只要让clone指向自己就能实现单例模式
   * 但是这里Block是非正常手段克隆的，而且我们希望Block的所有
   * 实例（克隆体）也都是单例模式
   * 于是我们将clone指向了一个返回self的方法
   */
}

void System_init()
{
  MH_init(65536, 65536);
  Obj_loadObjectAndBlock();
}

void System_final()
{
  MH_final();
}
