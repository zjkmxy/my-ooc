/*7:2*/

#include "amb.h"
#include <memory.h>
#include <stdlib.h>

struct Amb
{
  jmp_buf fail; /* 只有一个失败指针而已 */
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

  /* 简单地开启变参表并调用choosev */
  va_start(ap, cnt);
  ret = Amb_choosev(self, cont, cnt, ap);
  va_end(ap);
  return ret;
}

bool Amb_choosev(PAmb self, PAmbCont cont, int cnt, va_list ap)
{
  int ret;
  jmp_buf pre_fail;
  /* 保存之前的失败指针，因为之后我们要修改它 */
  memcpy(&pre_fail, &self->fail, sizeof(jmp_buf));
  for(; cnt > 0; cnt --)
  {
    ret = va_arg(ap, int);
    /* 设置失败指针：当有失败时就继续循环，否则返回true */
    if(setjmp(self->fail) == JMP_OK)
    {
      cont->run(cont, self, ret);
      return true;
    }
  }
  /* 通过失败指针向前跳出 */
  longjmp(pre_fail, JMP_FAIL);
  /* 这里永远不会执行到 */
  assert(false);
  return false;
}

void Amb_fail(PAmb self)
{
  /* 通过当前的失败指针向前跳转 */
  longjmp(self->fail, JMP_FAIL);
}

void Amb_free(PAmb self)
{
  free(self);
}
