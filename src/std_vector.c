/*
 * src/vector.c

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
#include "std/vector.h"

#define CONTAINER_TO_VECTOR(CONTAINER)	STD_CONTAINER_OF(CONTAINER, std_vector_t, stContainer)
#define VECTOR_TO_CONTAINER(VECTOR)		&VECTOR->stContainer

#define ITERATOR_TO_VECTORIT(IT)		STD_CONTAINER_OF(IT, std_vector_iterator_t, stIterator)
#define VECTORIT_TO_ITERATOR(VECTORIT)	&VECTORIT->stIterator

/**
 * Step a linear pointer forwards by the size of an iterator's item
 * 
 * @param[in]	pstIterator		Iterator
 * @param[in]	pvThis			Pointer
 * 
 * @return New pointer value
 */
static inline void * next_item(std_iterator_t * pstIterator, void * pvThis)
{
	return STD_LINEAR_ADD(pvThis, pstIterator->szSizeofItem);
}

/**
 * Step a linear pointer backwards by the size of an iterator's item
 *
 * @param[in]	pstIterator		Iterator
 * @param[in]	pvThis			Pointer
 *
 * @return New pointer value
 */
static inline void * prev_item(std_iterator_t * pstIterator, void * pvThis)
{
	return STD_LINEAR_SUB(pvThis, pstIterator->szSizeofItem);
}

/**
 * Calculate the address of an indexed entry in a vector container
 *
 * @param[in]	pstContainer	Vector container
 * @param[in]	szIndex			Index
 *
 * @return Address of the indexed entry
 */
inline void * stdlib_vector_at(std_container_t * pstContainer, size_t szIndex)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	return STD_LINEAR_ADD(pstVector->pvStartAddr, pstVector->stContainer.szSizeofItem * szIndex);
}

// Force the compiler to emit a non-inline version
void* stdlib_vector_at(std_container_t* pstContainer, size_t szIndex);

/**
 * Construct a vector container
 *
 * @param[in]	pstContainer	Vector to initialise
 * @param[in]	szSizeof		Size of an unwrapped item
 * @param[in]	szWrappedSizeof	Size of a wrapped item
 * @param[in]	szPayloadOffset	Offset to the payload within the wrapped item
 * @param[in]	eHas			Enumeration saying which handlers to expect
 */
void stdlib_vector_construct(std_container_t* pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas)
{
	if (szWrappedSizeof || szPayloadOffset) { /* Unused parameters */ }
	std_container_constructor(pstContainer, szSizeof, eHas);
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	pstVector->szNumAlloced	= 0;
	pstVector->pvStartAddr	= NULL;
}

/**
 * Destruct a vector container
 *
 * @param[in]	pstContainer	Vector container to destruct
 * 
 * @return True if destruction was successful, else false
 */
bool stdlib_vector_destruct(std_container_t * pstContainer)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
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
			void * pvItem = stdlib_vector_at(pstContainer, i);
			(*pstContainer->pstItemHandler->pfn_Destructor)(pstContainer->pstItemHandler, pvItem);
		}
	}

	// Free the memory allocated for this vector
	std_memoryhandler_free(pstContainer->pstMemoryHandler, pstContainer->eHas, pstVector->pvStartAddr);

	// Clear out all the fields (just to be tidy)
	pstContainer->szNumItems	= 0;
	pstVector->szNumAlloced		= 0;
	pstVector->pvStartAddr		= NULL;

	return true;
}

/**
 * Reserve space within a vector container for a number of items
 *
 * @param[in]	pstContainer	Vector container to reserve space for
 * @param[in]	szNewSize		New number of items
 * 
 * @return True if was able to reserve the memory OK, else false
 */
bool stdlib_vector_reserve(std_container_t * pstContainer, size_t szNewSize)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	size_t szNewCapacity;
	void * pvNewStart;

	if (szNewSize > pstVector->szNumAlloced)
	{
		szNewCapacity = 1ULL << (64U - __builtin_clzll(szNewSize));
		if (szNewCapacity < szNewSize)
		{
			szNewCapacity <<= 1;
		}

		if (szNewCapacity != pstVector->szNumAlloced)
		{
			pstVector->szNumAlloced = szNewCapacity;
			size_t szTotalSize = pstVector->szNumAlloced * pstVector->stContainer.szSizeofItem;
			pvNewStart = std_memoryhandler_realloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstVector->pvStartAddr, szTotalSize);
			if (pvNewStart == NULL)
			{
				return false;
			}
			if (pstContainer->eHas & std_container_has_itemhandler)
			{
				stdlib_item_relocate(pstContainer->pstItemHandler, pvNewStart, pstVector->pvStartAddr, szTotalSize);
			}
			pstVector->pvStartAddr = pvNewStart;
		}
	}

	return true;
}

