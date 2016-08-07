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
  /* private */
  Object base;
  PIAdditive (*addedWithInt)(PIAdditive self, PInteger lhs);
  PIAdditive (*addedWithFloat)(PIAdditive self, PFloat lhs);

  /* public */
  /* 将self与rhs相加，返回新建的结果对象 */
  PIAdditive (*plus)(PIAdditive self, PIAdditive rhs);

  /* 将self打印到buf中，buf最长不超过buf_len */
  void (*print)(PIAdditive self, char* buf, size_t buf_len);
};

/*
 * 另外，IAdditive类选择将虚拟函数放在结构体里，而不是像
 * 析构函数一样放在类描述符里。这样调用比较方便，但是也带
 * 来了额外的开销。成熟的对象系统，如C++和GObject(C语言)
 * 会将所有的虚方法都存在额外的虚方法表/类描述符里。
 */

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

