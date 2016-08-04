/*2:7*/

/*
 * 假设需求突然变更了，某个模块需要调用你的打印函数来打印一个Stream
 * Stream是一个无穷序列，它平时并不保存特定的值，而只保存某种求值方式
 * 当需要获得某个位置的值时才去计算它。例如Fibonacci数列。
 * 然而我们并不需要重新写一套算法，只要让Stream成为Sequence的子类就好了。
 * 后者比前者容易得多。
 */

#ifndef _STREAM_H
#define _STREAM_H

#include "sequence.h"

struct Stream;
typedef struct Stream Stream;
typedef struct Stream *PStream;

/*
 * 这里我们选择了另一种方式：让Stream同时有两个基类Sequence和Iterator。
 * 多继承是一个有争议的话题，在一般的程序中我们依然建议在可能的情况下使用单继承。
 * 这里只是想用一个不是很好的示例说明我们应该怎么做而已。
 */
struct Stream
{
  Sequence base_seq;
  Iterator base_it;

  /* 这里的Stream采用Fibonacci的例子，x表示当前值，y表示下一个值 */
  /* 当然再往后就是x+y, x+2y, 2x+3y, 3x+5y, ... */
  int x;
  int y;
};

PStream Stream_create(int x, int y);

#endif
