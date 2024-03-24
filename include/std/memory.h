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

#include "std/enums.h"	// for std_container_has_t

typedef struct std_memoryhandler_s std_memoryhandler_t;

struct std_memoryhandler_s
{
	void * (*pfn_Malloc)(	const std_memoryhandler_t * pstMemoryHandler, size_t szSize);
	void * (*pfn_Realloc)(	const std_memoryhandler_t * pstMemoryHandler, void * pvData, size_t szSize);
	void   (*pfn_Free)(		const std_memoryhandler_t * pstMemoryHandler, void * pvData);
};

static inline void * std_memoryhandler_malloc(const std_memoryhandler_t * pstMemoryHandler, std_container_has_t eHas, size_t szSize)
{
	if (eHas & std_container_has_memoryhandler)
	{
		return (*pstMemoryHandler->pfn_Malloc)(pstMemoryHandler, szSize);
	}
	return malloc(szSize);
}

static inline void * std_memoryhandler_realloc(const std_memoryhandler_t * pstMemoryHandler, std_container_has_t eHas, void * pvData, size_t szSize)
{
	if (eHas & std_container_has_memoryhandler)
	{
		return (*pstMemoryHandler->pfn_Realloc)(pstMemoryHandler, pvData, szSize);
	}
	return realloc(pvData, szSize);
}

static inline void std_memoryhandler_free(const std_memoryhandler_t * pstMemoryHandler, std_container_has_t eHas, void * pvData)
{
	if (eHas & std_container_has_memoryhandler)
	{
		(*pstMemoryHandler->pfn_Free)(pstMemoryHandler, pvData);
	}
	else
	{
		free(pvData);
	}
}

#endif /* STD_MEMORYHANDLER_H_ */
