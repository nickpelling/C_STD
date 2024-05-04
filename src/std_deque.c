/*
 * src/std_deque.c

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

/*
 * The central idea of a deque is to make a container that is largely linear
 * but that is able to push/pop items at the start or end.
 *
 * It achieves this by allocating blocks of items known as "buckets", and
 * iterating by stepping linearly through the items in each bucket.
 */

#include <string.h>		// for memcpy/memmove

#include "std/deque.h"

#define DEFAULT_BUCKET_SIZE		256

/*
 * Step a pointer linearly forwards within a bucket of items
 * 
 * @param[in]	pstIterator		The iterator context
 * @param[in]	pvThis			The current pointer
 * 
 * @return Pointer to the start of the next linearly sequential item
 */
static inline void * next_item(std_iterator_t * pstIterator, void * pvThis)
{
	return STD_LINEAR_ADD(pvThis, pstIterator->szSizeofItem);
}

/*
 * Step a pointer linearly backwards within a bucket of items
 *
 * @param[in]	pstIterator		The iterator context
 * @param[in]	pvThis			The current pointer
 *
 * @return Pointer to the start of the previous linearly sequential item
 */
static inline void * prev_item(std_iterator_t * pstIterator, void * pvThis)
{
	return STD_LINEAR_SUB(pvThis, pstIterator->szSizeofItem);
}

// --------------------------------------------------------------------------
// Private helper functions
// --------------------------------------------------------------------------

/**
 * Grab memory for a bucket of items (but don't initialise them)
 */
static inline void * bucket_alloc(const std_deque_t * pstDeque)
{
	size_t szTotalSize = pstDeque->stContainer.szSizeofItem * pstDeque->szItemsPerBucket;
	return std_memoryhandler_malloc(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, szTotalSize);
}

/**
 * Release memory for a bucket of items
 */
static inline void bucket_free(const std_deque_t* pstDeque, void * pvBucket)
{
	std_memoryhandler_free(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pvBucket);
}

#if 0
/**
 * Retrieve a pointer to an item from inside a bucket of items
 */
static inline void * bucket_at(void * pvBucket, size_t szIndex, size_t szSizeofItem)
{
	return STD_LINEAR_ADD(pvBucket, szIndex * szSizeofItem);
}
#endif

/**
 * Insert a new bucket of items at the start of the deque
 */
static bool bucket_insert_at_start(std_deque_t * pstDeque)
{
	size_t szNumBuckets;
	size_t szSize;
	void * * papvBuckets;
	void * pvBucket;

	pvBucket = bucket_alloc(pstDeque);
	if (pvBucket == NULL)
	{
		return false;
	}

	szNumBuckets = pstDeque->szNumBuckets + 1U;
	szSize = szNumBuckets * sizeof(papvBuckets[0]);
	papvBuckets = (void**)std_memoryhandler_realloc(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pstDeque->papvBuckets, szSize);
	if (papvBuckets == NULL)
	{
		std_memoryhandler_free(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pvBucket);
		return false;
	}

	memmove(papvBuckets, &papvBuckets[1], (szNumBuckets - 1U) * sizeof(papvBuckets[0]));
	papvBuckets[0] = pvBucket;

	pstDeque->papvBuckets = papvBuckets;
	pstDeque->szNumBuckets = szNumBuckets;

	return true;
}

/**
 * Append a new bucket of items to the end of the deque's set of buckets
 */
static bool bucket_append_to_end(std_deque_t * pstDeque)
{
	size_t szNumBuckets;
	size_t szSize;
	void** papvBuckets;
	void* pvBucket;

	pvBucket = bucket_alloc(pstDeque);
	if (pvBucket == NULL)
	{
		return false;
	}

	szNumBuckets = pstDeque->szNumBuckets + 1U;
	szSize = szNumBuckets * sizeof(papvBuckets[0]);
	papvBuckets = (void**)std_memoryhandler_realloc(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pstDeque->papvBuckets, szSize);
	if (papvBuckets == NULL)
	{
		std_memoryhandler_free(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pvBucket);
		return false;
	}

	papvBuckets[szNumBuckets - 1U] = pvBucket;

	pstDeque->papvBuckets = papvBuckets;
	pstDeque->szNumBuckets = szNumBuckets;

	return true;
}

