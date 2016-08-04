#ifndef _STREAM_H
#define _STREAM_H

#include "sequence.h"

struct Stream;
typedef struct Stream Stream;
typedef struct Stream *PStream;

struct Stream
{
  Sequence base_seq;
  Iterator base_it;
  int x;
  int y;
};

PStream Stream_create(int x, int y);

#endif
