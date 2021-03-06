/*8:1*/

/*
 * 这次我们探讨的问题是动态类型系统之一：原型系统
 * JavaScript和Lua以及Io语言使用了类似的系统。
 *
 * 1.关于元编程
 * 此前我们知道程序在动态知道自己类型的元信息就可以称为“反射”
 * 如果我们可以在运行时动态地操纵这些数据，比如给类型添加新的
 * 方法，就称为“元编程”。相对于面向对象的抽象来说，元编程提供了
 * 一个更高的抽象层次。
 * 一个支持反射的语言可以很通过标准的数据结构实现工厂模式，而一个
 * 支持元编程的语言代码本身就是自己的宏。
 * 目前“反射”和“元编程”两个概念通常一起出现，而且很多时候并
 * 不会过多计较它们的差别。
 *
 *
 * 2.关于类型系统
 * 我们理想中的编译器需要满足以下3个需求：
 * a)在有限的时间内编译完成
 * b)允许所有不会出错的程序通过编译
 * c)检查出一切类型错误
 * 然而数学家已经证明了这3个条件无法全部满足。
 * 一个例子是这样：
 * f x = {
 *   if(x is String){
 *     print x
 *   } else {
 *     print (x as String)
 *   }
 * }
 * main = {
 *   f "123+456!="
 *   f 789
 * }
 * 假设存在基本类型String和Int，Int支持as String的转化，那么上述的代码是正确的。
 * 然而对于编译器来说，它推断出f的类型应当是x Type->void，然后推断x Type时遇到了
 * 麻烦：编译器知道x Type应该包括String，但是它无法知道有哪些类型存在as String。
 * 因为模板的存在，类型的数量为无限，编译器不可能枚举所有这样的类型。
 * 于是编译器只能选择不再枚举x Type，将其按照泛型参数处理，记为'a(或α)，表示
 * “某种确定的类型”。print x一句没有错误，因为x is String的条件判断将其特化为
 * String；但是x as String一句出错，因为不是所有String以外的类型都支持as String。
 * 事实上在上述程序中f只会被用String和Int调用，但是编译器无法根据f的定义推断出这一点，
 * 于是这个程序一定会被静态类型检查的编译器阻止。如果你一定要让类似的程序通过编译，
 * 就必须人工构造一个类型(String | Int)出来。这是不必要的。
 * 而动态类型检查就会宽容得多：编译器不会试图在编译时刻检查f的类型约束是否满足，而是
 * 会在有人试图用一个不具有as String方法的对象调用f的时候，动态地给出这个错误。上面
 * 的程序没有这样的例子，因此也不会出错。
 * 更极端的例子是：
 * f x = {
 *   if(x is String)
 *     return (x + 13)
 * }
 * 只要f永远不被调用，程序就是安全的。然而静态类型检查的编译器无法知道这一点，因为f的调用
 * 不限于模块内，它可能被程序的任何一个模块调用。
 *
 * 因此这里我们实际上把编译器分成了3类：
 * 弱类型：程序可能在出错后继续执行，编译时和运行时都不会发现。例如：C++是静态弱类型，
 *    汇编是动态弱类型。（C++的数组越界可能导致不确定行为，运行时也不会被发现）
 * 静态强类型：编译器在编译期间通过人工标注和推导等方式确定了每个对象的类型。
 * 动态强类型：编译器不检查人工的类型标注，也不进行类型推导，但是在运行时刻保证发现所有
 *    可能的错误。
 * 在弱类型语言中，往往可以自由地解释二进制数据，比如把文件读到char*数组中，然后再用
 * struct S的指针去取其中的数据；在强类型语言中，不论是静态还是动态的，都不会允许类似的
 * 行为。
 * （注：值得注意的是，“weak typed”这个词没有精确的定义，大部分场合我们指的是“unsound typed”，
 * 就是上文的定义；一部分场合我们指的是“relatively weak typed”，它只是相对一般的“强类型检查”
 * 更弱而已，未必是不安全的；极少的场合我们指的是“expicit typed”，即人工标注类型）
 *
 *
 * 3.关于对象模型
 * 面向对象包括两个模型：消息模型和通用函数模型。
 * 消息模型认为，“对象”是一个消息接收器（受体），拥有若干个可以接受消息的“槽”，其他
 * 对象将“消息”（函数名+参数）发给它，它就用相应的“槽”进行处理。如：
 * Number a = new Int();
 * Number b = new Float();
 * a.+(b);
 * 最后的加法就是给a发送一个"+(b)"的消息，a会调用名为"+"的槽处理这个消息；这个槽可能是a从
 * Int或者是Number继承来的，也可能是a自己独有的。不论如何，a有且只有一个这样的槽，不论b的类型
 * 是什么。
 *
 * 通用函数模型认为，所有的重载方法都是一个“通用函数”，它在运行时刻从多个实现中选择一个
 * 最符合参数类型的重载实现并调用。如：
 * Number a = new Int();
 * Number b = new Float();
 * +(a, b);
 * 最后的加法是调用通用函数"+"的"+(Int,Float)"重载实现。
 * 注意C++的重载不是通用函数，因为类似的代码在C++重载中会调用"+(Number,Number)"，而不是动态
 * 派发为"+(Int,Float)"。
 *
 * 通用函数模型可以认为是消息模型的一个超集。处于实现简单而考虑，大部分对象系统采用消息模型，
 * 并在每个对象中设置指向槽的函数指针来实现消息模型。
 *
 *
 * 4.关于原型法
 * 原型法是一个最简单的、支持反射和元编程的对象系统，它基于消息模型实现。
 * 原型法中，“类”的概念被淡化，所有东西都只是一个带有不确定槽的Object。
 * 某个“类”以类原型的形式出现，当你需要构造一个类的实例时，就用这个原型clone一个
 * 新的对象出来；新的对象拥有一个proto槽指向clone的原型。
 * 当一个对象收到消息时，如果它自己有相关的处理槽，就自己处理；如果没有，就转发给
 * 它的proto处理。
 * clone机制有点像单细胞繁殖，不同的是即使克隆之后原型的改变依然会影响它的克隆体。
 */

