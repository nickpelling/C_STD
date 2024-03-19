#ifndef _MEMORY_COUNTER_H_
#define _MEMORY_COUNTER_H_

#include "std/memory.h"

extern std_memoryhandler_t stdMemoryCounter;

extern void memorycounter_reset(void);
extern void memorycounter_grab(int* piMallocs, int* piReallocs, int* piFrees);

#endif /* _MEMORY_COUNTER_H_ */