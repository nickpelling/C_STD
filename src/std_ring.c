/*
 * src/ring.c

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

#include <stdlib.h>		// for qsort
#include <stdarg.h>		// for variadic args

#include "std/item.h"
#include "std/ring.h"

#define CONTAINER_TO_RING(CONTAINER)	STD_CONTAINER_OF(CONTAINER, std_ring_t, stContainer)
#define RING_TO_CONTAINER(RING)			&RING->stContainer

#define ITERATOR_TO_RINGIT(IT)			STD_CONTAINER_OF(IT, std_ring_iterator_t, stIterator)
#define RINGIT_TO_ITERATOR(RINGIT)		&RINGIT->stIterator

/**
 * Step a ring pointer forwards by the size of an iterator's item
 * 
 * @param[in]	pstIterator		Iterator
 * @param[in]	pvThis			Pointer
 * 
 * @return New pointer value
 */
static inline void * next_item(std_iterator_t * pstIterator, void * pvThis)
{
	std_ring_iterator_t* pstRingIt = ITERATOR_TO_RINGIT(pstIterator);
	pvThis = STD_LINEAR_ADD(pvThis, pstIterator->szSizeofItem);
	if (pvThis >= pstRingIt->pvRingEnd)
	{
		pvThis = pstRingIt->pvRingStart;
	}
	return pvThis;
}

/**
 * Step a ring pointer backwards by the size of an iterator's item
 *
 * @param[in]	pstIterator		Iterator
 * @param[in]	pvThis			Pointer
 *
 * @return New pointer value
 */
static inline void * prev_item(std_iterator_t * pstIterator, void * pvThis)
{
	std_ring_iterator_t* pstRingIt = ITERATOR_TO_RINGIT(pstIterator);
	if (pvThis <= pstRingIt->pvRingStart)
	{
		pvThis = pstRingIt->pvRingEnd;
	}
	return STD_LINEAR_SUB(pvThis, pstIterator->szSizeofItem);
}

/**
 * Calculate the address of an indexed entry in a ring container
 *
 * @param[in]	pstContainer	Ring container
 * @param[in]	szIndex			Index
 *
 * @return Address of the indexed entry
 */
inline void * stdlib_ring_at(std_container_t * pstContainer, size_t szIndex)
{
	std_ring_t * pstRing = CONTAINER_TO_RING(pstContainer);
	szIndex = (szIndex + pstRing->szStartOffset) & (pstRing->szNumAlloced - 1U);
	return STD_LINEAR_ADD(pstRing->pvStartAddr, szIndex * pstContainer->szSizeofItem);
}

// Force the compiler to emit a non-inline version
void* stdlib_ring_at(std_container_t* pstContainer, size_t szIndex);

/**
 * Construct a ring container
 *
 * @param[in]	pstContainer	Ring to initialise
 * @param[in]	szFullSizeof	Size of a (possibly wrapped) item
 * @param[in]	szPayloadOffset	Offset to the payload within the (possibly wrapped) item
 * @param[in]	eHas			Enumeration saying which handlers to expect
 */
void stdlib_ring_construct(std_container_t* pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas)
{
	if (szWrappedSizeof || szPayloadOffset) { /* Unused parameters */ }
	std_container_constructor(pstContainer, szSizeof, eHas);
	std_ring_t * pstRing = CONTAINER_TO_RING(pstContainer);
	pstRing->szNumAlloced	= 0;
	pstRing->pvStartAddr	= NULL;
	pstRing->szStartOffset	= 0;
}

/**
 * Destruct a ring container
 *
 * @param[in]	pstContainer	Ring container to destruct
 * 
 * @return True if destruction was successful, else false
 */
