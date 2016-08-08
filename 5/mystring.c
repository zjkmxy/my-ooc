#include "mystring.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static void Str_destroy(PObject self);

struct String
{
  Object base;
  size_t length;
  char *buf;
};

const ClassDesc String_class = {
  {&Class_class, 1},
  sizeof(String),
  Str_destroy,
  &Object_class
};


PString Str_create(PString ret, char* buf)
{
  ret->length = strlen(buf);
  ret->buf = (char*)malloc(sizeof(char) * (ret->length + 1));
  strcpy(ret->buf, buf);
  return ret;
}

PString Str_clone(PString self)
{
  /* 复制时传自身引用 */
  addref(self);
  return self;
}

char Str_at(PString self, size_t pos)
{
  assert(pos <= self->length);
  return self->buf[pos];
}

void Str_setAt(PString *self, size_t pos, char ch)
{
  PString ret = *self;
  assert(pos <= (*self)->length);
  /* 修改时如果是多重引用，就复制一个 */
  if(((PObject)*self)->ref > 1)
  {
    ((PObject)*self)->ref --;
    ret = Str_create(new(String), (*self)->buf);
  }
  ret->buf[pos] = ch;
  *self = ret;
}

void Str_print(PString self)
{
  puts(self->buf);
}

size_t Str_length(PString self)
{
  return self->length;
}

void Str_destroy(PObject self)
{
  free(((PString)self)->buf);
}

