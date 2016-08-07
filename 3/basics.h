#ifndef _BASICS_H
#define _BASICS_H

#include "object.h"

/*
 * 我们声明具有加法功能的接口IAdditive，
 * 将其扩展为整数类Integer和浮点数Float。
 */
struct IAdditive;
typedef struct IAdditive IAdditive;
typedef struct IAdditive *PIAdditive;

struct Integer;
typedef struct Integer Integer;
typedef struct Integer *PInteger;
struct Float;
typedef struct Float Float;
typedef struct Float *PFloat;

/*
 * 这里我们采用双分派模式来处理加法问题：
 * a->plus(b)调用时，a知道自己的类型，不知道b的类型；
 * 于是a根据自己是Int还是Float分别调用b->addedWithInt(a)，
 * 或者b->addedWithFloat(a)；
 * 然后b也知道自己的类型，于是完成了一次匹配。
 * 双分派有利于增加数据类别，不利于增加功能；
 *
 * 另一种做法是，在一个函数中直接用switch-case嵌套分析类别：
 * switch(a->class){
 * case Int:
 *   switch(b->class)
 *   case Int:
 *     ...
 *     break;
 * ...
 * 这样有利于增加功能，不利于增加类别。
 *
 * 双分派对于不了解这个模式的人来说不是那么直观，建议慎用。
 */
struct IAdditive
{
  Object base;
  PIAdditive (*plus)(PIAdditive self, PIAdditive rhs);
  PIAdditive (*addedWithInt)(PIAdditive self, PInteger lhs);
  PIAdditive (*addedWithFloat)(PIAdditive self, PFloat lhs);
  void (*print)(PIAdditive self, char* buf, size_t buf_len);
};

struct Integer{
  IAdditive base;
  int val;
};

struct Float{
  IAdditive base;
  float val;
};

extern const ClassDesc IAdditive_class;
extern const ClassDesc Integer_class;
extern const ClassDesc Float_class;

PInteger Int_create(PInteger ret, int val);
PFloat Float_create(PFloat ret, float val);

#endif

