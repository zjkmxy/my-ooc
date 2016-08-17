/*7:2*/

#include "integer.h"
#include "object.h"
#include "mheap.h"
#include <memory.h>
#include <stdlib.h>

/* g_ȫ�ֶ��� */
Handle g_Object = NULL;
Handle g_Block = NULL;
Handle g_Int = NULL;

/* gp_ȫ�ֿ���� */
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

  /* ѹ�����õĲ� */
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

  /* ����б�Ҫ����� */
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

/* �����µĿհ׶�����ɻ�����ʼ���;����ջ */
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

/* ��òۣ��������ԭ�� */
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

/* ���òۣ����ᴥ��ԭ�� */
void Obj_setSlot(PObject obj, Symbol slot, Handle val)
{
  PSlot cur, end, blank;
  Handle handle;

  assert(obj);
  end = obj->slots + obj->slot_cap;
  blank = NULL;
  /* �Ѿ����ڣ����д */
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
  /* �޿�λ����չ */
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
  /* �п�λ���½� */
  blank->name = slot;
  blank->val = val;
}

Handle Obj_clone(PObject obj)
{
  size_t siz = Obj_calcSize(DEFAULT_SLOT_CNT);
  PObject ret = MH_alloc(siz);
  ret->handle = MH_newHandle(ret);

  /* 
   * �������������¡����ԭ����һ������ִ��
   * �������Ӧ���Ǻ��ټ��ģ��Ҳ�û�������������
   * ֻ�Ǻ���Ȼ����ô���˶��ѣ�δ���Ǻܺõ�����
   */
  ret->exec = obj->exec;
  ret->size = siz;
  ret->slot_cap = DEFAULT_SLOT_CNT;
  memset(ret->slots, 0, sizeof(Slot)* DEFAULT_SLOT_CNT);

  /* ����ԭ�� */
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

  /* ��¡������ִ���� */
  assert(g_Block);
  ret = Obj_clone(*g_Block);
  (*ret)->exec = msg_func;

  /* �Ѳ������װ����� */
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
  /* ����ǿ飨����������ִ�У��������䷵��ֵ */
  if(ret && (*ret)->exec != NULL)
  {
    ret = (*ret)->exec(obj, ret, ap);
  }
  va_end(ap);
  return ret;
}

/************************************************************************/
/* ������һЩ����������MsgFunc��                                        */
/************************************************************************/

Handle Obj_slotCntMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  /*TODO*/
  return NULL;
}

Handle Obj_getSlotMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  Symbol slot = va_arg(args, Symbol);
  return Obj_getSlot(*obj, slot);
}

Handle Obj_setSlotMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  Symbol slot = va_arg(args, Symbol);
  Handle val = va_arg(args, Handle);
  Obj_setSlot(*obj, slot, val);
  return NULL;
}

Handle Obj_cloneMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  return Obj_clone(*obj);
}

Handle Obj_execMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  Handle receptor = va_arg(args, Handle);
  return (*obj)->exec(receptor, (*obj)->handle, args);
}

Handle Obj_identMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  return obj;
}

Handle Block_makeMsgFunc(Handle obj, Handle msg_func, va_list args)
{
  MsgFunc func;
  Handle ret;
  Symbol slot;
  Handle val;
  int trap_cnt;

  func = va_arg(args, MsgFunc);

  /* ��¡������ִ���� */
  assert(g_Block);
  ret = Obj_clone(*g_Block);
  (*ret)->exec = func;

  /* �Ѳ������װ����� */
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
/* ������ԭ��ϵͳ�ĳ�ʼ���͹���                                         */
/************************************************************************/

/* 
 * ����Objectԭ�Ͷ����Blockԭ�Ͷ���
 * �������ǵĻ�����
 */
void Obj_loadObjectAndBlock()
{
  /* ������� */
  g_Object = Obj_newBlankObject();
  g_Block = Obj_clone(*g_Object);
  Obj_setSlot(*g_Object, SYMBOL_TYPE, g_Object);
  Obj_setSlot(*g_Block, SYMBOL_TYPE, g_Block);

  /* ��װ�������� */
  gp_slotCnt = Block_make(Obj_slotCntMsgFunc, 0);
  gp_getSlot = Block_make(Obj_getSlotMsgFunc, 0);
  gp_setSlot = Block_make(Obj_setSlotMsgFunc, 0);
  gp_clone = Block_make(Obj_cloneMsgFunc, 0);
  gp_exec = Block_make(Obj_execMsgFunc, 0);
  gp_ident = Block_make(Obj_identMsgFunc, 0);
  gp_blockMake = Block_make(Block_makeMsgFunc, 0);

  /* ��������Ϊ�ۼ���ԭ�� */
  Obj_setSlot(*g_Object, SYMBOL_SLOTCNT, gp_slotCnt);
  Obj_setSlot(*g_Object, SYMBOL_GETSLOT, gp_getSlot);
  Obj_setSlot(*g_Object, SYMBOL_SETSLOT, gp_setSlot);
  Obj_setSlot(*g_Object, SYMBOL_CLONE, gp_clone);
  Obj_setSlot(*g_Object, SYMBOL_IDENT, gp_ident);

  Obj_setSlot(*g_Block, SYMBOL_EXEC, gp_exec);
  Obj_setSlot(*g_Block, SYMBOL_CLONE, gp_ident); /* Block�ǵ������� */
  Obj_setSlot(*g_Block, SYMBOL_MAKE, gp_blockMake);
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