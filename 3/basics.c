#include "basics.h"
#include <string.h>
#include <stdlib.h>

const ClassDesc _class = {
  {&class_class},
  sizeof(Object),
  Obj_destroy,
  NULL
};
