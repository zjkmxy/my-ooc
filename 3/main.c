#include <stdio.h>
#include "queue.h"
#include "basics.h"

void printBool(bool val)
{
  if(val)
    printf("true\n");
  else
    printf("false\n");
}

void printTop(PQueue q)
{
  static char buf[256];
  assert(Class_kindOf(Queue_template(q), &Plachta_class));
  PPlachta val = (PPlachta)Queue_front(q);
  val->print(val, buf, 256);
  printf("%s\n", buf);
}

int main()
{
  PPlachta one = (PPlachta)Int_create(new(Integer), 1);
  PPlachta two = (PPlachta)Float_create(new(Float), 2.0);
  PPlachta three = one->plus(one, two);
  PPlachta four = (PPlachta)Int_create(new(Integer), 4);
  PPlachta five = four->plus(four, one);
  PQueue q;

  q = Queue_create(new(Queue), &Integer_class);
  printBool(Queue_push(q, (PObject)three));
  printBool(Queue_push(q, (PObject)one));
  printBool(Queue_push(q, (PObject)five));
  printTop(q);
  Queue_pop(q);
  printTop(q);
  delete(q);

  q = Queue_create(new(Queue), &Plachta_class);
  printBool(Queue_push(q, (PObject)three));
  printBool(Queue_push(q, (PObject)one));
  printBool(Queue_push(q, (PObject)five));
  printTop(q);
  Queue_pop(q);
  printTop(q);
  delete(q);

  delete(one);
  delete(two);
  delete(three);
  delete(four);
  delete(five);

  return 0;
}

