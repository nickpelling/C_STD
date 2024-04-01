/*
 * std_deque.c
 *
 *  Created on: 18 Oct 2022
 *      Author: Nick Pelling
 *
 * The central idea of a deque is to make a container that is largely linear
 * but that is able to push/pop items at the start or end.
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
	return (pstDeque->szStartOffset + pstDeque->stContainer.szNumItems + szExtraItems) / pstDeque->szItemsPerBucket;
}

/**
 * Insert a new bucket of items at the start of the deque
 */
static inline bool bucket_insert_at_start(std_deque_t * pstDeque)
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
	szSize = szNumBuckets * pstDeque->stContainer.szSizeofItem;
	papvBuckets = (void**)std_memoryhandler_realloc(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pstDeque->papvBuckets, szSize);
	if (papvBuckets == NULL)
	{
		std_memoryhandler_free(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pvBucket);
		return false;
	}

	memmove(papvBuckets, &papvBuckets[1], (szNumBuckets - 1U) * sizeof(papvBuckets[0]));
	pstDeque->papvBuckets = papvBuckets;
	pstDeque->papvBuckets[0] = pvBucket;
	pstDeque->szNumBuckets = szNumBuckets;

	return true;
}

/**
 * Append a new bucket of items to the end of the deque's set of buckets
 */
static inline bool bucket_append_to_end(std_deque_t * pstDeque)
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
	szSize = szNumBuckets * pstDeque->stContainer.szSizeofItem;
	papvBuckets = (void**)std_memoryhandler_realloc(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pstDeque->papvBuckets, szSize);
	if (papvBuckets == NULL)
	{
		std_memoryhandler_free(pstDeque->stContainer.pstMemoryHandler, pstDeque->stContainer.eHas, pvBucket);
		return false;
	}

	pstDeque->papvBuckets = papvBuckets;
	pstDeque->papvBuckets[szNumBuckets - 1U] = pvBucket;
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
	return bucket_at(pvBucket, szRemainder, pstContainer->szSizeofItem);
}

/**
 *
 */
void stdlib_deque_next(std_iterator_t * pstIterator)
{
	std_deque_iterator_t * pstDequeIt = ITERATOR_TO_DEQUEIT(pstIterator);
	std_container_t* pstContainer = pstDequeIt->stIterator.pstContainer;

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
	std_container_t* pstContainer = pstIterator->pstContainer;

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
	if (pstContainer->szNumItems == 0)
	{
		stdlib_iterator_construct_done(pstIterator);
	}
	else
	{
		stdlib_iterator_construct(pstIterator, pstContainer, stdlib_deque_at(pstContainer, 0));

		std_deque_iterator_t* pstDequeIt = ITERATOR_TO_DEQUEIT(pstIterator);
		pstDequeIt->szIndex			= 0U;
		pstDequeIt->szRangeLen		= pstContainer->szNumItems;
	}
}

/**
 *
 */
void stdlib_deque_reverseiterator_construct(std_container_t* pstContainer, std_iterator_t* pstIterator)
{
	if (pstContainer->szNumItems == 0)
	{
		stdlib_iterator_construct_done(pstIterator);
	}
	else
	{
		stdlib_iterator_construct(pstIterator, pstContainer, stdlib_deque_at(pstContainer, pstContainer->szNumItems - 1U));

		std_deque_iterator_t* pstDequeIt = ITERATOR_TO_DEQUEIT(pstIterator);
		pstDequeIt->szIndex = pstContainer->szNumItems - 1U;
		pstDequeIt->szRangeLen = pstContainer->szNumItems;
	}
}

/**
 *
 */
size_t stdlib_deque_push_front(std_container_t * pstContainer, const void * pvBase, size_t szNumItems)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	void * pvItem;
	size_t i;

	for (i = 0; i < szNumItems; i++, pvBase = STD_LINEAR_ADD(pvBase, pstContainer->szSizeofItem))
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
		memcpy(pvItem, pvBase, pstContainer->szSizeofItem);
		if (pstContainer->eHas & std_container_has_itemhandler)
		{
			std_item_relocate(pstContainer->pstItemHandler, pvItem, pvBase, pstContainer->szSizeofItem);
		}
	}

	return i;
}

/**
 *
 */
size_t stdlib_deque_push_back(std_container_t * pstContainer, const void * pvBase, size_t szNumItems)
{
	std_deque_t * pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	void * pvItem;
	size_t i;

	for (i = 0; i < szNumItems; i++, pvBase = STD_LINEAR_ADD(pvBase, pstContainer->szSizeofItem))
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
		memcpy(pvItem, pvBase, pstContainer->szSizeofItem);
		if (pstContainer->eHas & std_container_has_itemhandler)
		{
			std_item_relocate(pstContainer->pstItemHandler, pvItem, pvBase, pstContainer->szSizeofItem);
		}
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

	for (i = 0; i < szMaxItems; i++, pvResult=STD_LINEAR_ADD(pvResult, pstContainer->szSizeofItem))
	{
		// Get the address of the first item in the deque
		pvItem = stdlib_deque_at(pstContainer, pstContainer->szNumItems - 1U);
		std_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstContainer->szSizeofItem);
		pstDeque->szStartOffset++;
		pstContainer->szNumItems--;
	}

	return szMaxItems;
}

/**
 *
 */
size_t stdlib_deque_pop_back(std_container_t * pstContainer, void * pvResult, size_t szMaxItems)
{
	void * pvItem;
	size_t i;

	if (szMaxItems > pstContainer->szNumItems)
	{
		szMaxItems = pstContainer->szNumItems;
	}

	for (i = 0; i < szMaxItems; i++, pvResult=STD_LINEAR_ADD(pvResult, pstContainer->szSizeofItem))
	{
		// Get the address of the final item in the deque
		pvItem = stdlib_deque_at(pstContainer, pstContainer->szNumItems - 1U);
		std_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstContainer->szSizeofItem);
		pstContainer->szNumItems--;
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
