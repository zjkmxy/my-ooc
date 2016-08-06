#include "basics.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const ClassDesc Plachta_class = {
  {&Class_class},
  sizeof(Plachta),
  Obj_destroy,
  &Object_class
};

const ClassDesc Integer_class = {
  {&Class_class},
  sizeof(Integer),
  Obj_destroy,
  &Plachta_class
};

const ClassDesc Float_class = {
  {&Class_class},
  sizeof(Float),
  Obj_destroy,
  &Plachta_class
};

static PIAdditive Int_plus(PIAdditive self, PIAdditive rhs);
static PIAdditive Int_addedWithInt(PIAdditive self, PInteger lhs);
static PIAdditive Int_addedWithFloat(PIAdditive self, PFloat lhs);
static void Int_print(PIAdditive self, char* buf, size_t buf_len);

static PIAdditive Float_plus(PIAdditive self, PIAdditive rhs);
static PIAdditive Float_addedWithInt(PIAdditive self, PInteger lhs);
static PIAdditive Float_addedWithFloat(PIAdditive self, PFloat lhs);
static void Float_print(PIAdditive self, char* buf, size_t buf_len);

PInteger Int_create(PInteger ret, int val)
{
  ret->val = val;

  ret->base.plus = Int_plus;
  ret->base.addedWithInt = Int_addedWithInt;
  ret->base.addedWithFloat = Int_addedWithFloat;
  ret->base.print = Int_print;

  return ret;
}

PFloat Float_create(PFloat ret, float val)
{
  ret->val = val;

  ret->base.plus = Float_plus;
  ret->base.addedWithInt = Float_addedWithInt;
  ret->base.addedWithFloat = Float_addedWithFloat;
  ret->base.print = Float_print;

  return ret;
}

PIAdditive Int_plus(PIAdditive self, PIAdditive rhs)
{
  return rhs->addedWithInt(rhs, (PInteger)self);
}

PIAdditive Float_plus(PIAdditive self, PIAdditive rhs)
{
  return rhs->addedWithFloat(rhs, (PFloat)self);
}

PIAdditive Int_addedWithInt(PIAdditive self, PInteger lhs)
{
  PInteger ret = Int_create(new(Integer), lhs->val + ((PInteger)self)->val);
  return (PPlachta)ret;
}

PIAdditive Int_addedWithFloat(PIAdditive self, PFloat lhs)
{
  /* 利用对称性 */
  return Float_addedWithInt((PPlachta)lhs, (PInteger)self);
}

PIAdditive Float_addedWithInt(PIAdditive self, PInteger lhs)
{
  PFloat ret = Float_create(new(Float), (float)lhs->val + ((PFloat)self)->val);
  return (PPlachta)ret;
}

PIAdditive Float_addedWithFloat(PIAdditive self, PFloat lhs)
{
  PFloat ret = Float_create(new(Float), lhs->val + ((PFloat)self)->val);
  return (PPlachta)ret;
}

void Int_print(PIAdditive self, char* buf, size_t buf_len)
{
  int val = ((PInteger)self)->val;
  char* tmp = (char*)malloc(1024);
  sprintf(tmp, "%d", val);
  for(size_t i = 0; tmp[i] && i < buf_len - 1; i ++)
  {
    buf[i] = tmp[i];
  }
  buf[i] = 0;
  free(tmp);
}

void Float_print(PIAdditive self, char* buf, size_t buf_len)
{
  float val = ((PFloat)self)->val;
  char* tmp = (char*)malloc(1024);
  sprintf(tmp, "%f", val);
  for(size_t i = 0; tmp[i] && i < buf_len - 1; i ++)
  {
    buf[i] = tmp[i];
  }
  /* 这里忽略舍入误差 */
  buf[i] = 0;
  free(tmp);
}