#ifndef _OBJECT_H
#define _OBJECT_H

#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include "system_export.h"

/*
 * 槽
 * 这里我们采用int型的Symbol（在system_export中定义）
 * 来代表“符号”。
 * 其他编译器可能会是用字符串之类的。
 * 同时我们采用二级指针（句柄Handle）配合托管堆进行内存管理
 */
struct Slot
{
  Symbol name;
  Handle val;
};

/*
 * 严格来讲，对象中除了槽外还应该包括“额外数据”
 * 比如Int类型对象的值，还有Block/Closure对象的函数指针
 * 以及Array类型对象的每个元素，包括String也是特殊的Array
 * 这些内容作为对象系统的“黑盒内部”，无法通过系统外部访问到
 * 但是支持着整个系统的运作。
 * 这里我们固化了两个额外数据：exec和extra_data
 * 原则上来讲额外数据应该是动态的，因为不是每个对象都需要这些数据
 */
struct Object
{
  /* 对象大小 */
  size_t size;

  /* 
   * 闭包/代码块对象的函数指针
   * 额外数据
   */
  MsgFunc exec;

  /* 唯一的句柄 */
  Handle handle;

  /* 访问标志 */
  bool access;

  /* 
   * Int类型的值
   * 额外数据
   */
  int extra_data;

  /* 分配空间的槽位个数(空白槽也算) */
  size_t slot_cap;

  /*
  * 槽位数组
  * 特殊槽位如下：
  * NIL      : 空槽位
  * type     : 对象的类型
  * proto    : 该对象的原形
  * clone    : 克隆操作
  * slotCnt  : 槽的个数
  * slotNames: 数组，表示每个槽的符号(未实现)
  * getSlot  : 返回某个槽
  * setSlot  : 设置某个槽
  * ident    : 返回自己
  *
  * 槽位不够时，自动申请倍增扩展
  * 槽位小于四分之一且垃圾回收时，自动半减
  * 有些槽的设定和Io差别很大，比如proto和type
  * 在Io里，它们是方法槽；而这里它们是数据槽
  */
  Slot slots[1];
};

/* 获得obj槽的个数（有效个数，不是容量） */
size_t Obj_slotCnt(PObject obj);

/* 压缩obj槽，并返回压缩后它的个数（有效个数，不是容量） */
size_t Obj_compressSlot(PObject obj);

/* 根据槽的个数（容量）计算对象空间大小 */
size_t Obj_calcSize(size_t slotCnt);

/* 获得obj的slot槽，会搜索它的原型 */
Handle Obj_getSlot(PObject obj, Symbol slot);

/* 设置obj的slot槽为val，不会搜索它的原型，会自动扩充对象 */
void Obj_setSlot(PObject obj, Symbol slot, Handle val);

/* 克隆obj，返回句柄 */
Handle Obj_clone(PObject obj);

/* 执行代码块对象obj，作用于对象receptor，...是参数 */
Handle Obj_exec(PObject obj, Handle receptor, ...);

#endif