/**
 * Delete the very first bucket in the deque
 * 
 * @param[in]	pstDeque	Deque
 * 
 * @return True if bucket was successfully deleted, else false
 */
static bool bucket_delete_first(std_deque_t* pstDeque)
{
	size_t szNumBuckets;
	size_t szSize;
	void** papvBuckets;
	void* pvBucket;
	void* pvBucketLast;

	// Grab pointers to the first bucket and the last bucket in the deque
	pvBucket = pstDeque->papvBuckets[0];
	pvBucketLast = pstDeque->papvBuckets[pstDeque->szNumBuckets - 1U];

	// Try to reallocate the bucket pointer table
	szNumBuckets = pstDeque->szNumBuckets - 1U;
	szSize = szNumBuckets * sizeof(papvBuckets[0]);
	papvBuckets = (void**)std_memoryhandler_realloc(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pstDeque->papvBuckets, szSize);
	if ((papvBuckets == NULL) && (szSize != 0U))
	{
		return false;
	}

	// Free the bucket
	bucket_free(pstDeque, pvBucket);

	// Move any remaining bucket pointers down by one, and then reinstate the final bucket pointer
	if ((papvBuckets != NULL) && (szNumBuckets > 1U))
	{
		memmove(papvBuckets, &papvBuckets[1], sizeof(papvBuckets[0]) * (szNumBuckets - 1U));
	}
	if (papvBuckets != NULL)
	{
		papvBuckets[szNumBuckets - 1U] = pvBucketLast;
	}

	pstDeque->papvBuckets = papvBuckets;
	pstDeque->szNumBuckets = szNumBuckets;

	return true;
}

/**
 * Delete the very last bucket in the deque
 *
 * @param[in]	pstDeque	Deque
 *
 * @return True if bucket was successfully deleted, else false
 */
static bool bucket_delete_last(std_deque_t* pstDeque)
{
	size_t szNumBuckets;
	size_t szSize;
	void** papvBuckets;
	void* pvBucketLast;

	// Grab pointer to the last bucket in the deque
	pvBucketLast = pstDeque->papvBuckets[pstDeque->szNumBuckets - 1U];

	// Try to reallocate the bucket array contents
	szNumBuckets = pstDeque->szNumBuckets - 1U;
	szSize = szNumBuckets * sizeof(papvBuckets[0]);
	papvBuckets = (void**)std_memoryhandler_realloc(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pstDeque->papvBuckets, szSize);
	if ((papvBuckets == NULL) && (szSize != 0))
	{
		return false;
	}

	// Free the last bucket
	bucket_free(pstDeque, pvBucketLast);

	pstDeque->papvBuckets = papvBuckets;
	pstDeque->szNumBuckets = szNumBuckets;

	return true;
}

// --------------------------------------------------------------------------
// Public deque functions
// --------------------------------------------------------------------------

/**
 * Initialise a newly-allocated deque
 *
 * @param[in]	pstList			Deque to initialise
 * @param[in]	pstItemHandler	Deque this list should use
 * @param[in]	szSizeofItem	Size of an item
 * @param[in]	szAlignofItem	Alignment of an item
 */
void stdlib_deque_construct(std_container_t * pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas)
{
	if (szWrappedSizeof || szPayloadOffset) { /* Unused parameters */ }
	std_container_constructor(pstContainer, szSizeof, eHas);
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	pstDeque->szItemsPerBucket	= DEFAULT_BUCKET_SIZE;
	pstDeque->papvBuckets		= NULL;
	pstDeque->szNumBuckets		= 0;
	pstDeque->szStartOffset		= 0U;
}

bool stdlib_deque_destruct(std_container_t* pstContainer)
{
	if (pstContainer == NULL)
	{
		return false;
	}

	stdlib_deque_pop_front(pstContainer, NULL, pstContainer->szNumItems);

	return true;
}

