/*5:1*/

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
 * 获得数组长度
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

typedef size_t FieldDesc;

struct ManagedHeap;
typedef struct ManagedHeap ManagedHeap;
typedef struct ManagedHeap *PManagedHeap;

struct RefLink;
typedef struct RefLink RefLink;
typedef struct RefLink *PRefLink;

struct Object
{
  /* 类型元信息 */
  Class meta;

  /* 访问标志 */
  PRefLink refs;
};

struct ClassDesc
{

  /* 该类型实例的尺寸 */
  size_t size;

  /* 析构函数 */
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
  FieldDesc *fields;

  /* 对象指针字段的个数 */
  size_t field_cnt;
};

/* 默认加载器（什么都不做） */
void Obj_loader();

/* 默认析构函数（什么都不做） */
void Obj_destroy(PObject self);

/* 托管堆垃圾回收算法 */
void MH_collect();

/* 托管堆内存分配 */
PObject MH_alloc(Class klass);

/* 初始化托管堆 */
void MH_init(size_t buflen, size_t var_count);

/* 进入新的函数 */
void MH_enter(int var_cnt, ...);

/* 离开函数 */
void MH_leave();

/* 载入类 */
void MH_loadClass(int class_cnt, ...);

/* 销毁托管堆 */
void MH_final();

/* Object类的描述符 */
extern const ClassDesc Object_class;

PObject Obj_new(Class klass);

#define new(classname)  (classname*)Obj_new(&classname##_class)

#endif