bool stdlib_ring_destruct(std_container_t * pstContainer)
{
	std_ring_t * pstRing = CONTAINER_TO_RING(pstContainer);
	size_t i;

	if (pstContainer == NULL)
	{
		return false;
	}

	// If an item handler is attached AND that item handler has a destructor, destruct each item in the container
	if (	(pstContainer->eHas & std_container_has_itemhandler)
		&&	(pstContainer->pstItemHandler->pfn_Destructor != NULL)	)
	{
		for (i = 0; i < pstContainer->szNumItems; i++)
		{
			void * pvItem = stdlib_ring_at(pstContainer, i);
			(*pstContainer->pstItemHandler->pfn_Destructor)(pstContainer->pstItemHandler, pvItem);
		}
	}

	// Free the memory allocated for this ring
	std_memoryhandler_free(pstContainer->pstMemoryHandler, pstContainer->eHas, pstRing->pvStartAddr);

	// Clear out all the fields (just to be tidy)
	pstContainer->szNumItems	= 0;
	pstRing->szNumAlloced		= 0;
	pstRing->pvStartAddr		= NULL;
	pstRing->szStartOffset		= 0;

	return true;
}

/**
 * Reserve space within a ring container for a number of items
 *
 * @param[in]	pstContainer	Ring container to reserve space for
 * @param[in]	szNewSize		New number of items
 * 
 * @return True if was able to reserve the memory OK, else false
 */
bool stdlib_ring_reserve(std_container_t * pstContainer, size_t szNewSize)
{
	std_ring_t * pstRing = CONTAINER_TO_RING(pstContainer);
	size_t szNewCapacity;
	void * pvNewStart;

	if (szNewSize > pstRing->szNumAlloced)
	{
		szNewCapacity = 1ULL << (64U - __builtin_clzll(szNewSize));
		if (szNewCapacity < szNewSize)
		{
			szNewCapacity <<= 1;
		}

		if (szNewCapacity != pstRing->szNumAlloced)
		{
			pstRing->szNumAlloced = szNewCapacity;
			size_t szTotalSize = pstRing->szNumAlloced * pstContainer->szSizeofItem;
			pvNewStart = std_memoryhandler_realloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstRing->pvStartAddr, szTotalSize);
			if (pvNewStart == NULL)
			{
				return false;
			}
			if (pstContainer->eHas & std_container_has_itemhandler)
			{
				stdlib_item_relocate(pstContainer->pstItemHandler, pvNewStart, pstRing->pvStartAddr, szTotalSize);
			}
			pstRing->pvStartAddr = pvNewStart;
		}
	}

	return true;
}

/**
 * Push a series of items onto the front of a ring
 *
 * @param[in]	pstContainer	Ring container to push the series of items onto
 * @param[in]	pstSeries		Series of items to push onto the ring
 * 
 * @return Number of items pushed onto the ring
 */
size_t stdlib_ring_push_front(std_container_t * pstContainer, std_linear_series_t* pstSeries)
{
	std_ring_t * pstRing = CONTAINER_TO_RING(pstContainer);
	size_t szSizeofItem = pstContainer->szSizeofItem;
	size_t szOldCount = pstContainer->szNumItems;
	size_t szNumItems = pstSeries->szNumItems;
	size_t szNewCount = szOldCount + szNumItems;
	void * pvItem;

	if ((szNumItems == 0) || (pstSeries->pvData == NULL))
	{
		return 0;
	}

	// Try to reserve space, exit early if that didn't succeed
	if (stdlib_ring_reserve(pstContainer, szNewCount) == false)
	{
		return 0;
	}

	for (; !std_linear_series_done(pstSeries); std_linear_series_next(pstSeries), pvItem = STD_LINEAR_SUB(pvItem, szSizeofItem))
	{
		if (pstRing->szStartOffset == 0)
		{
			pstRing->szStartOffset = pstRing->szNumAlloced;
		}
		pstRing->szStartOffset--;
		pstContainer->szNumItems++;

		pvItem = stdlib_ring_at(pstContainer, 0);
		stdlib_container_relocate_items(pstContainer, pvItem, pstSeries->pvData, 1);
	}

	// Return the number of items successfully pushed onto the container
	return szNumItems;
}

