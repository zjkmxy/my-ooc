/*3:7*/

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
  assert(Class_kindOf(Queue_template(q), &IAdditive_class));
  PIAdditive val = (PIAdditive)Queue_front(q);
  val->print(val, buf, 256);
  printf("%s\n", buf);
}

int main()
{
  PIAdditive one = (PIAdditive)Int_create(new(Integer), 1);
  PIAdditive two = (PIAdditive)Float_create(new(Float), 2.0);
  PIAdditive three = one->plus(one, two);
  PIAdditive four = (PIAdditive)Int_create(new(Integer), 4);
  PIAdditive five = four->plus(four, one);
  PQueue q;

  q = Queue_create(new(Queue), &Integer_class);
  printBool(Queue_push(q, (PObject)three));
  printBool(Queue_push(q, (PObject)one));
  printBool(Queue_push(q, (PObject)five));
  printTop(q);
  Queue_pop(q);
  printTop(q);
  delete(q);

  q = Queue_create(new(Queue), &IAdditive_class);
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

