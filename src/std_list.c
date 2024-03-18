/*
 * std_list.c
 *
 *  Created on: 16 Oct 2022
 *      Author: Nick Pelling
 */

#include "std/list.h"
#include "std/config.h"

#define CONTAINER_TO_LIST(CONTAINER)	STD_CONTAINER_OF(CONTAINER, std_list_t, stContainer)
#define LIST_TO_CONTAINER(LIST)			&LIST->stContainer

#define ITERATOR_TO_LISTIT(IT)			STD_CONTAINER_OF(IT, std_list_iterator_t, stIterator)
#define LISTIT_TO_ITERATOR(LISTIT)		&LISTIT->stIterator

#define std_list_link_sizeof(LIST)		sizeof(LIST.pstLink[0])
#define std_list_link_typeof(LIST)		STD_TYPEOF(LIST.pstLink[0])
#define std_list_link_cast(LIST,X)		((STD_TYPEOF(LIST.pstLink))(X))
#define std_list_link_offsetof(LIST)	STD_OFFSETOF(std_list_link_typeof(LIST), stPayload)

inline void * stdlib_list_front(std_container_t * pstContainer)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
    return STD_LINEAR_ADD(pstList->pstHead, pstList->szPayloadOffset);
}

inline void * stdlib_list_back(std_container_t * pstContainer)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
    return STD_LINEAR_ADD(pstList->pstTail, pstList->szPayloadOffset);
}

inline std_list_node_t * stdlib_list_begin(std_container_t * pstContainer)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
    return pstList->pstHead;
}

inline std_list_node_t * stdlib_list_end(std_container_t * pstContainer)
{
    if (pstContainer) { /* lint - parameter not used */ }

    return NULL;
}

inline std_list_node_t * stdlib_list_rbegin(std_container_t * pstContainer)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
    return pstList->pstTail;
}

inline std_list_node_t * stdlib_list_rend(std_container_t * pstContainer)
{
    if (pstContainer) { /* lint - parameter not used */ }

    return NULL;
}

inline bool stdlib_list_empty(std_container_t * pstContainer)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
    return (pstList->szNumItems == 0);
}

// --------------------------------------------------------------------------
// Private functions
// --------------------------------------------------------------------------

/**
 *
 */
static void node_disconnect(std_list_t * pstList, std_list_node_t * pstNode)
{
	std_list_node_t * pstPrev = pstNode->pstPrev;
	std_list_node_t * pstNext = pstNode->pstNext;
	if (pstPrev == NULL)
	{
		pstList->pstHead = pstNext;
	}
	else
	{
		pstPrev->pstNext = pstNext;
	}
	if (pstNext == NULL)
	{
		pstList->pstTail = pstPrev;
	}
	else
	{
		pstNext->pstPrev = pstPrev;
	}
}

/**
 * Insert a list link after an existing list link within a list
 *
 * @param[in]	pstList			List
 * @param[in]	pstPosition		Existing list link
 * @param[in]	pstNode			New list link to insert
 */
static void node_insert_after(std_list_t * pstList, std_list_node_t * pstPosition, std_list_node_t * pstNode)
{
	if (stdlib_list_empty(LIST_TO_CONTAINER(pstList)))
	{
		pstList->pstHead = pstList->pstTail = pstNode;
		pstNode->pstNext = pstNode->pstPrev = NULL;
	}
	else
    {
    	pstNode->pstPrev = pstPosition;
    	pstNode->pstNext = pstPosition->pstNext;
        if (pstPosition->pstNext)
        {
        	pstPosition->pstNext->pstPrev = pstNode;
        }
        pstPosition->pstNext = pstNode;
        if (pstPosition == pstList->pstTail)
        {
        	pstList->pstTail = pstNode;
        }
    }
	pstList->szNumItems++;
}

/**
 * Insert a list link before an existing list link within a list
 *
 * @param[in]	pstList			List
 * @param[in]	pstPosition		Existing list link
 * @param[in]	pstNode			New list link to insert
 */
