/*8:7*/

#include "integer.h"

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

static Handle Int_make(Handle obj, Handle msg_func, va_list args);
static Handle Int_add(Handle obj, Handle msg_func, va_list args);
static Handle Int_print(Handle obj, Handle msg_func, va_list args);

/*
 * 创建Int原型对象，增加其基本槽
 * 正常编程时候我们只需要send方法就足够了
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
/* 基本的方法                                                           */
/************************************************************************/
Handle Int_make(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(msg_func);
  UNUSED(obj);

  Handle ret;
  int val;

  MH_enter();
  ret = send(g_Int, SYMBOL_CLONE);
  val = va_arg(args, int);
  Obj_setExtra(ret, val);
  /* 返回值重入栈避免损失 */
  MH_return(ret);
  return ret;
}

Handle Int_add(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(msg_func);

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
  UNUSED(msg_func);
  UNUSED(args);

  MH_enter();
  printf("%d\n", Obj_getExtra(obj));
  MH_return(NULL);
  return NULL;
}
