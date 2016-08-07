#ifndef _BASICS_H
#define _BASICS_H

#include "object.h"

struct IAdditive;
typedef struct IAdditive IAdditive;
typedef struct IAdditive *PIAdditive;

struct Integer;
typedef struct Integer Integer;
typedef struct Integer *PInteger;
struct Float;
typedef struct Float Float;
typedef struct Float *PFloat;

struct IAdditive
{
  /* private */
  Object base;
  PIAdditive (*addedWithInt)(PIAdditive self, PInteger lhs);
  PIAdditive (*addedWithFloat)(PIAdditive self, PFloat lhs);

  /* public */
  /* 将self与rhs相加，返回新建的结果对象 */
  PIAdditive (*plus)(PIAdditive self, PIAdditive rhs);

  /* 将self打印到buf中，buf最长不超过buf_len */
  void (*print)(PIAdditive self, char* buf, size_t buf_len);
};

struct Integer{
  IAdditive base;
  int val;
};

struct Float{
  IAdditive base;
  float val;
};

extern const ClassDesc IAdditive_class;
extern const ClassDesc Integer_class;
extern const ClassDesc Float_class;

PInteger Int_create(PInteger ret, int val);
PFloat Float_create(PFloat ret, float val);

#endif

