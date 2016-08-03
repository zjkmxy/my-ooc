/*1:1*/

/*
 * 假设你需要为软件写一个写一个模块，它负责求两个序列的公共子序列并输出。
 * （为简化问题，这里我们只实现输出模块）
 * 然而，你的同僚采用了两种不同的数据结构：一个采用了链表，一个采用数组。
 * 一部分数据是从文件中读入的，所以选择数组；而另一部分是从互联网服务器得到的，
 * 服务器断可能随时增减某些项，所以选择链表。两个选择都是合理的，无法统一。
 * 然而我们不可能把一套算法写两个不同的版本：事实上整个算法99%的操作是相同的。
 * 算法可能变化，模块也可能添加新的功能，
 * 如果所有的变更都需要维护两个几乎相同的版本会增加出错的可能。
 * 
 * 观察这个问题，数组和链表对我们意味着：
 * 1.获得起始位置（下标或是节点指针）；
 * 2.知道我们是否到达了终点（最大下标或链表尾）
 * 3.从当前位置到达下一个位置（下标+1,或是指向下一个节点）
 * 4.获得当前位置的值
 *
 * 我们的算法不需要知道链表和数组怎样实现这些操作，
 * 事实上，只需要“它们自己知道自己怎样工作”就行了。
 * 为此我们进行如下编码：
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* 不使用的参数 */
#define UNUSED(name) ((void)(name))

/* List begin */

/*
 * 链表区
 * 因为链表和数组提供相同的功能，也就是说我们定义了同名函数。
 * 因此为了避免冲突，我们把同样范畴的内容放在一起，并在前面加上共同的名字前缀。
 * 如：List（结构体）和List_（函数）
 * 我们将这个前缀称为“命名空间”
 */

/* 表示链表节点的结构体 */
typedef struct ListNode
{
  int val;                /* 当前节点的值 */
  struct ListNode *next;  /* 下一个节点 */
} ListNode, *PListNode;

/*
 * 显然每个链表都具有同样的结构，因此我们定义一个结构体代表它。
 * 不仅如此，它们还具有同样的一套以List_开头的函数（后面定义）
 * 而且我们几乎只会通过这一套函数访问所有的链表。
 * 这种情况下，我们称这个结构体List为“类”，
 * 结构体的每一个变量称为类的“成员变量”
 * 这一套函数称为类的“成员函数”或“成员方法”
 * 将每一个具体的List变量（实际上是指针）称为List类的“实例”
 * （注意：不同的面向对象理念可能会给上述概念起不同的名字，如“槽”或者“消息”）
 * （那些是后话，这里我们姑且采用C++和Java的用语体系）
 */
typedef struct List
{
  int length;     /*链表长度*/
  PListNode head; /*链表头*/
  PListNode tail; /*链表尾*/
} List, *PList;

/*
 * 创建一个空链表
 * 不止为List分配内存，还负责初始化所有的成员
 * 在当前情况下，我们只使用这个函数创造List的实例
 * 我们将其称为“构造函数”
 */
PList List_create()
{
  PList ret = (PList)malloc(sizeof(List));
  ret->length = 0;
  ret->head = NULL;
  ret->tail = NULL;
  return ret;
}

/*
 * 成员函数，在链表后追加元素
 * self: 指向当前正在操作的链表本身的指针
 * val : 新插入的元素
 */
void List_append(PList self, int val)
{
  PListNode cur = (PListNode)malloc(sizeof(ListNode));
  cur->val = val;
  cur->next = NULL;

  if(!self->head)
  {
    self->head = self->tail = cur;
  }
  else
  {
    self->tail->next = cur;
    self->tail = cur;
  }
  self->length ++;
}

/*
 * 释放整个链表，清空所有内容
 * 链表指针只有通过这个函数释放才是安全的
 * 我们将其称为“析构函数”
 */
void List_free(PList self)
{
  PListNode cur;

  for(cur = self->head; cur; cur = self->head)
  {
    self->head = cur->next;
    free(cur);
  }
  free(self);
}

/* 
 * 获得链表的头
 * 为了和数组达成一致，这里我们把指针转换成void*返回
 */
void* List_begin(PList self)
{
  return self->head;
}

/*
 * 获得链表的尾，注意为了方便，一般情况下end返回的指针并不指向区间中的元素
 * 例如：{2,3,4,5}，头指向2,尾指向5之后的下一个
 */
void* List_end(PList self)
{
  UNUSED(self);
  return NULL;
}

/*
 * 获得链表节点的值
 * 这个函数是链表节点ListNode的成员方法，不是List的成员方法
 * 当然，如果知道self是个ListNode结构体，我们可以直接解引用。
 * 但是在算法中我们并不清楚我们操作的对象是否是链表，
 * 因此我们需要让链表节点自己知道如何取得自身的数据。
 */
