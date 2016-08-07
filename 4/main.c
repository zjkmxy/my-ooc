/*4:3*/

#include <stdio.h>
#include "msgqueue.h"
#include "basics.h"

void printBool(bool val)
{
  if(val)
    printf("true\n");
  else
    printf("false\n");
}

void printAdditive(PObject val_obj)
{
  static char buf[256];
  assert(Class_kindOf(Obj_classOf(val_obj), &IAdditive_class));
  PIAdditive val = (PIAdditive)val_obj;
  val->print(val, buf, 256);
  printf("Plachta: %s\n", buf);
}

void printFloat(PObject val_obj)
{
  static char buf[256];
  assert(Class_kindOf(Obj_classOf(val_obj), &Float_class));
  PIAdditive val = (PIAdditive)val_obj;
  val->print(val, buf, 256);
  printf("Float: %s\n", buf);
}

int main()
{
  PInteger one = Int_create(new(Integer), 1);
  PInteger two = Int_create(new(Integer), 2);
  PFloat three = Float_create(new(Float), 3.0);
  PMsgQueue q = MsgQueue_create(new(MsgQueue));

  /* 这时One会被拒收 */
  printBool(MsgQueue_post(q, (PObject)one));
  MsgQueue_spinOnce(q);

  MsgQueue_register(q, &IAdditive_class, printAdditive);
  MsgQueue_register(q, &Float_class, printFloat);

  /* 消息发送成功，2和3被托管 */
  printBool(MsgQueue_post(q, (PObject)two));
  printBool(MsgQueue_post(q, (PObject)three));

  /* 应该打印2和3.0 */
  MsgQueue_spinOnce(q);

  /* 需要手工删除1 */
  delete(q);
  delete(one);

  return 0;
}

