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

/**
 * Construct a vector container
 *
 * @param[in]	pstContainer	Vector to initialise
 * @param[in]	szFullSizeof	Size of a (possibly wrapped) item
 * @param[in]	szPayloadOffset	Offset to the payload within the (possibly wrapped) item
 * @param[in]	eHas			Enumeration saying which handlers to expect
 * @param[in]	pstHandlers		Struct holding handlers declared
 *
 * @return True if construction was successful, else false
 */
bool stdlib_vector_construct(std_container_t* pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas, const std_container_handlers_t* pstHandlers)
{
	bool bResult = std_container_constructor(pstContainer, szSizeof, eHas, pstHandlers);
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	pstVector->szNumAlloced	= 0;
	pstVector->pvStartAddr	= NULL;

	return bResult;
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
			(*pstContainer->pstItemHandler->pfn_Destructor)(pstContainer->pstItemHandler, stdlib_vector_at(pstContainer, i));
		}
	}

	// Free the memory allocated for this vector
	free(pstVector->pvStartAddr);

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
 */
void stdlib_vector_reserve(std_container_t * pstContainer, size_t szNewSize)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	size_t szNewCapacity;
	void * pvNewStart;

	if (szNewSize > pstVector->szNumAlloced)
	{
		szNewCapacity = 1U << (64U - __builtin_clzll(szNewSize));
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
 * @param[in]	pstContainer	Vector container to shrink
 */
void stdlib_vector_fit(std_container_t * pstContainer)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);
	void * pvNewPtr;
	size_t szNumItems;
	size_t szSize;

	szNumItems = pstContainer->szNumItems;
	if (pstVector->szNumAlloced != szNumItems)
	{
		pstVector->szNumAlloced = szNumItems;
		szSize = szNumItems * pstVector->stContainer.szSizeofItem;
		pvNewPtr = std_memoryhandler_realloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstVector->pvStartAddr, szSize);
		if (pstContainer->eHas & std_container_has_itemhandler)
		{
			std_item_relocate(pstContainer->pstItemHandler, pvNewPtr, pstVector->pvStartAddr, szSize);
		}
		pstVector->pvStartAddr = pvNewPtr;
	}
}

/**
 * Push a series of items onto the front of a vector
 *
 * @param[in]	pstContainer	Vector container to push the series of items onto
 * @param[in]	pvBase			Pointer to start of array of items
 * @param[in]	szNumItems		Number of items in the array
 */
void stdlib_vector_push_front(std_container_t * pstContainer, const void * pvBase, size_t szNumItems)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);

	pstContainer->szNumItems += szNumItems;
	stdlib_vector_reserve(pstContainer, pstContainer->szNumItems);

	std_item_relocate(pstContainer->pstItemHandler, stdlib_vector_at(pstContainer, szNumItems), stdlib_vector_at(pstContainer, 0), (pstContainer->szNumItems - szNumItems) * pstContainer->szSizeofItem);
	memcpy(stdlib_vector_at(pstContainer, 0), pvBase, szNumItems * pstContainer->szSizeofItem);

	if (pstContainer->eHas & std_container_has_itemhandler)
	{
		std_item_construct(pstContainer->pstItemHandler, stdlib_vector_at(pstContainer, 0), szNumItems);
	}
}

/**
 * Push a series of items onto the back of a vector
 *
 * @param[in]	pstContainer	Vector container to push the series of items onto
 * @param[in]	pvBase			Pointer to start of array of items
 * @param[in]	szNumItems		Number of items in the array
 */
void stdlib_vector_push_back(std_container_t * pstContainer, const void *pvBase, size_t szNumItems)
{
	std_vector_t * pstVector = CONTAINER_TO_VECTOR(pstContainer);

	pstContainer->szNumItems += szNumItems;
	stdlib_vector_reserve( pstContainer, pstContainer->szNumItems );
	memcpy(stdlib_vector_at(pstContainer, pstContainer->szNumItems - szNumItems), pvBase, szNumItems * pstContainer->szSizeofItem);

	if (pstContainer->eHas & std_container_has_itemhandler)
	{
		std_item_construct(pstContainer->pstItemHandler, stdlib_vector_at(pstContainer, pstContainer->szNumItems - szNumItems), szNumItems);
	}
}

/**
 * Pop an item from the very back of a vector
 *
 * @param[in]	pstContainer	Vector to add an item to
 * @param[out]	pvResult		Where to pop the item to (can be NULL)
 *
 * @return	Pointer to the newly popped item
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
		pvItem = stdlib_vector_at(pstContainer, pstContainer->szNumItems - 1U);
		std_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstVector->stContainer.szSizeofItem);
		pstContainer->szNumItems--;
		if (pvResult)
		{
			pvResult = STD_LINEAR_ADD(pvResult, pstContainer->szSizeofItem);
		}
	}

	return szMaxItems;
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

void stdlib_vector_forwarditerator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	std_vector_t* pstVector = CONTAINER_TO_VECTOR(pstContainer);
	void * pvBegin = pstVector->pvStartAddr;
	void * pvEnd   = stdlib_vector_at(pstContainer, pstContainer->szNumItems);
	stdlib_vector_forwarditerator_range( pstContainer, pstIterator, pvBegin, pvEnd);
}

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

void stdlib_vector_reverseiterator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	std_vector_t* pstVector = CONTAINER_TO_VECTOR(pstContainer);
	void * pvBegin = stdlib_vector_at(pstContainer, pstContainer->szNumItems - 1U);
	void * pvEnd = STD_LINEAR_SUB(pstVector->pvStartAddr, pstContainer->szSizeofItem);
	stdlib_vector_reverseiterator_range( pstContainer, pstIterator, pvBegin, pvEnd );
}
