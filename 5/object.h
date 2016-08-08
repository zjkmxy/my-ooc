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

/* 对象类：每个对象都有类描述符 */
struct Object
{
  PCClassDesc class_desc;

  /* 引用计数 */
  size_t ref;
};

/* 类描述符类，当然它是Object的子类 */
struct ClassDesc
{
  /* 基类 */
  Object base;

  /* 尺寸 */
  size_t size;

  /* 析构函数 */
  void (*destroy)(PObject self);

  /* 基类的描述符 */
  PCClassDesc super_class;
};

/* Object类的描述符 */
extern const ClassDesc Object_class;

/* Class类的描述符 */
extern const ClassDesc Class_class;

/* 取得self的类型信息 */
PCClassDesc Obj_classOf(PCObject self);

/* 判断self是否是class_desc类的对象，考虑继承 */
bool Obj_instOf(PCObject self, PCClassDesc class_desc);

/* 
 * 为类class的对象分配内存
 * 新分配的对象自带1的引用计数。
 */
PObject Obj_new(PCClassDesc class_desc);

/*
 * 释放self，减少1个计数
 * 如果减少到0了就调用析构并释放内存。
 */
void Obj_release(PObject self);

/*
 * Object类的默认析构函数（什么都不做）
 */
void Obj_destroy(PObject self);

/*
 * 取得某个类的基类
 */
PCClassDesc Class_getBase(PCClassDesc self);

/*
 * 判断self是否是base的子类
 */
bool Class_kindOf(PCClassDesc self, PCClassDesc base);

/*
 * 快速分配内存的语法糖
 * 这样我们就可以写：PCLASS VAR = CLASS_create(new(CLASS), ...);
 */
#define new(classname)  (classname*)Obj_new(&classname##_class)

/*
 * 删除某个对象的语法糖
 */
#define release(ptr) Obj_release((PObject)ptr)

/*
 * 增加一个引用计数
 */
PObject Obj_addRef(PObject self);

/*
 * 取得引用计数值
 */
size_t Obj_getRef(PObject self);

#define addref(obj)  (Obj_addRef((PObject)(obj)))
#define getref(obj)  (Obj_getRef((PObject)(obj)))

#endif
