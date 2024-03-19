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