void stdlib_deque_setbucketsize(std_container_t * pstContainer, size_t szBucketSize)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);

	pstDeque->szItemsPerBucket = szBucketSize;
}

/**
 *
 */
void * stdlib_deque_at(std_container_t * pstContainer, size_t szIndex)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	void * pvBucket;
	size_t szQuotient;
	size_t szRemainder;

	if (szIndex >= pstContainer->szNumItems)
	{
		return NULL;
	}

	szIndex += pstDeque->szStartOffset;
	szQuotient  = szIndex / pstDeque->szItemsPerBucket;
	szRemainder = szIndex % pstDeque->szItemsPerBucket;
	pvBucket = pstDeque->papvBuckets[szQuotient];
	return STD_LINEAR_ADD(pvBucket, szRemainder * pstContainer->szSizeofItem);
}

/**
 *
 */
void stdlib_deque_forwarditerator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator, size_t szFirst, size_t szLast)
{
	std_deque_t* pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	size_t szIndex;
	size_t szQuotient;
	size_t szRemainder;
	void * pvRef;

	if (pstContainer->szNumItems == 0)
	{
		stdlib_iterator_construct_done(pstIterator);
	}
	else
	{
		szIndex = szFirst;

		std_deque_iterator_t* pstDequeIt = ITERATOR_TO_DEQUEIT(pstIterator);
		pstDequeIt->szIndex			= szIndex;
		pstDequeIt->szRangeEnd		= szLast;

		szIndex += pstDeque->szStartOffset;
		szQuotient = szIndex / pstDeque->szItemsPerBucket;
		szRemainder = szIndex % pstDeque->szItemsPerBucket;

		pstDequeIt->pvBucketStart = pstDeque->papvBuckets[szQuotient];
		pstDequeIt->pvBucketEnd = STD_LINEAR_ADD(pstDequeIt->pvBucketStart, pstIterator->szSizeofItem * pstDeque->szItemsPerBucket);

		pvRef = STD_LINEAR_ADD(pstDequeIt->pvBucketStart, szRemainder * pstContainer->szSizeofItem);
		stdlib_iterator_construct(pstIterator, pstContainer, pvRef);
	}
}

/**
 *
 */
void stdlib_deque_reverseiterator_construct(std_container_t* pstContainer, std_iterator_t* pstIterator, size_t szFirst, size_t szLast)
{
	std_deque_iterator_t* pstDequeIt = ITERATOR_TO_DEQUEIT(pstIterator);
	std_deque_t* pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	size_t szIndex;
	size_t szQuotient;
	size_t szRemainder;
	void* pvRef;

	if (pstContainer->szNumItems == 0)
	{
		stdlib_iterator_construct_done(pstIterator);
	}
	else
	{
		szFirst = (pstContainer->szNumItems - 1U) - szFirst;
		szLast  = (pstContainer->szNumItems - 1U) - szLast;

		szIndex = szFirst;

		pstDequeIt->szIndex = szIndex;
		pstDequeIt->szRangeEnd = szLast;

		szIndex += pstDeque->szStartOffset;
		szQuotient = szIndex / pstDeque->szItemsPerBucket;
		szRemainder = szIndex % pstDeque->szItemsPerBucket;

		pstDequeIt->pvBucketStart = pstDeque->papvBuckets[szQuotient];
		pstDequeIt->pvBucketEnd = STD_LINEAR_ADD(pstDequeIt->pvBucketStart, pstIterator->szSizeofItem * pstDeque->szItemsPerBucket);

		pvRef = STD_LINEAR_ADD(pstDequeIt->pvBucketStart, szRemainder * pstContainer->szSizeofItem);
		stdlib_iterator_construct(pstIterator, pstContainer, pvRef);
	}
}

/**
 *
 */
