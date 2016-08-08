/*5:2*/

#include "object.h"
#include <string.h>
#include <stdlib.h>

void Obj_destroy(PObject self)
{
  UNUSED(self);
}

PCClassDesc Obj_classOf(PCObject self)
{
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
  ret->ref = 1;
  return ret;
}

void Obj_release(PObject self)
{
  assert(self);
  assert(self->class_desc);
  assert(self->class_desc->destroy);
  self->ref --;
  if(self->ref <= 0)
  {
    self->class_desc->destroy(self);
    free(self);
  }
}

const ClassDesc Object_class = {
  {&Class_class, 1},
  sizeof(Object),
  Obj_destroy,
  NULL
};

const ClassDesc Class_class = {
  {&Class_class, 1},
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

PObject Obj_addRef(PObject self)
{
  self->ref ++;
  return self;
}

size_t Obj_getRef(PObject self)
{
  return self->ref;
}
