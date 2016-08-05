#ifndef _BASICS_H
#define _BASICS_H

#include "object.h"

struct IAdditive;
typedef struct IAdditive IAdditive;
typedef struct IAdditive *PIAdditive;
typedef struct IAdditive Fixnum;
typedef struct IAdditive *PFixnum;
typedef struct IAdditive Plachta;
typedef struct IAdditive *PPlachta;

struct Integer;
typedef struct Integer Integer;
typedef struct Integer *PInteger;
struct Float;
typedef struct Float Float;
typedef struct Float *PFloat;

struct IAdditive
{
  Object base;
  PPlachta (*plus)(PPlachta self, PPlachta rhs);
  PPlachta (*addedWithInt)(PPlachta self, PInteger lhs);
  PPlachta (*addedWithFloat)(PPlachta self, PFloat lhs);
  void (*print)(PPlachta self, char* buf, size_t buf_len);
};

struct Integer{
  Plachta base;
  int val;
};

struct Float{
  Plachta base;
  float val;
};

PInteger Int_create(PInteger ret, int val);
PFloat Float_create(PFloat ret, int val);

#endif

