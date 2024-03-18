/*
 * std_deque.c
 *
 *  Created on: 18 Oct 2022
 *      Author: Nick Pelling
 *
 * The central idea of a deque is to make it a container that keeps nearly
 * all the speed advantages of a linear vector (e.g. when iterating through
 * the contains of an individual bucket) but adds the flexibility of a queue
 * (e.g. when pushing/popping items at the start or end).
 *
 * It achieves this by allocating blocks of items known as "buckets", and
 * iterating by stepping linearly through the items in each bucket.
 */

#include <string.h>		// for memcpy/memmove

#include "std/deque.h"

#define DEFAULT_BUCKET_SIZE		256

#define CONTAINER_TO_DEQUE(CONTAINER)	STD_CONTAINER_OF(CONTAINER, std_deque_t, stContainer)
#define DEQUE_TO_CONTAINER(DEQUE)		&DEQUE->stContainer

#define ITERATOR_TO_DEQUEIT(IT)			STD_CONTAINER_OF(IT, std_deque_iterator_t, stIterator)
#define DEQUEIT_TO_ITERATOR(DEQUEIT)	&DEQUEIT->stIterator

static inline void * next_item(std_iterator_t * pstIterator, void * pvThis)
{
	return STD_LINEAR_ADD(pvThis, pstIterator->szSizeofItem);
}

static inline void * prev_item(std_iterator_t * pstIterator, void * pvThis)
{
	return STD_LINEAR_SUB(pvThis, pstIterator->szSizeofItem);
}

inline void * stdlib_deque_front(std_container_t * pstContainer)
{
    return stdlib_deque_at(pstContainer, 0);
}

inline void * stdlib_deque_back(std_container_t * pstContainer)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
    return stdlib_deque_at(pstContainer, pstDeque->szNumItems - 1U);
}

inline void * stdlib_deque_begin(std_container_t * pstContainer)
{
    return stdlib_deque_front(pstContainer);
}

inline void * stdlib_deque_end(std_container_t * pstContainer)
{
    if (pstContainer) { /* lint - parameter not used */ }

    return NULL;
}

inline void * stdlib_deque_rbegin(std_container_t * pstContainer)
{
    return stdlib_deque_back(pstContainer);
}

inline void * stdlib_deque_rend(std_container_t * pstContainer)
{
    if (pstContainer) { /* lint - parameter not used */ }

    return NULL;
}

inline bool stdlib_deque_empty(std_container_t * pstContainer)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
    return (pstDeque->szNumItems == 0);
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
 * Retrieve a pointer to an item from inside a bucket of items
 */
static inline void * bucket_at(void * pvBucket, size_t szIndex, size_t szSizeofItem)
{
	return STD_LINEAR_ADD(pvBucket, szIndex * szSizeofItem);
}

/**
 * Calculate the number of buckets that would be needed if extra items were added
 */
static inline size_t bucket_count(std_deque_t * pstDeque, size_t szExtraItems)
{
	return (pstDeque->szStartOffset + pstDeque->szNumItems + szExtraItems) / pstDeque->szItemsPerBucket;
}

/**
 * Insert a new bucket of items at the start of the deque
 */
static inline void * bucket_insert_at_start(std_deque_t * pstDeque)
{
	size_t szNumBuckets;
	size_t szSize;
	void * pvBucket;

	szNumBuckets = ++(pstDeque->szNumBuckets);
	szSize = szNumBuckets * pstDeque->stContainer.szSizeofItem;

	if (szNumBuckets == 1U)
	{
		pstDeque->papvBuckets = (void * *) std_memoryhandler_malloc(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, szSize);
	}
	else
	{
		pstDeque->papvBuckets = (void * *) std_memoryhandler_realloc(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pstDeque->papvBuckets, szSize);
		memmove(pstDeque->papvBuckets, &pstDeque->papvBuckets[1], (szNumBuckets - 1U) * sizeof(pstDeque->papvBuckets[0]));
	}

	pvBucket = bucket_alloc(pstDeque);
	pstDeque->papvBuckets[0] = pvBucket;

	return pvBucket;
}

/**
 * Append a new bucket of items to the end of the deque's set of buckets
 */
static inline void * bucket_append_to_end(std_deque_t * pstDeque)
{
	size_t szNumBuckets;
	void * pvBucket;
	size_t szSize;

	szNumBuckets = ++(pstDeque->szNumBuckets);
	szSize = szNumBuckets * pstDeque->stContainer.szSizeofItem;
	if (szNumBuckets == 1U)
	{
		pstDeque->papvBuckets = (void * *) std_memoryhandler_malloc(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, szSize);
	}
	else
	{
		pstDeque->papvBuckets = (void * *) std_memoryhandler_realloc(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pstDeque->papvBuckets, szSize);
	}

	pvBucket = bucket_alloc(pstDeque);
	pstDeque->papvBuckets[szNumBuckets - 1U] = pvBucket;

	return pvBucket;
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
bool stdlib_deque_construct(std_container_t * pstContainer, size_t szFullSizeof, size_t szPayloadOffset, std_container_has_t eHas, const std_container_handlers_t * pstHandlers)
{
	bool bResult = std_container_constructor(pstContainer, szFullSizeof - szPayloadOffset, eHas, pstHandlers);
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	pstDeque->szItemsPerBucket	= DEFAULT_BUCKET_SIZE;
	pstDeque->papvBuckets		= NULL;
	pstDeque->szNumBuckets		= 0;
	pstDeque->szNumItems		= 0U;
	pstDeque->szStartOffset		= 0U;
	return bResult;
}

bool stdlib_deque_destruct(std_container_t* pstContainer)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	if (pstContainer == NULL)
	{
		return false;
	}

	while (pstDeque->szNumItems != 0)
	{
		stdlib_deque_pop_front(pstContainer, NULL);
	}

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
void * stdlib_deque_at(std_container_t * pstContainer, int32_t iIndex)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	void * pvBucket;
	size_t szQuotient;
	size_t szRemainder;

	if (iIndex >= pstDeque->szNumItems)
	{
		return NULL;
	}

	iIndex += pstDeque->szStartOffset;
	szQuotient  = iIndex / pstDeque->szItemsPerBucket;
	szRemainder = iIndex % pstDeque->szItemsPerBucket;
	pvBucket = pstDeque->papvBuckets[szQuotient];
	return bucket_at(pvBucket, szRemainder, pstContainer->szSizeofItem);
}

