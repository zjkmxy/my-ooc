/*2:1*/

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

typedef int   BOOL;
#define TRUE  1
#define FALSE 0

/* 前置定义可以让我们更加好过 */
struct Iterator;
typedef struct Iterator Iterator;
typedef struct Iterator *PIterator;
struct Sequence;
typedef struct Sequence Sequence;
typedef struct Sequence *PSequence;

struct Iterator
{
  BOOL (*isEnd)(PIterator self);
  int (*getVal)(PIterator self);
  void (*getNext)(PIterator self);
  void (*free)(PIterator self);
};

struct Sequence
{
  int size;
  PIterator (*begin)(PSequence self);
  BOOL (*append)(PSequence self, int val);
  void (*free)(PSequence self);
};

int Seq_getSize(PSequence self);
void Seq_print(PSequence self, int maxLength);

#endif
