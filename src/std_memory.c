#include <stdlib.h>		// for malloc/realloc/free

#include "std/memory.h"

static int iMallocs_count;
static int iReallocs_count;
static int iFrees_count;

static void* wrapped_malloc(const std_memoryhandler_t* pstMemoryHandler, size_t szSize)
{
	return malloc(szSize);
}

static void* wrapped_realloc(const std_memoryhandler_t* pstMemoryHandler, void* pvOldPtr, size_t szSize)
{
	return realloc(pvOldPtr, szSize);
}

static void wrapped_free(const std_memoryhandler_t* pstMemoryHandler, void* pvPtr)
{
	free(pvPtr);
}

std_memoryhandler_t std_memoryhandler_default =
{
	.pfn_Malloc		= &wrapped_malloc,
	.pfn_Realloc	= &wrapped_realloc,
	.pfn_Free		= &wrapped_free
};

void std_memoryhandler_counter_reset(void)
{
	iMallocs_count = 0;
	iReallocs_count = 0;
	iFrees_count = 0;
}

void std_memoryhandler_counter_grab(int *piMallocs, int *piReallocs, int *piFrees)
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

std_memoryhandler_t std_memoryhandler_counter =
{
	.pfn_Malloc		= &counter_malloc,
	.pfn_Realloc	= &counter_realloc,
	.pfn_Free		= &counter_free
};
