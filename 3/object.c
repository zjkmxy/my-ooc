#include "object.h"
#include <string.h>
#include <stdlib.h>

static void Obj_destroy(PObject self)
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
  PCClassDesc desc;
  if(!self && !self->class_desc)
    return false;
  if(class_desc == &object_class)
    return true;

  for(desc = self->class_desc; desc; desc = desc->super_class)
  {
    if(desc == class_desc)
      return true;
  }

  return false;
}

PObject Obj_new(PCClassDesc class_desc)
{
  assert(class_desc);
  return (PObject)malloc(class_desc->size);
}

void Obj_free(PObject self)
{
  self->class_desc->destroy(self);
  free(self);
}
