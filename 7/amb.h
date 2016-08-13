/*7:1*/

#ifndef _AMB_H
#define _AMB_H

#include <stddef.h>
#include <stdbool.h>
#include <assert.h>
#include <stdarg.h>
#include <setjmp.h>

struct Amb;
typedef struct Amb Amb;
typedef struct Amb *PAmb;

struct AmbCont;
typedef struct AmbCont AmbCont;
typedef struct AmbCont *PAmbCont;

#define JMP_OK    0
#define JMP_FAIL  1
#define OPTIONAL

struct AmbCont
{
  void(*run)(PAmbCont self, PAmb sender, int ret);
};

PAmb Amb_create(jmp_buf *fail);
bool Amb_choose(PAmb self, PAmbCont cont, int cnt, ...);
bool Amb_choosev(PAmb self, PAmbCont cont, int cnt, va_list ap);
void Amb_fail(PAmb self);
void Amb_free(PAmb self);

#endif
