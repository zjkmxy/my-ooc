#include <stdio.h>
#include "object.h"


/* 
 * 这里用一个整数类来计算斐波那契数列
 * 测试我们的托管堆能否正常工作
 */

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

  /* 这里必须明确地表示出引用，否则计算b时a会被销毁 */
  a = fibonacci(x - 2);
  b = fibonacci(x - 1);
  a = Int_add(a, b);
  MH_leave();
  return a;
}

void fibTest()
{
  PInt val = NULL;
  MH_enter(1, &val);

  val = fibonacci(30); /* fib(30)的话，托管堆会被迫回收很多次 */
  Int_print(val);

  MH_leave();
}

/************************************************************************/

/* 我们用链表来测试对象引用的递归扫描 */

struct Link;
typedef struct Link Link;
typedef struct Link *PLink;

struct Link
{
  Object base;
  int val;
  PLink next;
};
PLink Link_global = NULL;

void Link_loader();

/* 有一个指针成员，给出其偏移 */
const FieldDesc Link_fields_desc[] = {OFFSETOF(Link, next)};
const ClassDesc Link_class =
{
  sizeof(Link),
  Obj_destroy,
  Link_loader,
  &Object_class,
  Link_fields_desc,
  ARRAY_SIZE(Link_fields_desc)
};

void Link_loader()
{
  /* 载入一个静态变量，它是根对象 */
  MH_addStackVar((PObject*)&Link_global);
}

PLink Link_create(PLink ret, int val, PLink next)
{
  ret->val = val;
  ret->next = next;
  return ret;
}

PLink Link_pushHead(PLink self, int val)
{
  return Link_create(new(Link), val, self);
}

void linkTest1()
{
  MH_enter(0);

  Link_global = Link_pushHead(Link_global, 1);
  Link_global = Link_pushHead(Link_global, 2);
  Link_global = Link_pushHead(Link_global, 3);
  Link_global = Link_pushHead(Link_global, 4);
  Link_global = Link_pushHead(Link_global, 5);
  Link_global->next->next = NULL;

  MH_leave();
}

void linkTest2()
{
  PLink cur = NULL;
  MH_enter(1, &cur);

  /* 打印5和4，说明间接的引用没有被误释放 */
  for(cur = Link_global; cur; cur = cur->next)
  {
    printf("%d\n", cur->val);
  }

  MH_leave();
}

/************************************************************************/

int main()
{
  MH_init(2048, 2048);
  MH_loadClass(2, &Int_class, &Link_class);

  MH_enter(0);
  linkTest1();
  fibTest();
  linkTest2();
  MH_leave();
  
  MH_final();
  return 0;
}

/*
 * 1.一个麻烦的地方在于，MH本身并不知道真实的栈的样子，
 * 因此必须明确地给出变量保存函数中间结果， 不然可能会被误释放掉。
 * 为此C++/CLI的所有对象都是二级指针，变量指向一个对象句柄，对象句柄
 * 再指向托管堆上真实的对象。（推测）这样如果我们监控了所有句柄的创建和释放，
 * 就可以不需监控局部变量。但是依然很蛋疼。
 *
 * 2.我们的托管堆无法支持对象数组。这和我们不健全的元信息机制有关。
 *
 * 3.可以查阅相关资料了解一下JVM的垃圾回收和.NET CLR的托管堆。
 */
