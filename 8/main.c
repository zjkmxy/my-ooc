/*8:8*/

#include "system_export.h"
#include "integer.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

Handle Fibonacci(int i)
{
  /* 使用二级指针（句柄）后，代码看起来舒服多了 */
  Handle ret;
  MH_enter();
  if(i < 3)
  {
    ret = send(g_Int, SYMBOL_MAKE, 1);
  }
  else
  {
    ret = send(Fibonacci(i - 1), SYMBOL_ADD, Fibonacci(i - 2));
  }
  MH_return(ret);
  return ret;
}

int main()
{
  System_init();
  Int_loadInt(); /* 构造根对象的load函数可以不套在enter-return里 */

  MH_enter();

  send(Fibonacci(25), SYMBOL_PRINT);

  MH_return(NULL);

  System_final();
  return 0;
}

/*
 * 参考阅读：
 * C#通过反射写数据库的例子：http://www.cnblogs.com/binfire/archive/2013/01/17/2864887.html
 * 元语言的例子：http://www.cnblogs.com/weidagang2046/p/the-nature-of-meta.html
 * 以及《七周七语言》里关于Io的探讨。
 */
