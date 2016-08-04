/*2:2*/

#include <stdio.h>
#include "sequence.h"

/* 
 * Sequence类的成员方法只依赖于Sequence和Iterator类的虚方法，
 * 和具体实现（List/Array）无关。
 * 因此我们可以任意地添加普适性的算法，而不需要考虑到底有多少种Sequence。
 */

int Seq_getSize(PSequence self)
{
  return self->size;
}

void Seq_print(PSequence self, int maxLength)
{
  PIterator it = self->begin(self);
  BOOL first = TRUE;
  int i = 0;

  while(!it->isEnd(it) && i < maxLength)
  {
    if(first)
    {
      first = FALSE;
      printf("{%d", it->getVal(it));
    }
    else
    {
      printf(", %d", it->getVal(it));
    }
    it->getNext(it);
    i ++;
  }

  if(!it->isEnd(it))
  {
    printf(",...");
  }
  printf("}\n");
  it->free(it);
}