/**
 * Push a series of items onto the front of a vector
 *
 * @param[in]	pstContainer	Vector container to push the series of items onto
 * @param[in]	pstSeries		Linear series of items
 *
 * @return Number of items pushed onto the vector
 */
size_t stdlib_vector_push_front(std_container_t * pstContainer, const std_linear_series_t* pstSeries)
{
	size_t szSizeofItem = pstContainer->szSizeofItem;
	size_t szOldCount = pstContainer->szNumItems;
	size_t szNumItems = pstSeries->szNumItems;
	size_t szNewCount = szOldCount + szNumItems;
	std_linear_series_iterator_t stIt;
	void * pvStart;
	void * pvItem;

	if ((szNumItems == 0) || (pstSeries->pvStart == NULL))
	{
		return 0;
	}

	// Try to reserve space, exit early if that didn't succeed
	if (stdlib_vector_reserve(pstContainer, szNewCount) == false)
	{
		return 0;
	}

	if (szOldCount != 0U)
	{
		pvStart = stdlib_vector_at(pstContainer, 0);
		pvItem  = stdlib_vector_at(pstContainer, szNumItems);
		stdlib_container_relocate_items(pstContainer, pvItem, pvStart, szOldCount);
	}

	pvItem = stdlib_vector_at(pstContainer, szNumItems - 1U);
	std_linear_series_iterator_construct(&stIt, pstSeries);
	for (; !std_linear_series_iterator_done(&stIt); std_linear_series_iterator_next(&stIt), pvItem = STD_LINEAR_SUB(pvItem, szSizeofItem))
	{
		stdlib_container_relocate_items(pstContainer, pvItem, stIt.pvData, 1);
	}

	// Update the number of items
	pstContainer->szNumItems = szNewCount;

	// Return the number of items successfully pushed onto the container
	return szNumItems;
}

/**
 * Push a series of items onto the back of a vector
 *
 * @param[in]	pstContainer	Vector container to push the series of items onto
 * @param[in]	pstSeries		Linear series of items
 *
 * @return Number of items pushed onto the vector
 */
size_t stdlib_vector_push_back(std_container_t * pstContainer, const std_linear_series_t* pstSeries)
{
	size_t szSizeofItem = pstContainer->szSizeofItem;
	size_t szOldCount = pstContainer->szNumItems;
	size_t szNumItems = pstSeries->szNumItems;
	size_t szNewCount = szOldCount + szNumItems;
	std_linear_series_iterator_t stIt;
	void * pvItem;

	if ((szNumItems == 0) || (pstSeries->pvStart == NULL))
	{
		return 0;
	}

	// Try to reserve space, exit early if that didn't succeed
	if (stdlib_vector_reserve(pstContainer, szNewCount) == false)
	{
		return 0;
	}

	pvItem = stdlib_vector_at(pstContainer, szOldCount);
	std_linear_series_iterator_construct(&stIt, pstSeries);
	for (; !std_linear_series_iterator_done(&stIt); std_linear_series_iterator_next(&stIt), pvItem=STD_LINEAR_ADD(pvItem,szSizeofItem))
	{
		stdlib_container_relocate_items(pstContainer, pvItem, stIt.pvData, 1);
	}

	// Update the number of items in the container
	pstContainer->szNumItems = szNewCount;

	// Return the number of items successfully pushed onto the container
	return szNumItems;
}

/**
 * Pop a series of items from the very front of a vector
 *
 * @param[in]	pstContainer	Vector to pop a series of items from
 * @param[out]	pvResult		Where to pop a series of items to (can be NULL)
 * @param[in]	szMaxItems		Maximum number of items that can be popped
 *
 * @return	Number of items popped
 */
