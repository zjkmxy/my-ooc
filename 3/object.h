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