/**
 *
 */
void stdlib_deque_next(std_iterator_t * pstIterator)
{
	std_deque_iterator_t * pstDequeIt = ITERATOR_TO_DEQUEIT(pstIterator);
	std_deque_t* pstDeque = pstDequeIt->pstDeque;
	std_container_t* pstContainer = DEQUE_TO_CONTAINER(pstDeque);

	if (++pstDequeIt->szIndex < pstDequeIt->szRangeLen)
	{
		pstIterator->pvRef = stdlib_deque_at(pstContainer, pstDequeIt->szIndex);
	}
	else
	{
    	pstDequeIt->stIterator.bDone = true;
    }
}

/**
 *
 */
void stdlib_deque_prev(std_iterator_t * pstIterator)
{
	std_deque_iterator_t* pstDequeIt = ITERATOR_TO_DEQUEIT(pstIterator);
	std_deque_t* pstDeque = pstDequeIt->pstDeque;
	std_container_t* pstContainer = DEQUE_TO_CONTAINER(pstDeque);

	if (pstDequeIt->szIndex-- != 0U)
	{
		pstIterator->pvRef = stdlib_deque_at(pstContainer, pstDequeIt->szIndex);
	}
	else
	{
		pstDequeIt->stIterator.bDone = true;
    }
}

/**
 *
 */
void stdlib_deque_forwarditerator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	std_deque_iterator_t * pstDequeIt = ITERATOR_TO_DEQUEIT(pstIterator);

	if (pstDeque->szNumItems == 0)
	{
    	pstDequeIt->stIterator.bDone = true;
	}
	else
	{
		pstDequeIt->pstDeque = pstDeque;

		pstDequeIt->stIterator.szSizeofItem	= pstContainer->szSizeofItem;
		pstDequeIt->stIterator.pvRef		= stdlib_deque_at(pstContainer, 0);
		pstDequeIt->stIterator.bDone = false;
		pstDequeIt->szIndex			= 0U;
		pstDequeIt->szRangeLen		= pstDeque->szNumItems;
	}
}

/**
 *
 */
void stdlib_deque_reverseiterator_construct(std_container_t* pstContainer, std_iterator_t* pstIterator)
{
	std_deque_t* pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	std_deque_iterator_t* pstDequeIt = ITERATOR_TO_DEQUEIT(pstIterator);

	if (pstDeque->szNumItems == 0)
	{
		pstDequeIt->stIterator.bDone = true;
	}
	else
	{
		pstDequeIt->pstDeque = pstDeque;

		pstDequeIt->stIterator.szSizeofItem = pstContainer->szSizeofItem;
		pstDequeIt->stIterator.pvRef = stdlib_deque_at(pstContainer, pstDeque->szNumItems - 1U);
		pstDequeIt->stIterator.bDone = false;
		pstDequeIt->szIndex = pstDeque->szNumItems - 1U;
		pstDequeIt->szRangeLen = pstDeque->szNumItems;
	}
}

/**
 *
 */
void * stdlib_deque_push_front(std_container_t * pstContainer)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	void * pvItem;

    if (pstDeque->szStartOffset == 0)
    {
    	bucket_insert_at_start(pstDeque);
    	pstDeque->szStartOffset = pstDeque->szItemsPerBucket - 1;
    }
	else
	{
		pstDeque->szStartOffset--;
	}
	pstDeque->szNumItems++;

	pvItem = stdlib_deque_at(pstContainer, 0);
	if (pstContainer->eHas & std_container_has_itemhandler)
	{
		std_item_construct(pstContainer->pstItemHandler, pvItem, 1U);
	}

	return pvItem;
}

/**
 *
 */
void * stdlib_deque_push_back(std_container_t * pstContainer)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	void * pvItem;

	pstDeque->szNumItems++;
    if ((pstDeque->szStartOffset + pstDeque->szNumItems) > (pstDeque->szNumBuckets * pstDeque->szItemsPerBucket))
    {
    	bucket_append_to_end(pstDeque);
    }

	pvItem = stdlib_deque_at(pstContainer, pstDeque->szNumItems - 1U);
	if (pstContainer->eHas & std_container_has_itemhandler)
	{
		std_item_construct(pstContainer->pstItemHandler, pvItem, 1U);
	}

	return pvItem;
}

/**
 *
 */
void * stdlib_deque_pop_front(std_container_t * pstContainer, void * pvResult)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	void * pvItem;

	// Get the address of the first item in the deque
	pvItem = stdlib_deque_front(pstContainer);
	std_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstContainer->szSizeofItem);
	pstDeque->szStartOffset++;
	pstDeque->szNumItems--;

	return pvResult;
}

/**
 *
 */
void * stdlib_deque_pop_back(std_container_t * pstContainer, void * pvResult)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	void * pvItem;

	// Get the address of the final item in the deque
	pvItem = stdlib_deque_back(pstContainer);
	std_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstContainer->szSizeofItem);
	pstDeque->szNumItems--;

	return pvResult;
}