size_t stdlib_vector_pop_front(std_container_t* pstContainer, void* pvResult, size_t szMaxItems)
{
	std_vector_t* pstVector = CONTAINER_TO_VECTOR(pstContainer);
	void* pvBase;
	void* pvItem;
	size_t i;

	if (szMaxItems > pstContainer->szNumItems)
	{
		szMaxItems = pstContainer->szNumItems;
	}

	for (i = 0; i < szMaxItems; i++)
	{
		pvItem = stdlib_vector_at(pstContainer, i);
		stdlib_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstVector->stContainer.szSizeofItem);
		if (pvResult)
		{
			pvResult = STD_LINEAR_ADD(pvResult, pstContainer->szSizeofItem);
		}
	}

	if (pstContainer->szNumItems > szMaxItems)
	{
		pvBase = stdlib_vector_at(pstContainer, 0);
		pvItem = stdlib_vector_at(pstContainer, szMaxItems);
		stdlib_container_relocate_items(pstContainer, pvItem, pvBase, pstContainer->szNumItems - szMaxItems);
	}
	pstContainer->szNumItems -= szMaxItems;

	return szMaxItems;
}

/**
 * Pop a series of items from the very back of a vector
 *
 * @param[in]	pstContainer	Vector to pop a series of items from
 * @param[out]	pvResult		Where to pop a series of items to (can be NULL)
 * @param[in]	szMaxItems		Maximum number of items that can be popped
 *
 * @return	Number of items popped
 */
size_t stdlib_vector_pop_back(	std_container_t * pstContainer, void * pvResult, size_t szMaxItems)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	void * pvItem;
	size_t i;

	if (szMaxItems > pstContainer->szNumItems)
	{
		szMaxItems = pstContainer->szNumItems;
	}

	for (i = 0; i < szMaxItems; i++)
	{
		pvItem = stdlib_vector_at(pstContainer, pstContainer->szNumItems - 1U - i);
		stdlib_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstVector->stContainer.szSizeofItem);
		if (pvResult)
		{
			pvResult = STD_LINEAR_ADD(pvResult, pstContainer->szSizeofItem);
		}
	}
	pstContainer->szNumItems -= szMaxItems;

	return szMaxItems;
}

/**
 * Sort a range of items within a vector
 *
 * @param[in]	pstContainer	Vector to sort
 * @param[in]	szFirst			Index of first element in range to sort
 * @param[in]	szLast			Index of last  element in range to sort
 * @param[in]	pfnCompare		Comparison function to use when sorting
 */
void stdlib_vector_ranged_sort(std_container_t * pstContainer, size_t szFirst, size_t szLast, pfn_std_compare_t pfnCompare)
{
	if (	(pstContainer->eHas & std_container_has_itemhandler)
		&&	(pstContainer->pstItemHandler->pfn_Relocator != NULL)	)
	{
		// FIXME: this code will need a completely different approach if the vector's allocator has a relocator function!
	}
	else
	{
		qsort(	stdlib_vector_at(pstContainer, szFirst),	// Start of array
				szLast + 1U - szFirst,						// Number of items being sorted
				pstContainer->szSizeofItem,					// Size of each item
				pfnCompare	);								// Comparison function
	}
}

/**
 *
 */
void stdlib_vector_forwarditerator_range(std_container_t * pstContainer, std_iterator_t * pstIterator, void *pvBegin, void * pvEnd)
{
	if (pvBegin == pvEnd)
	{
		stdlib_iterator_construct_done(pstIterator);
	}
	else
	{
		stdlib_iterator_construct(pstIterator, pstContainer, pvBegin);
		pstIterator->pvBegin	= pvBegin;
		pstIterator->pvEnd		= pvEnd;
		pstIterator->pvNext		= next_item(pstIterator, pvBegin);
	}
}

/**
 *
 */
void stdlib_vector_forwarditerator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	std_vector_t* pstVector = CONTAINER_TO_VECTOR(pstContainer);
	void * pvBegin = pstVector->pvStartAddr;
	void * pvEnd   = stdlib_vector_at(pstContainer, pstContainer->szNumItems);
	stdlib_vector_forwarditerator_range( pstContainer, pstIterator, pvBegin, pvEnd);
}

/**
 *
 */
void stdlib_vector_reverseiterator_range(std_container_t * pstContainer, std_iterator_t * pstIterator, void *pvBegin, void * pvEnd)
{
	if (pvBegin == pvEnd)
	{
		stdlib_iterator_construct_done(pstIterator);
	}
	else
	{
		stdlib_iterator_construct(pstIterator, pstContainer, pvBegin);
		pstIterator->pvBegin	= pvBegin;
		pstIterator->pvEnd		= pvEnd;
		pstIterator->pvNext		= prev_item(pstIterator, pvBegin);
	}
}

