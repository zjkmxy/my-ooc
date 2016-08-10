/*5:2*/

#include "object.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

void Obj_loader()
{
}

void Obj_destroy(PObject self)
{
  UNUSED(self);
}

struct ManagedHeap
{
  /* 托管堆 */
  char* buf;
  size_t buflen;
  char* top;

  /* 根对象栈 */
  PObject **stack;
  size_t stack_len;
  PObject **stack_top;
};

ManagedHeap MH_inst;

struct RefLink
{
  PObject *ref;
  PRefLink next;
};

/* 清除所有的访问标记 */
void MH_cleanAllTag()
{
  char* cur = MH_inst.buf;
  PObject obj = (PObject)cur;

  while(cur < MH_inst.top)
  {
    /* 在垃圾回收执行前，不应当有引用链存在 */
    assert(obj->refs == NULL);
    cur += obj->meta->size;
  }
}

/* 引用链上增加新的一环 */
PRefLink RL_makeLink(PObject *ref, PRefLink next)
{
  PRefLink ret = (PRefLink)malloc(sizeof(RefLink));
  ret->ref = ref;
  ret->next = next;
  return ret;
}

/* 递归标记一个对象 */
void MH_markObject(PObject *ref)
{
  PObject *field;
  size_t i;

  /* 如果已经标记，在记录引用链后直接回退 */
  if((*ref)->refs){
    (*ref)->refs = RL_makeLink(ref, (*ref)->refs);
    return ;
  }

  /* 如果是初次标记，就递归 */
  (*ref)->refs = RL_makeLink(ref, NULL);
  for(i = 0; i < (*ref)->meta->field_cnt; i ++)
  {
    field = (PObject*)(((char*)(*ref)) + (*ref)->meta->fields[i]);
    if(*field)
    {
      MH_markObject(field);
    }
  }
}

/* 递归标记根对象 */
void MH_markRoot()
{
  PObject **cur;
  for(cur = MH_inst.stack; cur < MH_inst.stack_top; cur ++)
  {
    /* 这里*cur是判断分页标记NULL，**cur是判断对象指针是否为NULL */
    if(*cur && **cur)
      MH_markObject(*cur);
  }
}

/* 在托管堆上移动对象，返回大小 */
size_t MH_moveObject(PObject from, PObject to)
{
  PRefLink cur;
  size_t siz;
  
  /* 移动对象内存 */
  siz = from->meta->size;
  if(from != to)
    memcpy(to, from, siz);

  /* 修正引用链 */
  for(cur = to->refs; cur; cur = to->refs)
  {
    /* 更改引用从ori到to */
    assert(*cur->ref == from);
    *cur->ref = to;

    /* 删除引用链 */
    to->refs = cur->next;
    free(cur);
  }

  return siz;
}

/* 整理托管堆 */
void MH_rearrange()
{
  PObject from, to;
  size_t siz;

  from = (PObject)MH_inst.buf;
  to = (PObject)MH_inst.buf;
  while((char*)from < MH_inst.top)
  {
    if(from->refs == NULL)
    {
      /* 如果是无引用对象，直接跳过 */
      from = (PObject)((char*)from + from->meta->size);
    }
    else
    {
      /* 否则移动对象 */
      siz = MH_moveObject(from, to);
      from = (PObject)((char*)from + siz);
      to = (PObject)((char*)to + siz);
    }
  }

  MH_inst.top = (char*)to;
}

/* 托管堆垃圾回收算法 */
void MH_collect()
{
  MH_cleanAllTag();
  MH_markRoot();
  MH_rearrange();
  MH_cleanAllTag();
}

/* 托管堆内存分配 */
PObject MH_alloc(Class klass)
{
  /* 满了就整理 */
  if(MH_inst.top + klass->size > MH_inst.buf + MH_inst.buflen)
  {
    MH_collect();
    assert(MH_inst.top + klass->size <= MH_inst.buf + MH_inst.buflen);
  }

  /* 然后直接连续分配 */
  PObject ret = (PObject)MH_inst.top;
  MH_inst.top += klass->size;
  return ret;
}

/* 增加栈上变量 */
void MH_addStackVar(PObject *var)
{
  assert(MH_inst.stack_top <= MH_inst.stack + MH_inst.stack_len);
  *MH_inst.stack_top = var;
  MH_inst.stack_top ++;
}

/* 初始化托管堆 */
void MH_init(size_t buflen, size_t var_count)
{
  MH_inst.buflen = buflen;
  MH_inst.buf = (char*)malloc(buflen);
  MH_inst.top = MH_inst.buf;

  MH_inst.stack_len = var_count;
  MH_inst.stack = (PObject**)malloc(sizeof(PObject*)* var_count);
  MH_inst.stack_top = MH_inst.stack;
}

/* 进入新的函数 */
void MH_enter(int var_cnt, ...)
{
  va_list ap;
  PObject* var;
  int i;

  /* 开启变参表 */
  va_start(ap, var_cnt);

  /* 设置栈分页标志NULL */
  MH_addStackVar(NULL);
  for(i = 0; i < var_cnt; i ++)
  {
    var = va_arg(ap, PObject*);
    MH_addStackVar(var);
  }
  /* 关闭变参表 */
  va_end(ap);
}

/* 离开函数 */
void MH_leave()
{
  MH_inst.stack_top --;
  while(*MH_inst.stack_top != NULL)
  {
    MH_inst.stack_top --;
  }
}

/* 载入类 */
void MH_loadClass(int class_cnt, ...)
{
  va_list ap;
  Class klass;
  int i;

  /* 开启变参表 */
  va_start(ap, class_cnt);
  for(i = 0; i < class_cnt; i ++)
  {
    klass = va_arg(ap, Class);
    klass->loader();
  }
  /* 关闭变参表 */
  va_end(ap);
}

void MH_final()
{
  free(MH_inst.buf);
  free(MH_inst.stack);
}

const ClassDesc Object_class =
{
  sizeof(Object),
  Obj_destroy,
  Obj_loader,
  NULL,
  NULL,
  0
};

PObject Obj_new(Class klass)
{
  assert(klass);
  PObject ret = MH_alloc(klass);
  ret->meta = klass;
  ret->refs = NULL;
  return ret;
}