/**
 * Push a series of items onto the back of a ring
 *
 * @param[in]	pstContainer	Ring container to push the series of items onto
 * @param[in]	pvBase			Pointer to start of array of items
 * @param[in]	szNumItems		Number of items in the array
 *
 * @return Number of items pushed onto the ring
 */
size_t stdlib_ring_push_back(std_container_t * pstContainer, std_linear_series_t* pstSeries)
{
	size_t szOldCount = pstContainer->szNumItems;
	size_t szNumItems = pstSeries->szNumItems;
	size_t szNewCount = szOldCount + szNumItems;
	void * pvItem;
	size_t i;

	if ((szNumItems == 0) || (pstSeries->pvData == NULL))
	{
		return 0;
	}

	// Try to reserve space, exit early if that didn't succeed
	if (stdlib_ring_reserve(pstContainer, szNewCount) == false)
	{
		return 0;
	}

	for (i = szOldCount; !std_linear_series_done(pstSeries); std_linear_series_next(pstSeries), i++)
	{
		pvItem = stdlib_ring_at(pstContainer, i);
		stdlib_container_relocate_items(pstContainer, pvItem, pstSeries->pvData, 1);
	}

	// Update the number of items in the container
	pstContainer->szNumItems = szNewCount;

	// Return the number of items successfully pushed onto the container
	return szNumItems;
}

/**
 * Pop a series of items from the very front of a ring
 *
 * @param[in]	pstContainer	Ring to pop a series of items from
 * @param[out]	pvResult		Where to pop a series of items to (can be NULL)
 * @param[in]	szMaxItems		Maximum number of items that can be popped
 *
 * @return	Number of items popped
 */
size_t stdlib_ring_pop_front(std_container_t* pstContainer, void* pvResult, size_t szMaxItems)
{
	std_ring_t* pstRing = CONTAINER_TO_RING(pstContainer);
	void* pvItem;
	size_t i;

	if (szMaxItems > pstContainer->szNumItems)
	{
		szMaxItems = pstContainer->szNumItems;
	}

	for (i = 0; i < szMaxItems; i++)
	{
		pvItem = stdlib_ring_at(pstContainer, i);
		stdlib_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstRing->stContainer.szSizeofItem);
		if (pvResult)
		{
			pvResult = STD_LINEAR_ADD(pvResult, pstContainer->szSizeofItem);
		}
	}

	pstRing->szStartOffset   += szMaxItems;
	pstContainer->szNumItems -= szMaxItems;

	return szMaxItems;
}

/**
 * Pop a series of items from the very back of a ring
 *
 * @param[in]	pstContainer	Ring to pop a series of items from
 * @param[out]	pvResult		Where to pop a series of items to (can be NULL)
 * @param[in]	szMaxItems		Maximum number of items that can be popped
 *
 * @return	Number of items popped
 */
size_t stdlib_ring_pop_back(std_container_t * pstContainer, void * pvResult, size_t szMaxItems)
{
	std_ring_t * pstRing = CONTAINER_TO_RING(pstContainer);
	void * pvItem;
	size_t i;

	if (szMaxItems > pstContainer->szNumItems)
	{
		szMaxItems = pstContainer->szNumItems;
	}

	for (i = 0; i < szMaxItems; i++)
	{
		pvItem = stdlib_ring_at(pstContainer, pstContainer->szNumItems - 1U - i);
		stdlib_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstRing->stContainer.szSizeofItem);
		if (pvResult)
		{
			pvResult = STD_LINEAR_ADD(pvResult, pstContainer->szSizeofItem);
		}
	}
	pstContainer->szNumItems -= szMaxItems;

	return szMaxItems;
}

/**
 * Step a ring iterator forwards in memory
 *
 * @param[in]	pstIterator		Ring iterator
 */
void stdlib_ring_iterator_next(std_iterator_t * pstIterator)
{
	if (pstIterator->pvNext == pstIterator->pvEnd)
	{
		pstIterator->bDone = true;
	}
	else
	{
		pstIterator->pvRef = pstIterator->pvNext;
		pstIterator->pvNext = next_item(pstIterator, pstIterator->pvRef);
    }
}

