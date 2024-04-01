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
	*piMallocs = iMallocs_count;
	*piReallocs = iReallocs_count;
	*piFrees = iFrees_count;
}

static void* counter_malloc(const std_memoryhandler_t* pstMemoryHandler, size_t szSize)
{
	if (pstMemoryHandler) { /* Unused parameter */ }
	iMallocs_count++;
	return malloc(szSize);
}

static void* counter_realloc(const std_memoryhandler_t* pstMemoryHandler, void* pvOldPtr, size_t szSize)
{
	if (pstMemoryHandler) { /* Unused parameter */ }
	iReallocs_count++;
	return realloc(pvOldPtr, szSize);
}

static void counter_free(const std_memoryhandler_t* pstMemoryHandler, void* pvPtr)
{
	if (pstMemoryHandler) { /* Unused parameter */ }
	iFrees_count++;
	free(pvPtr);
}

std_memoryhandler_t stMemoryCounter =
{
	.pfn_Malloc		= &counter_malloc,
	.pfn_Realloc	= &counter_realloc,
	.pfn_Free		= &counter_free
};
