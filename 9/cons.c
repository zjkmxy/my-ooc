/*9:2*/

#include "cons.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Handle g_Cons = NULL;

static Handle Cons_make(Handle obj, Handle msg_func, va_list args);
static Handle Cons_makeList(Handle obj, Handle msg_func, va_list args);
static Handle Cons_randInt(Handle obj, Handle msg_func, va_list args);
static Handle Cons_print(Handle obj, Handle msg_func, va_list args);

/*
 * 创建Cons原型对象，增加其基本槽
 */
void Cons_loadCons()
{
  g_Cons = send(g_Object, SYMBOL_CLONE);
  send(g_Cons, SYMBOL_SETSLOT, SYMBOL_TYPE, g_Cons);
  send(g_Cons, SYMBOL_SETSLOT, SYMBOL_MAKE,
    send(g_Block, SYMBOL_MAKE, Cons_make, 0));
  send(g_Cons, SYMBOL_SETSLOT, SYMBOL_MAKE_LIST,
    send(g_Block, SYMBOL_MAKE, Cons_makeList, 0));
  send(g_Cons, SYMBOL_SETSLOT, SYMBOL_RANDINT,
    send(g_Block, SYMBOL_MAKE, Cons_randInt, 0));
  send(g_Cons, SYMBOL_SETSLOT, SYMBOL_PRINT,
    send(g_Block, SYMBOL_MAKE, Cons_print, 0));
}

/************************************************************************/
/* 基本的方法                                                           */
/************************************************************************/
Handle Cons_make(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(obj);
  UNUSED(msg_func);
  
  Handle ret;

  MH_enter();
  ret = send(g_Cons, SYMBOL_CLONE);
  send(ret, SYMBOL_SETSLOT, SYMBOL_CAR, va_arg(args, Handle));
  send(ret, SYMBOL_SETSLOT, SYMBOL_CDR, va_arg(args, Handle));
  MH_return(ret);
  return ret;
}

Handle Cons_makeList(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(obj);
  UNUSED(msg_func);

  Handle ret;
  int cnt;

  MH_enter();
  ret = NULL;
  cnt = va_arg(args, int);
  while(cnt --)
    ret = send(g_Cons, SYMBOL_MAKE, va_arg(args, Handle), ret);
  MH_return(ret);
  return ret;
}

Handle Cons_randInt(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(obj);
  UNUSED(msg_func);
  UNUSED(args);

  Handle ret;
  srand((unsigned int)time(NULL));

  MH_enter();
  ret = send(g_Cons, SYMBOL_MAKE_LIST, 3,
    send(g_Int, SYMBOL_MAKE, rand()),
    send(g_Int, SYMBOL_MAKE, rand()),
    send(g_Int, SYMBOL_MAKE, rand()));
  MH_return(ret);
  return ret;
}

Handle Cons_print(Handle obj, Handle msg_func, va_list args)
{
  UNUSED(msg_func);
  UNUSED(args);

  Handle rest;

  MH_enter();
  for(rest = obj; rest; rest = send(rest, SYMBOL_CDR))
    send(send(rest, SYMBOL_CAR), SYMBOL_PRINT);
  MH_return(obj);
  return obj;
}