static void node_insert_before(std_list_t * pstList, std_list_node_t * pstPosition, std_list_node_t * pstNode)
{
	if (stdlib_list_empty(LIST_TO_CONTAINER(pstList)))
	{
		pstList->pstHead = pstList->pstTail = pstNode;
		pstNode->pstNext = pstNode->pstPrev = NULL;
	}
	else
	{
		pstNode->pstNext = pstPosition;
		pstNode->pstPrev = pstPosition->pstPrev;
        if (pstPosition->pstPrev)
        {
        	pstPosition->pstPrev->pstNext = pstNode;
        }
        pstPosition->pstPrev = pstNode;
        if (pstPosition == pstList->pstHead)
        {
        	pstList->pstHead = pstNode;
        }
    }
	pstList->szNumItems++;
}

// --------------------------------------------------------------------------
// Public functions
// --------------------------------------------------------------------------

/**
 * Initialise a newly-allocated list
 *
 * @param[in]	pstList			List to initialise
 * @param[in]	pstItemHandler	Allocator this list should use
 * @param[in]	szSizeofItem	Size of an item
 * @param[in]	szAlignofItem	Alignment of an item
 */
bool stdlib_list_construct(std_container_t* pstContainer, size_t szFullSizeof, size_t szPayloadOffset, std_container_has_t eHas, const std_container_handlers_t* pstHandlers)
{
	bool bResult = std_container_constructor(pstContainer, szFullSizeof - szPayloadOffset, eHas, pstHandlers);
	std_list_t* pstList = CONTAINER_TO_LIST(pstContainer);
	pstList->szLinkSize			= szFullSizeof;
	pstList->szPayloadOffset	= szPayloadOffset;
	pstList->szNumItems			= 0;
	pstList->pstHead			= NULL;
	pstList->pstTail			= NULL;
	return bResult;
}

bool stdlib_list_destruct(std_container_t* pstContainer)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);

	if (pstContainer == NULL)
	{
		return false;
	}

	while (pstList->szNumItems != 0)
	{
		stdlib_list_pop_front(pstContainer, NULL);
	}

	return true;
}

/**
 * Create a new item and insert it at the front of an existing list
 *
 * @param[in]	pstList		List
 *
 * @return Pointer to the newly-created contained item
 */
void * stdlib_list_push_front(std_container_t * pstContainer)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_node_t * pstNode;
	void * pvItem;

	pstNode = std_memoryhandler_malloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstList->szLinkSize);
	node_insert_before(pstList, pstList->pstHead, pstNode);

	pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
	if (pstContainer->eHas & std_container_has_itemhandler)
	{
		std_item_construct(pstContainer->pstItemHandler, pvItem, 1U);
	}

	return pvItem;
}

/**
 * Create a new item and insert it at the back of an existing list
 *
 * @param[in]	pstList		List
 *
 * @return Pointer to the newly-created contained item
 */
void * stdlib_list_push_back(std_container_t * pstContainer)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_node_t * pstNode;
	void * pvItem;

	pstNode = std_memoryhandler_malloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstList->szLinkSize);
	node_insert_after(pstList, pstList->pstTail, pstNode);

	pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
	if (pstContainer->eHas & std_container_has_itemhandler)
	{
		std_item_construct(pstContainer->pstItemHandler, pvItem, 1U);
	}

	return pvItem;
}

/**
 *
 */
void * stdlib_list_pop_front(std_container_t * pstContainer, void * pvResult)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_node_t * pstNode;
	void * pvItem;

	pstNode = pstList->pstHead;
	node_disconnect(pstList, pstNode);
	pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
	std_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstList->szLinkSize - pstList->szPayloadOffset);
	std_memoryhandler_free(pstContainer->pstMemoryHandler, pstContainer->eHas, pstNode);

	pstList->szNumItems--;

	return pvResult;
}

/**
 *
 */
