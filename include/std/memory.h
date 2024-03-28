/*
 * std/memory.h
 *
 *  Created on: 22 Oct 2022
 *      Author: Nick Pelling
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
