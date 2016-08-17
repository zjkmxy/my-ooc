/*7:3*/

#include "system_export.h"
#include "integer.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

Handle Fibonacci(int i)
{
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
  Int_loadInt();

  MH_enter();

  send(Fibonacci(25), SYMBOL_PRINT);

  MH_return(NULL);

  System_final();
  return 0;
}