void * stdlib_list_pop_back(std_container_t * pstContainer, void * pvResult)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_node_t * pstNode;
	void * pvItem;

	pstNode = pstList->pstTail;
	node_disconnect(pstList, pstNode);
	pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
	std_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstList->szLinkSize - pstList->szPayloadOffset);
	std_memoryhandler_free(pstContainer->pstMemoryHandler, pstContainer->eHas, pstNode);

	pstList->szNumItems--;

	return pvResult;
}

/**
 * Step a list iterator forwards through a list
 *
 * @param[in]	pstIterator	List iterator
 */
void stdlib_list_next(std_iterator_t * pstIterator)
{
	std_list_iterator_t * pstListIt = ITERATOR_TO_LISTIT(pstIterator);
    if (pstListIt->pstNext == pstListIt->pstEnd)
    {
    	pstListIt->stIterator.bDone = true;
    }
    else
    {
    	pstListIt->pstNode = pstListIt->pstNext;
    	pstListIt->stIterator.pvRef = STD_LINEAR_ADD(pstListIt->pstNode, pstListIt->szPayloadOffset);
    	pstListIt->pstNext = pstListIt->pstNode->pstNext;
    }
}

/**
 * Step a list iterator backwards through a list
 *
 * @param[in]	pstIterator	List iterator
 */
void stdlib_list_prev(std_iterator_t * pstIterator)
{
	std_list_iterator_t * pstListIt = ITERATOR_TO_LISTIT(pstIterator);
    if (pstListIt->pstNext == pstListIt->pstEnd)
    {
    	pstListIt->stIterator.bDone = true;
    }
    else
    {
    	pstListIt->pstNode = pstListIt->pstNext;
    	pstListIt->stIterator.pvRef = STD_LINEAR_ADD(pstListIt->pstNode, pstListIt->szPayloadOffset);
    	pstListIt->pstNext = pstListIt->pstNode->pstPrev;
    }
}

/**
 * Set up a list iterator to step forwards through a list
 *
 * @param[in]	pstContainer	List
 * @param[in]	pstIterator		List iterator
 */
void stdlib_list_forwarditerator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_iterator_t * pstListIt = ITERATOR_TO_LISTIT(pstIterator);
	std_list_node_t *pstNode;

	if (pstList->pstHead == NULL)
	{
		pstListIt->stIterator.bDone = true;
	}
	else
	{
		pstListIt->szLinkSize = pstList->szLinkSize;
		pstListIt->szPayloadOffset = pstList->szPayloadOffset;

		pstNode = stdlib_list_begin(pstContainer);
		pstListIt->pstBegin = pstNode;
		pstListIt->pstEnd   = stdlib_list_end(pstContainer);
		pstListIt->pstNext	= pstNode->pstNext;
		pstListIt->pstNode	= pstNode;

		pstListIt->stIterator.bDone = false;
		pstListIt->stIterator.pvRef = STD_LINEAR_ADD(pstNode, pstListIt->szPayloadOffset);
    }
}

/**
 * Set up a list iterator to step backwards through a list
 *
 * @param[in]	pstContainer	List
 * @param[in]	pstIterator		List iterator
 */
void stdlib_list_reverseiterator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_iterator_t * pstListIt = ITERATOR_TO_LISTIT(pstIterator);
	std_list_node_t *pstNode;

	if (pstList->pstHead == NULL)
	{
		pstListIt->stIterator.bDone = true;
	}
	else
	{
		pstListIt->szLinkSize = pstList->szLinkSize;
		pstListIt->szPayloadOffset = pstList->szPayloadOffset;

		pstNode = stdlib_list_rbegin(pstContainer);
		pstListIt->pstBegin = pstNode;
		pstListIt->pstEnd   = stdlib_list_rend(pstContainer);
		pstListIt->pstNext	= pstNode->pstPrev;
		pstListIt->pstNode	= pstNode;

		pstListIt->stIterator.bDone = false;
		pstListIt->stIterator.pvRef = STD_LINEAR_ADD(pstNode, pstListIt->szPayloadOffset);
    }
}
