/*5:3*/

#ifndef _MYSTRING_H
#define _MYSTRING_H

#include "object.h"

struct String;
typedef struct String String;
typedef struct String *PString;

PString Str_create(PString ret, char* buf);
PString Str_clone(PString self);
char Str_at(PString self, size_t pos);
void Str_setAt(PString *self, size_t pos, char ch);
void Str_print(PString self);
size_t Str_length(PString self);

extern const ClassDesc String_class;

#endif
