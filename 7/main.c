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
  /* 
   * 将5个人的排名封入闭包
   * 当然我们可以选择数组，不如说那样反而更好。
   */
  int betty;
  int ethel;
  int joan;
  int kitty;
  int marry;
};

/* 比较两个整数 */
int comp(const void* a, const void* b)
{
  return *(int*)a - *(int*)b;
}

/* 判断5个数是否各不相同 */
bool distinctFive(int a, int b, int c, int d, int e)
{
  int arr[5] = {a, b, c, d, e};
  qsort(&arr, 5, sizeof(int), comp);
  return (arr[0] != arr[1]) && (arr[1] != arr[2]) && (arr[2] != arr[3]) && (arr[3] != arr[4]);
}

/* 
 * 每个choose完成后的延续
 * 如果用数组的话1个函数就够了
 * 这里出于教学目的将5个全部明确写出
 */
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

  /* 
   * 构造afterEthel，作为[ethel = ]choose(...)的延续
   * 这里afterEthel的上下文是：所有函数 + self_amb +
   * sender + betty(=ret) + self
   * next，也就是afterEthel本身因为尚未构造完成，所以在
   * 上下文中是未初始化的变量，不能访问。
   * 尽管构造点之后next有值，但是在afterEthel闭包构造完成
   * 后的任何值都不在“上下文”之中。闭包只能捕获构造一瞬间
   * 变量的值。
   * 当然next的地址&next是不会变化的，而且一直处于栈空间中。
   * 所以&next在afterEthel的上下文之中。当然我们的程序中并
   * 没有这样使用next，于是上面的列表里就把类似的东西都省略了。
   */
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

  /* 
   * 构造afterJoan，作为[joan = ]choose(...)的延续
   * 这里afterJoan的上下文是：所有函数 + self_amb +
   * sender + betty + ethel(=ret) + self
   * betty存在于上下文中是因为afterEthel捕获了它，
   * 以至于我们可以在这个点访问到它。
   * 闭包捕获上下文并不会捕捉所有的值，事实上只有betty
   * 和ethel被afterJoan捕获，其他的值因为并没有在
   * afterJoan中作为自由变元出现，所以也没有捕获的必要。
   */
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

  /*
   * 这里我们并没有采用“纯”CPS的解决方案。
   * 试想如果下一句是return Abs_choose，然后afterJoan本身是bool返回值的，
   * 我们其实是可以完全放弃setjmp，而直接用return进行跳脱的。
   * （具体操作留作思考题）
   * CPS的作用除了多线程外，跨函数跳脱也是一个重要的内容。
   * 特别的，如果所有CPS函数的返回值都不是bool，而是另一个延续的闭包
   * 那么我们可以在之后调用这个延续来重新进入函数内。
  * 也就是说相比于异常机制，CPS不仅可以“跳脱”，还可以“重入”。
   * 我使用闭包来实现行为树的代码中正是使用了这个技术。
   */
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

  /*
   * 这里我们获得了所有5个人的排名，开始进行判定。
   * 失败就调用fail回滚。
   * 因此，就像输出所显示的那样，每个闭包和choose、fail
   * 都被执行了多次。
   */

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

/*
 * 总结：
 * 1.CPS的用途有很多，这里只谈了冰山一角。可以看看C#的yeild系列，然后和我们的比较一下。
 * 2.思考：如何不用setjmp，并把5人的排名放在数组里，实现相同的功能？
 * 3.联系x86架构汇编语言的栈上函数调用方式，思考setjmp为何不能在不同函数的代码中自由跳转，
 * 比如说主函数调用f，f中setjmp一下然后返回，主函数继续调用g，g中longjmp到f设置的点。
 * 如果要改写的话有没有什么办法呢？
 * 事实上实现了自由跳转之后，我们可以写出“理想的主函数”了。
 * 但是这种做法并不好。可能的情况下，应当避免这么做。
 * 这是一种只针对特定CPU实现的、过度底层化的CPS变换的实现。
 */
