#ifndef __MTWISTER_H
#define __MTWISTER_H

#define STATE_VECTOR_LENGTH 624
#define STATE_VECTOR_M      397 /* changes to STATE_VECTOR_LENGTH also require changes to this */

#include "types.h"

typedef struct tagMTRand {
  unsigned long mt[STATE_VECTOR_LENGTH];
  int index;
} MTRand;

MTRand seedRand(unsigned long seed);
unsigned long genRandLong(MTRand* rand);
float genRand(MTRand* rand);
u32 random_in_range(MTRand *twister, u32 max);

#endif /* #ifndef __MTWISTER_H */