/*
 * vector.c
 *
 *  Created on: 15 Oct 2022
 *      Author: Nick Pelling
 */

#include <stdlib.h>		// for qsort
#include <stdarg.h>		// for variadic args

#include "std/vector.h"

#define CONTAINER_TO_VECTOR(CONTAINER)	STD_CONTAINER_OF(CONTAINER, std_vector_t, stContainer)
#define VECTOR_TO_CONTAINER(VECTOR)		&VECTOR->stContainer

#define ITERATOR_TO_VECTORIT(IT)		STD_CONTAINER_OF(IT, std_vector_iterator_t, stIterator)
#define VECTORIT_TO_ITERATOR(VECTORIT)	&VECTORIT->stIterator

static inline void * next_item(std_iterator_t * pstIterator, void * pvThis)
{
	return STD_LINEAR_ADD(pvThis, pstIterator->szSizeofItem);
}

static inline void * prev_item(std_iterator_t * pstIterator, void * pvThis)
{
	return STD_LINEAR_SUB(pvThis, pstIterator->szSizeofItem);
}

inline void * stdlib_vector_at(std_container_t * pstContainer, int32_t iIndex)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	return STD_LINEAR_ADD(pstVector->pvStartAddr, pstVector->stContainer.szSizeofItem * (iIndex));
}

inline void * stdlib_vector_front(std_container_t * pstContainer)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	return pstVector->pvStartAddr;
}

inline void * stdlib_vector_back(std_container_t * pstContainer)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	return stdlib_vector_at(pstContainer, (int32_t)(pstVector->szNumItems) - 1);
}

inline void * stdlib_vector_begin(std_container_t * pstContainer)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	return pstVector->pvStartAddr;
}

inline void * stdlib_vector_end(std_container_t * pstContainer)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	return stdlib_vector_at(pstContainer, (int32_t)(pstVector->szNumItems));
}

inline void * stdlib_vector_rbegin(std_container_t * pstContainer)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	return stdlib_vector_at(pstContainer, (int32_t)(pstVector->szNumItems) - 1U);
}

inline void * stdlib_vector_rend(std_container_t * pstContainer)
{
	return stdlib_vector_at(pstContainer, -1);
}

inline bool stdlib_vector_empty(std_container_t * pstContainer)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	return (pstVector->szNumItems == 0);
}

/**
 * Construct a vector
 *
 * @param[in]	pstContainer	Vector to initialise
 * @param[in]	szFullSizeof	Size of a (possibly wrapped) item
 * @param[in]	szPayloadOffset	Offset to the payload within the (possibly wrapped) item
 * @param[in]	eHas			Enumeration saying which handlers to expect
 * @param[in]	pstHandlers		Struct holding handlers declared
 */
bool stdlib_vector_construct(std_container_t* pstContainer, size_t szFullSizeof, size_t szPayloadOffset, std_container_has_t eHas, const std_container_handlers_t* pstHandlers)
{
	bool bResult = std_container_constructor(pstContainer, szFullSizeof - szPayloadOffset, eHas, pstHandlers);
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	pstVector->szNumItems	= 0;
	pstVector->szNumAlloced	= 0;
	pstVector->pvStartAddr	= NULL;

	return bResult;
}

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
		for (i = 0; i < pstVector->szNumItems; i++)
		{
			(*pstContainer->pstItemHandler->pfn_Destructor)(pstContainer->pstItemHandler, stdlib_vector_at(pstContainer, i));
		}
	}

	// Free the memory allocated for this vector
	free(pstVector->pvStartAddr);

	// Clear out all the fields (just to be tidy)
	pstVector->szNumItems	= 0;
	pstVector->szNumAlloced	= 0;
	pstVector->pvStartAddr	= NULL;

	return true;
}
/**
 * Reserve space within a vector for a number of items
 *
 * @param[in]	pstContainer	Vector to reserve space for
 * @param[in]	szNewSize
 */
void stdlib_vector_reserve(std_container_t * pstContainer, size_t szNewSize)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	size_t szNewCapacity;
	void * pvNewStart;

	if (szNewSize > pstVector->szNumAlloced)
	{
		szNewCapacity = 1U << (32U - __builtin_clz(szNewSize));
		if (szNewCapacity < szNewSize)
		{
			szNewCapacity <<= 1;
		}

		if (szNewCapacity != pstVector->szNumAlloced)
		{
			pstVector->szNumAlloced = szNewCapacity;
			size_t szTotalSize = pstVector->szNumAlloced * pstVector->stContainer.szSizeofItem;
			pvNewStart = std_memoryhandler_realloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstVector->pvStartAddr, szTotalSize);
			if (pstContainer->eHas & std_container_has_itemhandler)
			{
				std_item_relocate(pstContainer->pstItemHandler, pvNewStart, pstVector->pvStartAddr, szTotalSize);
			}
			pstVector->pvStartAddr = pvNewStart;
		}
	}
}

/**
 * Shrink an over-allocated block to exactly fit the number of elements in it
 *
 * @param[in]	pstContainer	Vector to shrink
 */
void stdlib_vector_fit(std_container_t * pstContainer)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	void * pvNewPtr;
	size_t szSize;
	size_t szNewCapacity;

	szNewCapacity = 1U << (32U - __builtin_clz(pstVector->szNumItems));
	if (szNewCapacity < pstVector->szNumItems)
	{
		szNewCapacity <<= 1;
	}

	if (pstVector->szNumAlloced != szNewCapacity)
	{
		pstVector->szNumAlloced = szNewCapacity;
		szSize = szNewCapacity * pstVector->stContainer.szSizeofItem;
		pvNewPtr = std_memoryhandler_realloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstVector->pvStartAddr, szSize);
		if (pstContainer->eHas & std_container_has_itemhandler)
		{
			std_item_relocate(pstContainer->pstItemHandler, pvNewPtr, pstVector->pvStartAddr, szSize);
		}
		pstVector->pvStartAddr = pvNewPtr;
	}
}

