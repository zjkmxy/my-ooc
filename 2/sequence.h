/*2:1*/

/*
 * 前一节讨论过，我们的抽象程度还不够好。
 * 观察之前的List和Array，它们共有一组操作接口（同样形式的函数），以及成员变量size。
 * 事实上这是所有Sequence所共有的内容。这提示我们将抽象的Sequence提取出来作为一个类。
 * Array和List默认具有Sequence的所有成员（方法和变量），称为Sequence的“子类”或“派生类”。
 * Sequence是它们的“基类”。显然所有List和Array的实例都是Sequence的实例。
 * 因为XXX_getVal和XXX_getNext总是在一起使用的，我们将这些函数的指针也一并放在基类里。
 * 子类List和Array在各自的构造函数中会把自己的方法实现填入这些函数指针中。
 * 因此对于Sequence来说，它的这些方法只具有一个抽象意义，我们无法从一个Sequence实例
 * 中得出这些方法的具体内容（List_append或Array_append），称为“虚方法”或“虚函数”。
 *
 * 我们的代码已经扩充到了一个相当长的水平，继续放在一个文件里不合适。
 * 因此我们将每个命名空间XXX装入独立的两个文件里：XXX.h和XXX.c。
 * XXX.h中只包括提供给外界所知的类型定义和函数声明，被所有引用到这个命名空间的文件
 * 以#include的形式全文包含。
 * XXX.c中包括所有的函数定义，其中有一部分以static形式定义，只能在该文件内部使用。
 * 外部对成员函数的可见性将成员函数分为了两类：“公有”和“私有”
 */

/*
 * 下面的宏指令避免这个头文件被包含两次
 * 例如b.h和c.h包含a.h，main.cpp包含b.h和c.h
 * 那么main.cpp在遇到b.h的时候就会把b.h的文本插入进去
 * 并且递归地插入a.h，根据下面的宏，定义符号_A_H
 * 遇到c.h的时候再次插入a.h时，因为_A_H已经定义了，所以这个文件被置空
 * 这样就不会得到符号重定义的错误
 */
#ifndef _SEQUENCE_H
#define _SEQUENCE_H

/* ANSI C 没有原生的逻辑类型，需要人工定义 */
typedef int   BOOL;
#define TRUE  1
#define FALSE 0

/* 很多时候，类型前置声明可以让我们的日子更加好过 */
struct Iterator;
typedef struct Iterator Iterator;
typedef struct Iterator *PIterator;
struct Sequence;
typedef struct Sequence Sequence;
typedef struct Sequence *PSequence;

/* 
 * 之前的程序中，实际上使用了ListNode*与int*作为相应的迭代器类，它们也有共同的
 * 基类Iterator。这里我们将所有的类都显式给出，将Sequence和Iterator的职能分工明晰化。
 */
struct Iterator
{
  /* 判断迭代器是否到达终点 */
  BOOL (*isEnd)(PIterator self);
  
  /* 获得迭代器指向的值 */
  int (*getVal)(PIterator self);

  /* 将迭代器改为指向它的后继 */
  void (*getNext)(PIterator self);

  /* 迭代器的析构函数 */
  void (*free)(PIterator self);
};

struct Sequence
{
  /* 元素个数 */
  int size;

  /* 获得开始的迭代器 */
  PIterator (*begin)(PSequence self);

  /* 在结尾追加val，返回是否成功 */
  BOOL (*append)(PSequence self, int val);

  /* 序列的析构函数 */
  void (*free)(PSequence self);
};

/***********这里是华丽的分界线*********************/
/* 上面的成员函数都是虚函数，下面的都是普通成员函数 */
/*************************************************/

/* 获得序列中元素的个数 */
int Seq_getSize(PSequence self);

/*
 * 打印这个序列
 * maxLength: 当序列过长时，只打印前maxLength项
 */
void Seq_print(PSequence self, int maxLength);

#endif
