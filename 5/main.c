/*4:3*/

#include <stdio.h>
#include "mystring.h"

int main()
{
  PString str1 = Str_create(new(String), "str1");

  /* 下面这句并不复制任何内容 */
  PString str2 = Str_clone(str1);

  /* 下面这句才真正复制整个字符串 */
  Str_setAt(&str2, 3, '2');

  Str_print(str1);
  Str_print(str2);

  release(str1);
  release(str2);

  return 0;
}

