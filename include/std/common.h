/*
 * std/common.h
 *
 *  Created on: 15 Oct 2022
 *      Author: Nick Pelling
 */

#ifndef STD_COMMON_H_
#define STD_COMMON_H_

#include <stddef.h>		// for size_t
#include <stdint.h>		// for uint32_t etc
#include <stdbool.h>	// for bool

#include "std/config.h"
#include "std/enums.h"

// Headers for the item handlers
#include "std/item.h"
#include "std/lock.h"
#include "std/memory.h"

typedef int  (*pfn_std_compare_t)(const void * a, const void * b);

// Field construction helper macros (for making typed version of untyped callbacks)
#define STD_COMPARE(T, FIELD)	int (*FIELD)(T * a, T * b)

typedef struct
{
	const std_item_handler_t  * pstItemHandler;
	const std_memoryhandler_t * pstMemoryHandler;
	const std_lock_handler_t  * pstLockHandler;
} std_container_handlers_t;

#define STD_ITEMHANDLER(X)		.pstItemHandler = X
#define STD_MEMORYHANDLER(X)	.pstMemoryHandler = X
#define STD_LOCKHANDLER(X)		.pstLockHandler = X

typedef struct
{
	size_t						szSizeofItem;
	size_t						szNumItems;			// Number of items currently in the container
	std_container_has_t			eHas;
	const std_item_handler_t	* pstItemHandler;
	const std_memoryhandler_t	* pstMemoryHandler;
	const std_lock_handler_t	* pstLockHandler;
	std_lock_handle_t			  phLock;			// Opaque handle to a lock instance
} std_container_t;

STD_INLINE bool std_container_constructor(std_container_t * pstContainer, size_t szSizeofItem, std_container_has_t eHas, const std_container_handlers_t * pstHandlers)
{
	bool bResult;

	bResult = true;
	pstContainer->szNumItems = 0;
	pstContainer->szSizeofItem = szSizeofItem;
	pstContainer->eHas = eHas;

	if (eHas & std_container_has_itemhandler)
	{
		if (pstHandlers->pstItemHandler == NULL)
		{
			bResult = false;
		}
		else
		{
			pstContainer->pstItemHandler = pstHandlers->pstItemHandler;
		}
	}
	else
	{
		if (pstHandlers->pstItemHandler != NULL)
		{
			bResult = false;
		}
	}

	if (eHas & std_container_has_memoryhandler)
	{
		if (pstHandlers->pstMemoryHandler == NULL)
		{
			bResult = false;
		}
		else
		{
			pstContainer->pstMemoryHandler = pstHandlers->pstMemoryHandler;
		}
	}
	else
	{
		if (pstHandlers->pstMemoryHandler != NULL)
		{
			bResult = false;
		}
	}

	if (eHas & std_container_has_lockhandler)
	{
		if (pstHandlers->pstLockHandler == NULL)
		{
			bResult = false;
		}
		else
		{
			pstContainer->pstLockHandler = pstHandlers->pstLockHandler;
			std_lock_construct(pstContainer->pstLockHandler, &pstContainer->phLock);
		}
	}
	else
	{
		if (pstHandlers->pstLockHandler != NULL)
		{
			bResult = false;
		}
	}

	return bResult;
}

// Note: every base class should include an std_item_handler_t
#define STD_ITEM_GET(CONTAINER)		stBody.stContainer.pstItemHandler

// Note that because all containers AND iterators use the same type / const type
// smuggling mechanism, the following two groups of macros work exactly the same
// way for ALL of them.
#define STD_ITEM_SIZEOF(CONTAINER)			  sizeof(CONTAINER.pstType[0])
#define STD_ITEM_TYPEOF(CONTAINER)			  STD_TYPEOF(CONTAINER.pstType[0])
#define STD_ITEM_ALIGNOF(CONTAINER)			  STD_ALIGNOF(CONTAINER.pstType[0])
#define STD_ITEM_CAST(CONTAINER,X)			((STD_TYPEOF(CONTAINER.pstType[0]))(X))
#define STD_ITEM_PTR_CAST(CONTAINER,X)		((STD_TYPEOF(CONTAINER.pstType[0]) *)(X))
#define STD_ITEM_PTR_CAST_CONST(CONTAINER,X) ((STD_TYPEOF(CONTAINER.pstType) const)(X))

#define STD_CONST_COMPARE_TYPEOF(CONTAINER)	  STD_TYPEOF(CONTAINER.pfnCompare)
#define STD_CONST_COMPARE_CAST(CONTAINER,X)	((STD_TYPEOF(CONTAINER.pfnCompare))(X))

#define STD_CONTAINER_ENUM_SET(ENUM)				STD_ENUM_CONTAINER_SET(ENUM) pau8ContainerEnum
#define STD_CONTAINER_ENUM_GET(CONTAINER)			STD_ENUM_CONTAINER_GET(CONTAINER.pau8ContainerEnum)

#define STD_CONTAINER_HAS_SET(ENUM)					STD_ENUM_HAS_SET(ENUM) pau8HasHandler
#define STD_CONTAINER_HAS_GET(CONTAINER)			STD_ENUM_HAS_GET(CONTAINER.pau8HasHandler)
#define STD_CONTAINER_HAS_ITEMHANDLER(CONTAINER)	(STD_CONTAINER_HAS_GET(CONTAINER) & std_container_has_itemhandler)
#define STD_CONTAINER_HAS_MEMORYHANDLER(CONTAINER)	(STD_CONTAINER_HAS_GET(CONTAINER) & std_container_has_memoryhandler)
#define STD_CONTAINER_HAS_LOCKHANDLER(CONTAINER)	(STD_CONTAINER_HAS_GET(CONTAINER) & std_container_has_lockhandler)

#define STD_CONTAINER_WRAPPEDITEM_SIZEOF_SET(SIZE)			uint8_t (*pau8FullSizeof)[(SIZE) + 1U]
#define STD_CONTAINER_WRAPPEDITEM_SIZEOF_GET(CONTAINER)		((size_t)(sizeof(CONTAINER.pau8FullSizeof[0]) - 1U))

#define STD_CONTAINER_PAYLOAD_OFFSET_SET(OFFSET)	uint8_t (*pau8PayloadOffset)[(OFFSET) + 1U]
#define STD_CONTAINER_PAYLOAD_OFFSET_GET(CONTAINER)	((size_t)(sizeof(CONTAINER.pau8PayloadOffset[0]) - 1U))

#define STD_CONTAINER_IMPLEMENTS_SET(IMPLEMENTS)	uint8_t (*pau8Implements)[(IMPLEMENTS) + 1U]
#define STD_CONTAINER_IMPLEMENTS_GET(CONTAINER)		((std_container_implements_t)(sizeof(CONTAINER.pau8Implements[0]) - 1U))

#endif /* STD_COMMON_H_ */