/**
 *
 */
void stdlib_vector_reverseiterator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	std_vector_t* pstVector = CONTAINER_TO_VECTOR(pstContainer);
	void * pvBegin = stdlib_vector_at(pstContainer, pstContainer->szNumItems - 1U);
	void * pvEnd = STD_LINEAR_SUB(pstVector->pvStartAddr, pstContainer->szSizeofItem);
	stdlib_vector_reverseiterator_range( pstContainer, pstIterator, pvBegin, pvEnd );
}

// -------------------------------------------------------------------------

/**
 * Find the ordered slot where a new item should be inserted
 *
 * @param[in]	pstContainer		Vector container to search
 * @param[in]	pfnCompare			Comparison callback
 * @param[in]	pvBase				New item to compare existing items with
 *
 * @return Index in the container where new item should be inserted
 */
static size_t stdlib_vector_find_entry(std_container_t* pstContainer, pfn_std_compare_t pfnCompare, const void* pvBase)
{
	std_vector_t* pstVector = CONTAINER_TO_VECTOR(pstContainer);
	size_t szSizeofItem = pstContainer->szSizeofItem;
	size_t szNumItems = pstContainer->szNumItems;
	size_t i;
	void* pvItem;

	// Simple (non-binary-chopping) initial version to test all the interfaces and to write tests
	pvItem = pstVector->pvStartAddr;
	for (i = 0; i < szNumItems; i++, pvItem = STD_LINEAR_ADD(pvItem, szSizeofItem))
	{
		if ((*pfnCompare)(pvItem, pvBase) == false)
			break;
	}

	return i;
}

/**
 * Insert a series of items into an ordered vector (heap) 
 *
 * @param[in]	pstContainer	Vector container to insert the series of items onto
 * @param[in]	pstSeries		Linear series of items to insert into the container
 * @param[in]	pfnCompare		Comparison function callback
 *
 * @return Number of items inserted into the vector container
 */
size_t stdlib_vector_heap_insert(std_container_t* pstContainer, const std_linear_series_t* pstSeries, pfn_std_compare_t pfnCompare)
{
	size_t szSizeofItem = pstContainer->szSizeofItem;
	size_t szOldCount = pstContainer->szNumItems;
	size_t szNumItems = pstSeries->szNumItems;
	size_t szNewCount = szOldCount + szNumItems;
	std_linear_series_iterator_t stIt;
	void * pvItem;
	void * pvNext;
	size_t szIndex;
	size_t i;

	if ((szNumItems == 0) || (pstSeries->pvStart == NULL))
	{
		return 0;
	}

	// Try to reserve space, exit early if that didn't succeed
	if (stdlib_vector_reserve(pstContainer, szNewCount) == false)
	{
		return 0;
	}

	std_linear_series_iterator_construct(&stIt, pstSeries);
	for (i = 0; !std_linear_series_iterator_done(&stIt); std_linear_series_iterator_next(&stIt), pvItem = STD_LINEAR_ADD(pvItem, szSizeofItem), i++)
	{
		// Binary chop to find the highest entry less than the new item
		szIndex = stdlib_vector_find_entry(pstContainer, pfnCompare, stIt.pvData);
		pvItem = stdlib_vector_at(pstContainer, szIndex);

		// If this isn't the entry just past the final entry in the table, ripple all the items above it up by one
		if (szIndex < szOldCount + i)
		{
			pvNext = STD_LINEAR_ADD(pvItem, szSizeofItem);
			stdlib_container_relocate_items(pstContainer, pvNext, pvItem, szOldCount + i - szIndex);
		}
		stdlib_container_relocate_items(pstContainer, pvItem, stIt.pvData, 1);

		// Update the number of items in the container
		pstContainer->szNumItems++;
	}

	// Return the number of items successfully pushed onto the container
	return i;
}

// -------------------------------------------------------------------------

/**
 *
 */
static bool vector_default_destruct(const std_item_handler_t* pstItemHandler, void* pvData)
{
	if (pstItemHandler) { /* Unused parameter */ }
	return stdlib_vector_destruct((std_container_t *) pvData);
}

typedef std_vector(int) vector_int_t;

const std_item_handler_t std_vector_default_itemhandler =
{
	.szElementSize = sizeof(vector_int_t),
	.pfn_Destructor = &vector_default_destruct,
	.pfn_Relocator = NULL
};
