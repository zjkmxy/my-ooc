/*3:1*/

/*
 * 我们使用容器（如链表），或者其他一些抽象数据结构时往往希望它能运行在不同的数据类型上。
 * C语言中我们往往使用void*来代表其他任意类型的数据，然而无类型指针自由度太高了，这
 * 导致我们无法进行任何形式的安全检查：如我们希望一个int的List，但是因为失误将float
 * 误传进去，那么我们之后的所有操作都将得到错误的结果，而且很难发现。
 * C++和Java中采用“泛型/模板”作为解决方案。它们允许一个类在定义时接受特定的类T作为
 * "类型参数"，如List<int>，并且在*编译*期间检查所有的函数调用，如果类型参数不符，
 * 会阻止其通过编译。
 * 
 * C++的泛型：它在编译时不对泛型类/函数生成字节码，而是对每个特定类型的调用
 * 生成字节码。比如List<T>被作为List<int>、List<float>使用，就将生成对应于int和float
 * 的两套不同的字节码。如果我们在C语言中模仿这样的行为，就相当于人为复制代码很多份，
 * 这显然不是良好的做法。
 * 当然C++可以通过dynamic_cast技巧来把类型检查推迟到动态：令Int和Float继承自Object，
 * 然后将他们作为Object插入到List<Object>中。Object本身有点像void*，不过当我们在运行
 * 时试图用dynamic_cast<Int>(obj)将Float映射成的Object转化到Int时，会得到空指针。
 * 但是这仅限于我们*能*确定类型的时候。当我们在编写List的代码时，我们的List要适用于
 * 所有的Object，因而我们无法在List.push方法中使用dynamic_cast检测传入的参数类型。
 *
 * Java的泛型：Java的泛型仅限于编译时，运行时刻Java根本不知道你是否将一个int类型的
 * 对象传递给了List<float>做参数。然而C语言编译器无法
 */

#ifndef _OBJECT_H
#define _OBJECT_H

#include <stddef.h>
#include <stdbool.h>
#include <assert.h>

/* 未使用的参数 */
#define UNUSED(param) (void)(param)

/*
 * 取得成员偏移，即(&var->member - var)按字节数计量
 */
#define OFFSETOF(type, member) ((size_t) &((type*)0)->member)

/*
 * 通过成员地址取得所在结构体基地址
 * 相当于已知ptr = &var->member，求var
 */
#define CONTAINER_OF(ptr, type, member) \
  (type*)((char*)(ptr) - OFFSETOF(type, member))

struct ClassDesc;
typedef struct ClassDesc ClassDesc;
typedef struct ClassDesc *PClassDesc;
typedef const struct ClassDesc *PCClassDesc;
struct Object;
typedef struct Object Object;
typedef struct Object *PObject;
typedef const struct Object *PCObject;

struct Object
{
  PCClassDesc class_desc;
};

struct ClassDesc
{
  Object base;
  size_t size;
  void (*destroy)(PObject self);
  PCClassDesc super_class;
};

extern const ClassDesc Object_class;
extern const ClassDesc Class_class;

PCClassDesc Obj_classOf(PCObject self);
bool Obj_instOf(PCObject self, PCClassDesc class_desc);
PObject Obj_new(PCClassDesc class_desc);
void Obj_free(PObject self);
void Obj_destroy(PObject self);

PCClassDesc Class_getBase(PCClassDesc self);
bool Class_kindOf(PCClassDesc self, PCClassDesc base);

#define new(classname)  (classname*)Obj_new(&classname##_class)
#define delete(ptr) Obj_free((PObject)ptr)

#endif
