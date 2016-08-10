/*5:5*/

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

/*
 * 1.引用计数有两种策略，在C中一般用类似COM的内置引用计数，
 * 每个对象自己管理自己的引用；另一种是OpenCV的Ptr的外置
 * 引用计数，对象本身并不处理计数问题，一切交由模板类Ptr<T>
 * 解决。后者因为C语言的结构体没有默认析构而无法在C语言中实行。
 * Cocos2dx的对象系统有一个有趣的实现AutoreleasePool，可以参考。
 *
 * 2.成熟的系统一般都有内置的引用计数，而且一般不会单独使用。
 * 详见下一节。
 *
 * 3.务必注意：对于树形的引用结构，parent指针不参与引用计数。
 * 比如说图层Layer下有若干精灵Sprite，那么Layer对Sprite的引用计数为1，
 * Sprite对Layer没有引用计数。否则它们将互相锁定，永远无法释放。
 */