/**
 * Step a ring iterator backwards in memory
 *
 * @param[in]	pstIterator		Ring iterator
 */
void stdlib_ring_iterator_prev(std_iterator_t * pstIterator)
{
	if (pstIterator->pvNext == pstIterator->pvEnd)
	{
		pstIterator->bDone = true;
	}
	else
	{
		pstIterator->pvRef = pstIterator->pvNext;
		pstIterator->pvNext = prev_item(pstIterator, pstIterator->pvRef);
    }
}

void stdlib_ring_forwarditerator_range(std_container_t * pstContainer, std_iterator_t * pstIterator, void *pvBegin, void * pvEnd)
{
	std_ring_t * pstRing = CONTAINER_TO_RING(pstContainer);
	std_ring_iterator_t * pstRingIterator = ITERATOR_TO_RINGIT(pstIterator);
	if (pvBegin == pvEnd)
	{
		stdlib_iterator_construct_done(pstIterator);
	}
	else
	{
		stdlib_iterator_construct(pstIterator, pstContainer, pvBegin);
		pstRingIterator->pvRingStart = pstRing->pvStartAddr;
		pstRingIterator->pvRingEnd = STD_LINEAR_ADD(pstRing->pvStartAddr, pstRing->szNumAlloced * pstContainer->szSizeofItem);
		pstIterator->pvBegin	= pvBegin;
		pstIterator->pvEnd		= pvEnd;
		pstIterator->pvNext		= next_item(pstIterator, pvBegin);
	}
}

void stdlib_ring_forwarditerator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	std_ring_t* pstRing = CONTAINER_TO_RING(pstContainer);
	void * pvBegin = pstRing->pvStartAddr;
	void * pvEnd   = stdlib_ring_at(pstContainer, pstContainer->szNumItems);
	stdlib_ring_forwarditerator_range( pstContainer, pstIterator, pvBegin, pvEnd);
}

void stdlib_ring_reverseiterator_range(std_container_t * pstContainer, std_iterator_t * pstIterator, void *pvBegin, void * pvEnd)
{
	std_ring_t* pstRing = CONTAINER_TO_RING(pstContainer);
	std_ring_iterator_t* pstRingIterator = ITERATOR_TO_RINGIT(pstIterator);
	if (pvBegin == pvEnd)
	{
		stdlib_iterator_construct_done(pstIterator);
	}
	else
	{
		stdlib_iterator_construct(pstIterator, pstContainer, pvBegin);
		pstRingIterator->pvRingStart = pstRing->pvStartAddr;
		pstRingIterator->pvRingEnd = STD_LINEAR_ADD(pstRing->pvStartAddr, pstRing->szNumAlloced * pstContainer->szSizeofItem);
		pstIterator->pvBegin	= pvBegin;
		pstIterator->pvEnd		= pvEnd;
		pstIterator->pvNext		= prev_item(pstIterator, pvBegin);
	}
}

void stdlib_ring_reverseiterator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	std_ring_t* pstRing = CONTAINER_TO_RING(pstContainer);
	void * pvBegin = stdlib_ring_at(pstContainer, pstContainer->szNumItems - 1U);
	void * pvEnd   = stdlib_ring_at(pstContainer, pstRing->szNumAlloced - 1U);
	stdlib_ring_reverseiterator_range( pstContainer, pstIterator, pvBegin, pvEnd);
}

// -------------------------------------------------------------------------

static bool ring_default_destruct(const std_item_handler_t* pstItemHandler, void* pvData)
{
	if (pstItemHandler) { /* Unused parameter */ }
	return stdlib_ring_destruct((std_container_t *) pvData);
}

typedef std_ring(int) ring_int_t;

const std_item_handler_t std_ring_default_itemhandler =
{
	.szElementSize = sizeof(ring_int_t),
	.pfn_Destructor = &ring_default_destruct,
	.pfn_Relocator = NULL
};
