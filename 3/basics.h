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
  PIAdditive (*plus)(PIAdditive self, PIAdditive rhs);
  PIAdditive (*addedWithInt)(PIAdditive self, PInteger lhs);
  PIAdditive (*addedWithFloat)(PIAdditive self, PFloat lhs);
  void (*print)(PIAdditive self, char* buf, size_t buf_len);
};

struct Integer{
  Plachta base;
  int val;
};

struct Float{
  Plachta base;
  float val;
};

extern const ClassDesc Plachta_class;
extern const ClassDesc Integer_class;
extern const ClassDesc Float_class;

PInteger Int_create(PInteger ret, int val);
PFloat Float_create(PFloat ret, float val);

#endif

