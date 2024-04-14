/*
 * std/common.h

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

typedef struct std_container_s
{
	size_t						szSizeofItem;		// Size of each item in the container
	size_t						szNumItems;			// Number of items currently in the container
	std_container_has_t			eHas;
	const std_item_handler_t	* pstItemHandler;
	const std_memoryhandler_t	* pstMemoryHandler;
	const std_lock_handler_t	* pstLockHandler;
	std_lock_handle_t			  phLock;			// Opaque handle to a lock instance
} std_container_t;

STD_INLINE void std_container_constructor(std_container_t * pstContainer, size_t szSizeofItem, std_container_has_t eHas)
{
	pstContainer->szNumItems = 0;
	pstContainer->szSizeofItem = szSizeofItem;
	pstContainer->eHas = eHas;
}

// Note: every base class should include an std_item_handler_t
#define STD_ITEM_GET(CONTAINER)		stBody.stContainer.pstItemHandler

// Note that because all containers AND iterators use the same type / const type
// smuggling mechanism, the following two groups of macros work exactly the same
// way for ALL of them.
#define STD_ITEM(CONTAINER)						CONTAINER.pstType[0]
#define STD_ITEM_SIZEOF(CONTAINER)				sizeof(STD_ITEM(CONTAINER))
#define STD_ITEM_TYPEOF(CONTAINER)				STD_TYPEOF(STD_ITEM(CONTAINER))
#define STD_ITEM_ALIGNOF(CONTAINER)				STD_ALIGNOF(STD_ITEM(CONTAINER))
#define STD_ITEM_CAST(CONTAINER,X)				((STD_ITEM_TYPEOF(CONTAINER))(X))
#define STD_ITEM_PTR_CAST(CONTAINER,X)			((STD_ITEM_TYPEOF(CONTAINER) *)(X))
#define STD_ITEM_PTR_CAST_CONST(CONTAINER,X)	((STD_TYPEOF(CONTAINER.pstType) const)(X))

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
