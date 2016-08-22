/*9:3*/

#include "system_export.h"
#include "integer.h"
#include "cons.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

int main()
{
  System_init();
  Int_loadInt(); /* 构造根对象的load函数可以不套在enter-return里 */
  Cons_loadCons();

  MH_enter();

  /* 抽象类型Number和Printable在这里都是事实上的类型 */
  Handle numList = send(g_Cons, SYMBOL_RANDINT);
  send(numList, SYMBOL_PRINT);

  MH_return(NULL);

  System_final();
  return 0;
}

