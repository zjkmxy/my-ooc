#include "sequence.h"
#include "list.h"
#include "array.h"
#include "stream.h"

int main()
{
  PSequence list = (PSequence)List_create();
  PSequence array = (PSequence)Array_create(10);
  PStream stream = Stream_create(1, 1);

  list->append(list, 1);
  list->append(list, 2);
  list->append(list, 3);
  list->append(list, 4);
  list->append(list, 5);
  list->append(list, 6);

  array->append(array, 2);
  array->append(array, 3);
  array->append(array, 5);
  array->append(array, 7);
  array->append(array, 11);
  array->append(array, 13);

  Seq_print(list, 10);
  Seq_print(array, 4);
  Seq_print((PSequence)stream, 10);

  list->free(list);
  array->free(array);
  stream->base_seq.free((PSequence)(stream));

  return 0;
}