/**
 * Create a new item at the very front of a vector
 *
 * @param[in]	pstContainer	Vector to add an item to
 *
 * @return	Pointer to the newly created item
 */
void * stdlib_vector_push_front(std_container_t * pstContainer)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	void * pvItem;

	pstVector->szNumItems++;
	stdlib_vector_reserve(pstContainer, pstVector->szNumItems);

	std_item_relocate(pstContainer->pstItemHandler, stdlib_vector_at(pstContainer, 1), stdlib_vector_at(pstContainer, 0), (pstVector->szNumItems - 1U) * pstContainer->szSizeofItem);

	pvItem = stdlib_vector_at(pstContainer, 0);
	if (pstContainer->eHas & std_container_has_itemhandler)
	{
		std_item_construct(pstContainer->pstItemHandler, pvItem, 1);
	}

	return pvItem;
}

/**
 * Create a new item at the very back of a vector
 *
 * @param[in]	pstContainer	Vector to add an item to
 *
 * @return	Pointer to the newly created item
 */
void * stdlib_vector_push_back(std_container_t * pstContainer)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	void * pvItem;

	pstVector->szNumItems++;
	stdlib_vector_reserve( pstContainer, pstVector->szNumItems );

	pvItem = stdlib_vector_at(pstContainer, pstVector->szNumItems - 1);
	if (pstContainer->eHas & std_container_has_itemhandler)
	{
		std_item_construct(pstContainer->pstItemHandler, pvItem, 1);
	}

	return pvItem;
}

/**
 * Pop an item from the very back of a vector
 *
 * @param[in]	pstContainer	Vector to add an item to
 * @param[out]	pvResult		Where to pop the item to (can be NULL)
 *
 * @return	Pointer to the newly popped item
 */
void * stdlib_vector_pop_back(	std_container_t * pstContainer, void * pvResult)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	void * pvItem;

	pvItem = stdlib_vector_at(pstContainer, pstVector->szNumItems - 1U);
	std_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstVector->stContainer.szSizeofItem);
	pstVector->szNumItems--;

	return pvResult;
}

/**
 * Step a vector iterator forwards in memory
 *
 * @param[in]	pstIterator		Vector iterator
 */
void stdlib_vector_iterator_next(std_iterator_t * pstIterator)
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
 * Step a vector iterator backwards in memory
 *
 * @param[in]	pstIterator		Vector iterator
 */
void stdlib_vector_iterator_prev(std_iterator_t * pstIterator)
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
	const std_item_handler_t * pstItemHandler;

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

void stdlib_vector_forwarditerator_range(std_container_t * pstContainer, std_iterator_t * pstIterator, void *pvBegin, void * pvEnd)
{
	if (pvBegin == pvEnd)
	{
		pstIterator->bDone = true;
	}
	else
	{
		pstIterator->szSizeofItem = pstContainer->szSizeofItem;
		pstIterator->pvBegin	= pvBegin;
		pstIterator->pvEnd		= pvEnd;
		pstIterator->pvRef		= pvBegin;
		pstIterator->pvNext		= next_item(pstIterator, pvBegin);
		pstIterator->bDone		= false;
	}
}

void stdlib_vector_forwarditerator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	stdlib_vector_forwarditerator_range(	pstContainer,
											pstIterator,
											stdlib_vector_begin(pstContainer),
											stdlib_vector_end(pstContainer)		);
}

void stdlib_vector_reverseiterator_range(std_container_t * pstContainer, std_iterator_t * pstIterator, void *pvBegin, void * pvEnd)
{
	if (pvBegin == pvEnd)
	{
		pstIterator->bDone = true;
	}
	else
	{
		pstIterator->szSizeofItem = pstContainer->szSizeofItem;
		pstIterator->pvBegin	= pvBegin;
		pstIterator->pvEnd		= pvEnd;
		pstIterator->pvRef		= pvBegin;
		pstIterator->pvNext		= prev_item(pstIterator, pvBegin);
		pstIterator->bDone		= false;
	}
}

void stdlib_vector_reverseiterator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	stdlib_vector_reverseiterator_range(	pstContainer,
											pstIterator,
											stdlib_vector_rbegin(pstContainer),
											stdlib_vector_rend(pstContainer)		);
}

#if 0
/**
 * Compare two vectors for equality using a per-item comparison function
 * 
 * Note: this was just a demo, a more general container comparison is needed
 *
 * @param[in]	pstContainer1	First  vector of items to compare
 * @param[in]	pstContainer2	Second vector of items to compare
 * @param[in]	pfnEqual		Per-item equality comparison function
 */
bool stdlib_vector_equal(std_container_t * pstContainer1, std_container_t * pstContainer2, pfn_std_equal_t pfnEqual)
{
	std_vector_t * pstVector1 = CONTAINER_TO_VECTOR(pstContainer1);
	std_vector_t * pstVector2 = CONTAINER_TO_VECTOR(pstContainer2);
	size_t szNum;

	if (	(pstVector1->stContainer.szSizeofItem != pstVector2->stContainer.szSizeofItem)
		||	(pstVector1->szNumItems != pstVector2->szNumItems)	)
	{
		return false;
	}
	szNum = pstVector1->szNumItems;
	for (size_t szIndex = 0; szIndex < szNum; szIndex++)
	{
		if (pfnEqual(stdlib_vector_at(pstContainer1, szIndex), stdlib_vector_at(pstContainer2, szIndex)) == false)
		{
			return false;
		}
	}

	return true;
}
#endif