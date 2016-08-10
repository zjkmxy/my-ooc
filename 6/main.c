#include <stdio.h>
#include "object.h"

struct Int
{
  Object base;
  int val;
};
typedef struct Int Int;
typedef struct Int *PInt;

const ClassDesc Int_class =
{
  sizeof(Int),
  Obj_destroy,
  Obj_loader,
  &Object_class,
  NULL,
  0
};

PInt Int_create(PInt ret, int val)
{
  ret->val = val;
  return ret;
}

PInt Int_add(PInt self, PInt rhs)
{
  PInt ret = NULL;
  MH_enter(3, &self, &rhs, &ret);
  ret = Int_create(new(Int), self->val + rhs->val);
  MH_leave();
  return ret;
}

void Int_print(PInt self)
{
  MH_enter(1, &self);
  printf("%d\n", self->val);
  MH_leave();
}

PInt fibonacci(int x)
{
  PInt a = NULL, b = NULL;
  MH_enter(2, &a, &b);

  if(x <= 2)
  {
    MH_leave();
    return Int_create(new(Int), 1);
  }

  /* 这里必须明确地表示出引用，否则会被销毁 */
  a = fibonacci(x - 2);
  b = fibonacci(x - 1);
  a = Int_add(a, b);
  MH_leave();
  return a;
}

int main()
{
  MH_init(2048, 2048);
  PInt val = NULL;
  MH_enter(1, val);

  val = fibonacci(12);
  Int_print(val);

  MH_leave();
  MH_final();
  return 0;
}