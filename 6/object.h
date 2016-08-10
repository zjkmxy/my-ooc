/*6:1*/

/*
 * 被某个类的全体实例多共享的、属于特定类的成员就是静态成员。
 * 相当于类命名空间内的全局变量和全局函数。
 * 如果一个类在全局只有一个实例，它自己创建和维护这个实例，并
 * 对外值提供这一个实例，称为单例模式。单例模式一般用于多个实例
 * 会产生冲突的场合。例如：线程池，内存管理器，打印机，序列号发生器。
 * 单例和全局变量并无本质差异。
 *
 * 这里我们使用托管堆的例子来讲解单例模式。
 * 托管堆是大多数现代编程语言/虚拟机平台原生提供的服务。
 * 托管堆从操作系统预先获得一定内存，然后将所有的内存申请转化为堆上的
 * 内存申请。托管堆只提供申请不提供释放，因此只需线性向后分配内存即可
 * 分配速度很快。
 * 当堆内存耗尽时，托管堆检查堆上的所有对象，清理所有现在已经无法访问
 * 到的对象，然后整理存活的对象，然后继续利用剩余的内存。称为“垃圾回收”。
 *
 * 一般的托管堆不要求用户显示地给出引用计数控制，当有对象互相引用的
 * 时候可能发生互锁。因此现代的托管堆一般采用根指针法：记录所有全局变量
 * 和栈变量、寄存器变量，将它们作为根指针。从根指针开始进行搜索，标记所有
 * 搜索到的对象为存活，其他对象不管有没有引用计数都已经无法访问到，直接销毁。
 *
 * 托管堆在整理内存的时候也有不同的策略，最常见的做法是：
 * 将所有存活对象放在一起，并更正所有的指针到正确的引用。
 * 这种做法没有内存碎片，效率更高。当然，实际托管堆的工作机制要复杂得多。
 *
 * 托管堆内存回收时机不可控，因此所有对象默认都没有析构函数。
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

/*
 * 获得已知的数组长度
 * 注意不要用在指针上
 */
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))

struct Object;
typedef struct Object Object;
typedef struct Object *PObject;

struct ClassDesc;
typedef struct ClassDesc ClassDesc;
typedef const struct ClassDesc *PCClassDesc;
typedef PCClassDesc Class;

/* 
 * 成员描述
 * 描述类中每一个对象指针类型的成员变量的偏移
 * 供搜索存活对象时使用
 */
typedef size_t FieldDesc;

/* 托管堆 */
struct ManagedHeap;
typedef struct ManagedHeap ManagedHeap;
typedef struct ManagedHeap *PManagedHeap;

/* 
 * 引用链
 * 描述对一个对象的所有指针引用
 * 在移动对象的时候我们会修改这些引用
 * 注意引用链只在垃圾回收的时候有效
 * 回收结束后不再需要像引用计数一样动态维护它
 */
struct RefLink;
typedef struct RefLink RefLink;
typedef struct RefLink *PRefLink;

struct Object
{
  /* 
   * 类型元数据
   * 我们现在不止知道这个类型是什么
   * 而且知道几乎所有类型定义时的信息
   * 它称为“类型元数据”
   */
  Class meta;

  /* 引用链 */
  PRefLink refs;
};

struct ClassDesc
{

  /* 该类型实例的尺寸 */
  size_t size;

  /* 析构函数（没什么用） */
  void (*destroy)(PObject self);

  /* 
   * 类型加载器
   * 这里我们进行了简化处理：加载器只负责设置静态变量的引用。
   * 本身既不加在类型元信息，也不管继承关系。
   */
  void (*loader)();

  /* 基类的描述符 */
  PCClassDesc super_class;

  /* 对象指针字段的偏移(数组) */
  const FieldDesc *fields;

  /* 对象指针字段的个数 */
  size_t field_cnt;
};

/* 默认加载器（什么都不做） */
void Obj_loader();

/* 默认析构函数（什么都不做） */
void Obj_destroy(PObject self);

/* 托管堆垃圾回收算法 */
void MH_collect();

/* 托管堆内存分配，类型为klass */
PObject MH_alloc(Class klass);

/* 
 * 初始化托管堆
 * buflen    : 缓冲区大小
 * var_count : 支持对象指针变量的个数
 */
void MH_init(size_t buflen, size_t var_count);

/* 
 * 增加栈上变量
 * var : 指针变量的地址（所以是个二级指针）
 */
void MH_addStackVar(PObject *var);

/* 
 * 进入新的函数
 * var_cnt  : 该函数使用的所有对象指针局部变量
 * 后续参数 : 每个变量的地址
 */
void MH_enter(int var_cnt, ...);

/* 
 * 离开函数
 * 消除所有局部变量的引用
 */
void MH_leave();

/* 载入类 */
void MH_loadClass(int class_cnt, ...);

/* 销毁托管堆 */
void MH_final();

/* Object类的元数据 */
extern const ClassDesc Object_class;

/* 在托管堆上创建一个klass类的对象 */
PObject Obj_new(Class klass);

#define new(classname)  (classname*)Obj_new(&classname##_class)

#endif