int ListNode_getVal(void* self)
{
  if(!self)
  {
    return 0;
  }
  else
  {
    return ((PListNode)self)->val;
  }
}

/*
 * 获得下一个节点（后继）
 */
void* ListNode_getNext(void* self)
{
  if(!self)
  {
    return NULL;
  }
  else
  {
    return ((PListNode)self)->next;
  }
}

/* List ends */

/* Array begins */

/* 现在进入Array命名空间 */

typedef int* ArrayIt; /* The iterator of array，类似ListNode */
typedef struct Array
{
  int size;     /* 数组中现有的元素个数 */
  int capcity;  /* 数组预先分配的空间大小 */
  int *data;    /* 数组的数据区，大小是capcity * sizeof(int)字节 */
} Array, *PArray;

/* 数组的构造函数 */
PArray Array_create(int capcity)
{
  PArray ret = (PArray)malloc(sizeof(Array));
  ret->size = 0;
  ret->capcity = capcity;
  ret->data = (int*)malloc(sizeof(int) * capcity);
  return ret;
}

/* 数组的追加元素 */
void Array_append(PArray self, int val)
{
  if(self->size == self->capcity)
  {
    return ;
  }
  else
  {
    self->data[self->size] = val;
    self->size ++;
  }
}

/* 数组的析构函数 */
void Array_free(PArray self)
{
  free(self->data);
  free(self);
}

/* 数组的头指针，依然返回void* */
void* Array_begin(PArray self)
{
  return self->data;
}

/* 数组的尾指针，同样不包括在数组内 */
void* Array_end(PArray self)
{
  return self->data + self->size;
}

/* 
 * 获得数组指针指向的内容
 * 注意它在事实上是ArrayIt的成员方法（尽管ArrayIt并不是结构体）
 */
int ArrayIt_getVal(void* self)
{
  return *((int*)self);
}

/* 
 * 获得下一个元素的数组指针（后继）
 * 操作本身非常简单，但是很快我们就将看到这样的封装是值得的
 */
void* ArrayIt_getNext(void* self)
{
  return ((int*)self) + 1;
}

/* Array ends */

/* Sequence begins */

/* 
 * 下面进入Sequence命名空间，这里并没有类，函数也都不是成员函数
 * 我们在这里定义普适性的函数，它们同时作用与数组和链表
 * 很容易想到这样的函数其实有很多，代码量往往也是几个空间中最大的
 * 它们每个函数都接受一些函数指针，作为处理数据的方式
 */

/* 打印一个序列，这只是一个最简单的示例 */
void Seq_print(
    void* begin,              /* 起始指针 */
    void* end,                /* 结尾指针 */
    int (*getVal)(void*),     /* 从指针获得值的函数 */
    void* (*getNext)(void*)   /* 获得下一个指针的函数 */
    )
{
  void* it;
  for(it = begin; it != end; it = getNext(it))
    printf("%d\n", getVal(it));
}

/* Sequence ends */

int main()
{
  /* 声明链表和数组 */
  PList list;
  PArray array;

  /* 在其中插入元素 */
  list = List_create();
  List_append(list, 1);
  List_append(list, 2);
  List_append(list, 3);
  List_append(list, 4);
  List_append(list, 5);
  List_append(list, 6);

  array = Array_create(10);
  Array_append(array, 2);
  Array_append(array, 3);
  Array_append(array, 5);
  Array_append(array, 7);
  Array_append(array, 11);
  Array_append(array, 13);

  /* 打印它们 */
  Seq_print(
      List_begin(list),
      List_end(list),
      ListNode_getVal,
      ListNode_getNext);

  Seq_print(
      Array_begin(array),
      Array_end(array),
      ArrayIt_getVal,
      ArrayIt_getNext);

  /* 释放内存 */
  List_free(list);
  Array_free(array);

  return 0;
}

/*

分析和总结：
1.面向对象的根本思想是为了避免代码重复，为此我们可以将不变的算法和变化的数据相分离
2.每个类的实例自己知道如何管理好自身，外部的所有调用均通过成员方法来进行，
  而不访问成员变量，这样更加安全
3.成员方法和其他函数的根本区别其实在于有没有self指针，而不是根据命名空间，或是它们
  是否处理某个类的数据。于是我们发现构造函数其实不是成员方法。
4.我们永远不会把ArrayIt_getVal和ListNode_getNext放在一起使用，但是我们不得不传入
  两个参数。这启发我们：代码的抽象程度依然不够高，我们还可以做得更好

*/
