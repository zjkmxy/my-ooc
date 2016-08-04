#include <stdio.h>
#include "sequence.h"

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

