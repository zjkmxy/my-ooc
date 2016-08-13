/*7:2*/

#include "amb.h"
#include <memory.h>
#include <stdlib.h>

struct Amb
{
  jmp_buf fail;
};

PAmb Amb_create(jmp_buf *fail)
{
  PAmb ret = (PAmb)malloc(sizeof(Amb));
  memcpy(&ret->fail, fail, sizeof(jmp_buf));
  return ret;
}

bool Amb_choose(PAmb self, PAmbCont cont, int cnt, ...)
{
  va_list ap;
  bool ret;

  va_start(ap, cnt);
  ret = Amb_choosev(self, cont, cnt, ap);
  va_end(ap);
  return ret;
}

bool Amb_choosev(PAmb self, PAmbCont cont, int cnt, va_list ap)
{
  int ret;
  jmp_buf pre_fail;
  memcpy(&pre_fail, &self->fail, sizeof(jmp_buf));
  for(; cnt > 0; cnt --)
  {
    ret = va_arg(ap, int);
    if(setjmp(self->fail) == JMP_OK)
    {
      cont->run(cont, self, ret);
      return true;
    }
  }
  longjmp(pre_fail, JMP_FAIL);
  assert(false);
  return false;
}

void Amb_fail(PAmb self)
{
  longjmp(self->fail, JMP_FAIL);
}

void Amb_free(PAmb self)
{
  free(self);
}
