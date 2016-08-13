/*7:3*/

#include "amb.h"
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

struct MyCont;
typedef struct MyCont MyCont;
typedef struct MyCont *PMyCont;

struct MyCont
{
  AmbCont base;
  int betty;
  int ethel;
  int joan;
  int kitty;
  int marry;
};

int comp(const void* a, const void* b)
{
  return *(int*)a - *(int*)b;
}

bool distinctFive(int a, int b, int c, int d, int e)
{
  int arr[5] = {a, b, c, d, e};
  qsort(&arr, 5, sizeof(int), comp);
  return (arr[0] != arr[1]) && (arr[1] != arr[2]) && (arr[2] != arr[3]) && (arr[3] != arr[4]);
}

void afterBetty(PAmbCont self_amb, PAmb sender, int ret);
void afterEthel(PAmbCont self_amb, PAmb sender, int ret);
void afterJoan(PAmbCont self_amb, PAmb sender, int ret);
void afterKitty(PAmbCont self_amb, PAmb sender, int ret);
void afterMarry(PAmbCont self_amb, PAmb sender, int ret);

void afterBetty(PAmbCont self_amb, PAmb sender, int ret)
{
  PMyCont self = (PMyCont)self_amb;
  MyCont next;

  printf("Betty=%d\n", ret);

  memcpy(&next, self, sizeof(MyCont));
  next.base.run = afterEthel;
  next.betty = ret;

  Amb_choose(sender, (PAmbCont)&next, 5, 1, 2, 3, 4, 5);
}

void afterEthel(PAmbCont self_amb, PAmb sender, int ret)
{
  PMyCont self = (PMyCont)self_amb;
  MyCont next;

  printf("Betty=%d Ethel=%d\n",
    self->betty, ret);

  memcpy(&next, self, sizeof(MyCont));
  next.base.run = afterJoan;
  next.ethel = ret;

  Amb_choose(sender, (PAmbCont)&next, 5, 1, 2, 3, 4, 5);
}

void afterJoan(PAmbCont self_amb, PAmb sender, int ret)
{
  PMyCont self = (PMyCont)self_amb;
  MyCont next;

  printf("Betty=%d Ethel=%d Joan=%d\n",
    self->betty, self->ethel, ret);

  memcpy(&next, self, sizeof(MyCont));
  next.base.run = afterKitty;
  next.joan = ret;

  Amb_choose(sender, (PAmbCont)&next, 5, 1, 2, 3, 4, 5);
}

void afterKitty(PAmbCont self_amb, PAmb sender, int ret)
{
  PMyCont self = (PMyCont)self_amb;
  MyCont next;

  printf("Betty=%d Ethel=%d Joan=%d Kitty=%d\n",
    self->betty, self->ethel, self->joan, ret);

  memcpy(&next, self, sizeof(MyCont));
  next.base.run = afterMarry;
  next.kitty = ret;

  Amb_choose(sender, (PAmbCont)&next, 5, 1, 2, 3, 4, 5);
}

void afterMarry(PAmbCont self_amb, PAmb sender, int ret)
{
  PMyCont self = (PMyCont)self_amb;
  self->marry = ret;

  printf("Betty=%d Ethel=%d Joan=%d Kitty=%d Marry=%d\n",
    self->betty, self->ethel, self->joan, self->kitty, self->marry);

  if(!distinctFive(self->betty, self->ethel, self->joan, self->kitty, self->marry))
    Amb_fail(sender);
  if((self->kitty == 2) == (self->betty == 3))
    Amb_fail(sender);
  if((self->ethel == 1) == (self->joan == 2))
    Amb_fail(sender);
  if((self->joan == 3) == (self->ethel == 5))
    Amb_fail(sender);
  if((self->kitty == 2) == (self->marry == 4))
    Amb_fail(sender);
  if((self->marry == 4) == (self->betty == 1))
    Amb_fail(sender);

  return;
}

int main()
{
  MyCont next;
  PAmb amb;
  jmp_buf buf;

  memset(&next, 0, sizeof(MyCont));
  next.base.run = afterBetty;

  if(setjmp(buf) == JMP_OK)
  {
    amb = Amb_create(&buf);
    Amb_choose(amb, (PAmbCont)&next, 5, 1, 2, 3, 4, 5);
    printf("Succeed\n");
  }
  else
  {
    printf("Failed\n");
  }

  return 0;
}