size_t stdlib_deque_push_front(std_container_t * pstContainer, const std_linear_series_t * pstSeries)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	std_linear_series_iterator_t stIt;
	void * pvItem;
	size_t i;

	std_linear_series_iterator_construct(&stIt, pstSeries);
	for (i = 0; !std_linear_series_iterator_done(&stIt); i++, std_linear_series_iterator_next(&stIt))
	{
		if (pstDeque->szStartOffset == 0)
		{
			if (bucket_insert_at_start(pstDeque) == false)
			{
				break;
			}
			pstDeque->szStartOffset = pstDeque->szItemsPerBucket - 1;
		}
		else
		{
			pstDeque->szStartOffset--;
		}
		pstContainer->szNumItems++;

		pvItem = stdlib_deque_at(pstContainer, 0);
		stdlib_container_relocate_items(pstContainer, pvItem, stIt.pvData, 1);
	}

	return i;
}

/**
 *
 */
size_t stdlib_deque_push_back(std_container_t * pstContainer, const std_linear_series_t* pstSeries)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	std_linear_series_iterator_t stIt;
	void * pvItem;
	size_t i;

	std_linear_series_iterator_construct(&stIt, pstSeries);
	for (i = 0; !std_linear_series_iterator_done(&stIt); i++, std_linear_series_iterator_next(&stIt))
	{
		pstContainer->szNumItems++;
		if ((pstDeque->szStartOffset + pstContainer->szNumItems) > (pstDeque->szNumBuckets * pstDeque->szItemsPerBucket))
		{
			if (bucket_append_to_end(pstDeque) == false)
			{
				break;
			}
		}

		pvItem = stdlib_deque_at(pstContainer, pstContainer->szNumItems - 1U);
		stdlib_container_relocate_items(pstContainer, pvItem, stIt.pvData, 1);
	}

	return i;
}

/**
 *
 */
size_t stdlib_deque_pop_front(std_container_t * pstContainer, void * pvResult, size_t szMaxItems)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	void * pvItem;
	size_t i;

	if (szMaxItems > pstContainer->szNumItems)
	{
		szMaxItems = pstContainer->szNumItems;
	}

	for (i = 0; i < szMaxItems; i++)
	{
		// Get the address of the first item in the deque
		pvItem = stdlib_deque_at(pstContainer, 0);
		stdlib_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstContainer->szSizeofItem);
		pstDeque->szStartOffset++;
		pstContainer->szNumItems--;

		if (pstDeque->szStartOffset >= pstDeque->szItemsPerBucket)
		{
			bucket_delete_first(pstDeque);
			pstDeque->szStartOffset = 0;
		}

		if (pvResult != NULL)
		{
			pvResult = STD_LINEAR_ADD(pvResult, pstContainer->szSizeofItem);
		}
	}

	return szMaxItems;
}

/**
 *
 */
size_t stdlib_deque_pop_back(std_container_t * pstContainer, void * pvResult, size_t szMaxItems)
{
	std_deque_t* pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	void * pvItem;
	size_t i;

	if (szMaxItems > pstContainer->szNumItems)
	{
		szMaxItems = pstContainer->szNumItems;
	}

	for (i = 0; i < szMaxItems; i++)
	{
		// Get the address of the final item in the deque
		pvItem = stdlib_deque_at(pstContainer, pstContainer->szNumItems - 1U);
		stdlib_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstContainer->szSizeofItem);

		pstContainer->szNumItems--;
		if ((pstDeque->szStartOffset + pstContainer->szNumItems) % pstDeque->szItemsPerBucket == 0)
		{
			bucket_delete_last(pstDeque);
		}

		if (pvResult != NULL)
		{
			pvResult = STD_LINEAR_ADD(pvResult, pstContainer->szSizeofItem);
		}
	}

	return szMaxItems;
}

// -------------------------------------------------------------------------

static bool deque_default_destruct(const std_item_handler_t* pstItemHandler, void* pvData)
{
	if (pstItemHandler) { /* Unused parameter */ }
	return stdlib_deque_destruct((std_container_t*)pvData);
}

typedef std_deque(int) deque_int_t;

const std_item_handler_t std_deque_default_itemhandler =
{
	.szElementSize = sizeof(deque_int_t),
	.pfn_Destructor = &deque_default_destruct,
	.pfn_Relocator = NULL
};
