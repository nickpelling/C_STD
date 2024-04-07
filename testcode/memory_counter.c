/*
 * testcode/memory_counter.c

Copyright (c) 2024 Nick Pelling

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
 */

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
