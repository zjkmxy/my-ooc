/*8:4*/

#include "mheap.h"
#include <stdlib.h>
#include <memory.h>

/* 托管堆 */
struct ManagedHeap
{
  /* 空余内存 */
  char *buf;

  /* 空余内存尾 */
  char *buf_end;

  /* 对象堆顶 */
  char *heap_top;

  /*
  * 句柄堆顶
  * 句柄堆和对象堆从两头对冲
  */
  Handle handle_top;

  /*
  * 句柄堆空余链表头
  * 因为句柄是二级指针中的外一层所以不能移动：
  * Handle h ; *h -> ptr ; *ptr -> obj;
  * obj移动时改*h = ptr的值，但是h本身不能改
  * 因此我们只能用空余链表管理
  * 如果指向handle_top，我们必须扩充句柄堆
  */
  Handle handle_idie;

  /*
  * 局部句柄栈
  * 存储函数中临时产生的对象的句柄
  * 就是原来的根对象变量栈
  * 不同的是，原来我们需要记录所有的变量
  * 现在我们只需要在每个对象刚创建好的时候把句柄丢进来就行。
  */
  Handle *stack;
  size_t stack_len;
  Handle *stack_top;
} MH_inst;

/* 检查所有的标记 */
void MH_checkAllTag()
{
  char* cur = MH_inst.buf;
  PObject obj = (PObject)cur;

  while(cur < MH_inst.heap_top)
  {
    /* 在垃圾回收执行前，不应当有访问标记存在 */
    assert(obj->access == false);
    cur += obj->size;
  }
}

/* 递归标记一个对象 */
void MH_markObject(PObject obj)
{
  PSlot slot;
  size_t i;

  /* 如果已经标记，直接回退 */
  if(obj->access)
    return ;
  obj->access = true;

  /* 如果是初次标记，就递归遍历它的槽 */
  for(i = 0; i < obj->slot_cap; i ++)
  {
    slot = &obj->slots[i];
    if(slot->name != NIL && slot->val != NULL)
    {
      /* 每一个有效的句柄不允许指向空白的对象 */
      assert(*slot->val);
      MH_markObject(*slot->val);
    }
  }
}

/* 递归标记根对象 */
void MH_markRoot()
{
  Handle *cur;
  for(cur = MH_inst.stack; cur < MH_inst.stack_top; cur ++)
  {
    /* 这里*cur是判断分页标记NULL */
    if(*cur)
    {
      /* 每一个有效的句柄不允许指向空白的对象 */
      assert(**cur);
      MH_markObject(**cur);
    }
  }
}

/* 在托管堆上移动并压缩对象，返回新的大小 */
size_t MH_moveObject(PObject from, PObject to)
{
  size_t siz;

  /* 对象的压缩，每次最多减半一次 */
  Obj_compressSlot(from);

  /* 复制对象，返回大小 */
  siz = from->size;
  if(from != to)
  {
    memcpy(to, from, siz);
    /* 这里只需要调整句柄就好了，不需要调整所有引用 */
    *to->handle = to;
  }

  /* 清空访问标记 */
  to->access = false;

  return siz;
}

/* 移除一个无引用对象的句柄 */
void MH_removeHandle(Handle handle)
{
  *handle = (PObject)MH_inst.handle_idie;
  MH_inst.handle_idie = handle;
}

/* 整理托管堆 */
void MH_rearrange()
{
  PObject from, to;
  size_t siz, siz_ori;

  from = (PObject)MH_inst.buf;
  to = (PObject)MH_inst.buf;
  while((char*)from < MH_inst.heap_top)
  {
    siz_ori = from->size;
    if(!from->access)
    {
      /* 如果是无引用对象，消除句柄并跳过 */
      MH_removeHandle(from->handle);
      from = (PObject)((char*)from + siz_ori);
    }
    else
    {
      /* 否则移动对象 */
      siz = MH_moveObject(from, to);
      from = (PObject)((char*)from + siz_ori);
      to = (PObject)((char*)to + siz);
    }
  }

  MH_inst.heap_top = (char*)to;
}

/* 托管堆垃圾回收算法 */
void MH_collect()
{
  MH_checkAllTag();
  MH_markRoot();
  MH_rearrange();
}

/* 分配新的对象句柄 */
Handle MH_newHandle(PObject obj)
{
  Handle ret;

  if(MH_inst.handle_idie > MH_inst.handle_top)
  {
    /* 如果空闲链表没满，就直接分配 */
    ret = MH_inst.handle_idie;
    MH_inst.handle_idie = (Handle)*ret;
  } else
  {
    /* 否则试图扩容 */
    ret = MH_inst.handle_idie;
    if((char*)ret <= MH_inst.heap_top)
      MH_collect();
    assert((char*)ret > MH_inst.heap_top);
    MH_inst.handle_top --;
    MH_inst.handle_idie --;
  }
  *ret = obj;
  return ret;
}

/* 托管堆内存分配，请不要人工调用 */
PObject MH_alloc(size_t siz)
{
  /* 满了就整理 */
  if(MH_inst.heap_top + siz > (char*)MH_inst.handle_top)
  {
    MH_collect();
    assert(MH_inst.heap_top + siz <= (char*)MH_inst.handle_top);
  }

  /* 然后直接连续分配 */
  PObject ret = (PObject)MH_inst.heap_top;
  MH_inst.heap_top += siz;

  /* 设置访问标志，不管句柄，也不设置大小 */
  ret->access = false;

  return ret;
}

/* 增加栈上句柄 */
void MH_addStackVar(Handle handle)
{
  assert(MH_inst.stack_top <= MH_inst.stack + MH_inst.stack_len);
  *MH_inst.stack_top = handle;
  MH_inst.stack_top ++;
}

/* 进入新的函数 */
void MH_enter()
{
  MH_addStackVar(NULL);
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

/* 初始化托管堆 */
void MH_init(size_t buflen, size_t var_count)
{
  MH_inst.buf = (char*)malloc(buflen);
  MH_inst.heap_top = MH_inst.buf;
  MH_inst.buf_end = MH_inst.buf + buflen;

  MH_inst.stack_len = var_count;
  MH_inst.stack = (Handle*)malloc(sizeof(Handle) * var_count);
  MH_inst.stack_top = MH_inst.stack;

  MH_inst.handle_top = ((Handle)MH_inst.buf_end) - 1;
  MH_inst.handle_idie = MH_inst.handle_top;
}

/* 销毁托管堆 */
void MH_final()
{
  free(MH_inst.buf);
  free(MH_inst.stack);
}

void MH_expandObject(Handle obj, size_t new_siz)
{
  PObject from = *obj;
  PObject to = MH_alloc(new_siz);
  memcpy(to, from, from->size);
  to->size = new_siz;
  *obj = to;
}

void MH_return(Handle handle)
{
  MH_leave();
  /* 这里我们必须将返回值再次入栈，否则将因为函数的退出而被销毁 */
  if(handle)
  {
    MH_addStackVar(handle);
  }
}
