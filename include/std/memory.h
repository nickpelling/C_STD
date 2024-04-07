/*
 * std/memory.h

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

#ifndef STD_MEMORY_H_
#define STD_MEMORY_H_

#include <stddef.h>		// for size_t
#include <stdlib.h>		// for malloc/realloc/free

#include "std/config.h"
#include "std/enums.h"	// for std_container_has_t

typedef struct std_memoryhandler_s std_memoryhandler_t;

struct std_memoryhandler_s
{
	void * (*pfn_Malloc)(	const std_memoryhandler_t * pstMemoryHandler, size_t szSize);
	void * (*pfn_Realloc)(	const std_memoryhandler_t * pstMemoryHandler, void * pvData, size_t szSize);
	void   (*pfn_Free)(		const std_memoryhandler_t * pstMemoryHandler, void * pvData);
};

inline void * std_memoryhandler_malloc(const std_memoryhandler_t * pstMemoryHandler, std_container_has_t eHas, size_t szSize)
{
	// Try to sidestep many of the C-library-related issues to do with malloc() and realloc()
	if (szSize == 0U)
	{
		return NULL;
	}
	if (eHas & std_container_has_memoryhandler)
	{
		return (*pstMemoryHandler->pfn_Malloc)(pstMemoryHandler, szSize);
	}
	return malloc(szSize);
}

inline void std_memoryhandler_free(const std_memoryhandler_t * pstMemoryHandler, std_container_has_t eHas, void * pvData)
{
	if (eHas & std_container_has_memoryhandler)
	{
		(*pstMemoryHandler->pfn_Free)(pstMemoryHandler, pvData);
		return;
	}
	free(pvData);
}

inline void* std_memoryhandler_realloc(const std_memoryhandler_t* pstMemoryHandler, std_container_has_t eHas, void* pvData, size_t szSize)
{
	// Try to sidestep many of the C-library-related issues to do with malloc() and realloc()
	if (szSize == 0U)
	{
		std_memoryhandler_free(pstMemoryHandler, eHas, pvData);
		return NULL;
	}
	if (eHas & std_container_has_memoryhandler)
	{
		return (*pstMemoryHandler->pfn_Realloc)(pstMemoryHandler, pvData, szSize);
	}
	return realloc(pvData, szSize);
}

#endif /* STD_MEMORYHANDLER_H_ */
