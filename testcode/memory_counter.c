#include <stdlib.h>		// for malloc/realloc/free

#include "std/memory.h"

static int iMallocs_count = 0;
static int iReallocs_count = 0;
static int iFrees_count = 0;

void memorycounter_reset(void)
{
	iMallocs_count = 0;
	iReallocs_count = 0;
	iFrees_count = 0;
}

void memorycounter_grab(int *piMallocs, int *piReallocs, int *piFrees)
{
	iMallocs_count = 0;
	iReallocs_count = 0;
	iFrees_count = 0;
}

static void* counter_malloc(const std_memoryhandler_t* pstMemoryHandler, size_t szSize)
{
	iMallocs_count++;
	return malloc(szSize);
}

static void* counter_realloc(const std_memoryhandler_t* pstMemoryHandler, void* pvOldPtr, size_t szSize)
{
	iReallocs_count++;
	return realloc(pvOldPtr, szSize);
}

static void counter_free(const std_memoryhandler_t* pstMemoryHandler, void* pvPtr)
{
	iFrees_count++;
	free(pvPtr);
}

std_memoryhandler_t stMemoryCounter =
{
	.pfn_Malloc		= &counter_malloc,
	.pfn_Realloc	= &counter_realloc,
	.pfn_Free		= &counter_free
};
