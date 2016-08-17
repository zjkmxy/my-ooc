#include "integer.h"

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

static Handle Int_make(Handle obj, Handle msg_func, va_list args);
static Handle Int_add(Handle obj, Handle msg_func, va_list args);
static Handle Int_print(Handle obj, Handle msg_func, va_list args);

/*
 * ����Intԭ�Ͷ��������������
 * �������ǲ�����Ҫ��ԭʼ�����ˣ��һᾡ�����������һ������
 */
void Int_loadInt()
{
  g_Int = send(g_Object, SYMBOL_CLONE);
  send(g_Int, SYMBOL_SETSLOT, SYMBOL_TYPE, g_Int);
  send(g_Int, SYMBOL_SETSLOT, SYMBOL_MAKE,
    send(g_Block, SYMBOL_MAKE, Int_make, 0));
  send(g_Int, SYMBOL_SETSLOT, SYMBOL_ADD,
    send(g_Block, SYMBOL_MAKE, Int_add, 0));
  send(g_Int, SYMBOL_SETSLOT, SYMBOL_PRINT,
    send(g_Block, SYMBOL_MAKE, Int_print, 0));
}

/************************************************************************/
/* �����ķ���                                                           */
/************************************************************************/
Handle Int_make(Handle obj, Handle msg_func, va_list args)
{
  Handle ret;
  int val;

  MH_enter();
  ret = send(g_Int, SYMBOL_CLONE);
  val = va_arg(args, int);
  Obj_setExtra(ret, val);
  /* ����ֵ����ջ������ʧ */
  MH_return(ret);
  return ret;
}

Handle Int_add(Handle obj, Handle msg_func, va_list args)
{
  Handle ret;
  Handle rhs;

  MH_enter();
  ret = send(g_Int, SYMBOL_CLONE);
  rhs = va_arg(args, Handle);
  Obj_setExtra(ret, Obj_getExtra(obj) + Obj_getExtra(rhs));
  MH_return(ret);
  return ret;
}

Handle Int_print(Handle obj, Handle msg_func, va_list args)
{
  MH_enter();
  printf("%d\n", Obj_getExtra(obj));
  MH_return(NULL);
  return NULL;
}