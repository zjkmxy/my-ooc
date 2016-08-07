/*3:2*/

#include "object.h"
#include <string.h>
#include <stdlib.h>

void Obj_destroy(PObject self)
{
  UNUSED(self);
}

PCClassDesc Obj_classOf(PCObject self)
{
  /* 
   * assert断言是C语言里最常用的一种异常处理机制。
   * 相当于if(!expr){abort();}，不过仅仅在Debug模式下会被启用。
   * assert失败后程序会输出文件和行号，便于调试。
   * 因为C语言主张“每个程序员都应该知道自己在干什么”，
   * 所以对于强制满足的条件在Release版本中没有检查也没关系
   */
  assert(self);
  return self->class_desc;
}

bool Obj_instOf(PCObject self, PCClassDesc class_desc)
{
  if(!self && !self->class_desc)
    return false;
  if(class_desc == &Object_class)
    return true;
  return Class_kindOf(self->class_desc, class_desc);
}

PObject Obj_new(PCClassDesc class_desc)
{
  assert(class_desc);
  PObject ret = (PObject)malloc(class_desc->size);
  ret->class_desc = class_desc;
  return ret;
}

void Obj_free(PObject self)
{
  assert(self);
  assert(self->class_desc);
  assert(self->class_desc->destroy);
  self->class_desc->destroy(self);
  free(self);
}

const ClassDesc Object_class = {
  /*
   * 这个是Object继承来的结构，表示“对象本身的类型”
   * 所有类描述符的类型都是Class，所以一定是{&Class_class}
   */
  {&Class_class},
  sizeof(Object),
  Obj_destroy,
  NULL
};

const ClassDesc Class_class = {
  {&Class_class},
  sizeof(ClassDesc),
  Obj_destroy,
  &Object_class
};

PCClassDesc Class_getBase(PCClassDesc self)
{
  return self->super_class;
}

bool Class_kindOf(PCClassDesc self, PCClassDesc base)
{
  PCClassDesc desc;
  for(desc = self; desc; desc = desc->super_class)
  {
    if(desc == base)
      return true;
  }
  return false;
}

