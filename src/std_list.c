/*
 * std_list.c

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

#include "std/list.h"
#include "std/config.h"

// Cast a generic container to a list, and a list to a generic container
#define CONTAINER_TO_LIST(CONTAINER)	STD_CONTAINER_OF(CONTAINER, std_list_t, stContainer)
#define LIST_TO_CONTAINER(LIST)			&LIST->stContainer

// Cast a generic iterator to a list iterator, and a list iterator to a generic iterator
#define ITERATOR_TO_LISTIT(IT)			STD_CONTAINER_OF(IT, std_list_iterator_t, stIterator)
#define LISTIT_TO_ITERATOR(LISTIT)		&LISTIT->stIterator

#define LIST_LINK_SIZEOF(LIST)			sizeof(LIST.pstLink[0])
#define LIST_LINK_TYPEOF(LIST)			STD_TYPEOF(LIST.pstLink[0])
#define LIST_LINK_CAST(LIST,X)			((STD_TYPEOF(LIST.pstLink))(X))
#define LIST_LINK_OFFSETOF(LIST)		STD_OFFSETOF(LIST_LINK_TYPEOF(LIST), stPayload)

// --------------------------------------------------------------------------
// Private functions
// --------------------------------------------------------------------------

/**
 * Disconnect a doubly-linked node from its predecessor and successor nodes
 * 
 * @param[in]	pstList		List to update
 * @param[in]	pstNode		Node to disconnect
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
	if (pstList->stContainer.szNumItems == 0)
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
	pstList->stContainer.szNumItems++;
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
	if (pstList->stContainer.szNumItems == 0)
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
	pstList->stContainer.szNumItems++;
}

// --------------------------------------------------------------------------
// Public functions
// --------------------------------------------------------------------------

/**
 * Initialise a newly-allocated list
 *
 * @param[in]	pstContainer	List container to initialise
 * @param[in]	szSizeofItem	Size of a raw (unwrapped) item
 * @param[in]	szWrappedSizeof	Size of a wrapped item (i.e. including its node header)
 * @param[in]	szPayloadOffset	Offset of a raw (payload) item inside a wrapped (node) item
 * @param[in]	eHas			Specifies which kinds of handlers this container uses
 */
void stdlib_list_construct(std_container_t* pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas)
{
	std_container_constructor(pstContainer, szSizeof, eHas);
	std_list_t* pstList = CONTAINER_TO_LIST(pstContainer);
	pstList->szLinkSize			= szWrappedSizeof;
	pstList->szPayloadOffset	= szPayloadOffset;
	pstList->pstHead			= NULL;
	pstList->pstTail			= NULL;
}

/**
 * Destruct a list container (and all the things inside it)
 * 
 * @param[in]	pstContainer		List container to destruct
 * 
 * @return		True if list container was able to be destructed, else false
 */
bool stdlib_list_destruct(std_container_t* pstContainer)
{
	if (pstContainer == NULL)
	{
		return false;
	}

	stdlib_list_pop_front(pstContainer, NULL, pstContainer->szNumItems);

	return true;
}

/**
 * Push a series of items onto the front of a list container
 *
 * @param[in]	pstContainer	List container to push onto
 * @param[in]	pstSeries		Linear series of items to push onto the list
 * 
 * @return Number of items pushed onto the container
 */
size_t stdlib_list_push_front(std_container_t * pstContainer, std_linear_series_t * pstSeries)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_node_t * pstNode;
	void * pvItem;
	size_t i;

	for (i = 0; !std_linear_series_done(pstSeries); i++, std_linear_series_next(pstSeries))
	{
		pstNode = std_memoryhandler_malloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstList->szLinkSize);
		if (pstNode == NULL)
		{
			break;
		}
		node_insert_before(pstList, pstList->pstHead, pstNode);

		pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
		memcpy(pvItem, pstSeries->pvData, pstList->stContainer.szSizeofItem);
		if (pstContainer->eHas & std_container_has_itemhandler)
		{
			std_item_relocate(pstContainer->pstItemHandler, pvItem, pstSeries->pvData, pstList->stContainer.szSizeofItem);
		}
	}

	return i;
}

/**
 * Push a series of items onto the back of a list container
 *
 * @param[in]	pstContainer	List container to push items onto
 * @param[in]	pstSeries		Linear series of items to push onto the list
 */
size_t stdlib_list_push_back(std_container_t * pstContainer, std_linear_series_t* pstSeries)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_node_t * pstNode;
	void * pvItem;
	size_t i;

	for (i = 0; !std_linear_series_done(pstSeries); i++,std_linear_series_next(pstSeries))
	{
		pstNode = std_memoryhandler_malloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstList->szLinkSize);
		if (pstNode == NULL)
		{
			break;
		}
		node_insert_after(pstList, pstList->pstTail, pstNode);

		pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
		memcpy(pvItem, pstSeries->pvData, pstContainer->szSizeofItem);
		if (pstContainer->eHas & std_container_has_itemhandler)
		{
			std_item_relocate(pstContainer->pstItemHandler, pvItem, pstSeries->pvData, pstList->stContainer.szSizeofItem);
		}
	}

	return i;
}

/**
 * Pop a series of items off the front of a list container
 *
 * @param[in]	pstContainer	List container to pop items off
 * @param[in]	pvBase			Array of items to push onto the list
 * @param[in]	szNumItems		Number of items in the array
 * 
 * @return Number of items popped off the front of a list container
 */
size_t stdlib_list_pop_front(std_container_t * pstContainer, void * pvResult, size_t szMaxItems)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_node_t * pstNode;
	void * pvItem;
	size_t i;

	if (szMaxItems > pstContainer->szNumItems)
	{
		szMaxItems = pstContainer->szNumItems;
	}

	for (i = 0; i < szMaxItems; i++)
	{
		pstNode = pstList->pstHead;
		node_disconnect(pstList, pstNode);
		pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
		std_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstContainer->szSizeofItem);
		std_memoryhandler_free(pstContainer->pstMemoryHandler, pstContainer->eHas, pstNode);
		pstContainer->szNumItems--;
		if (pvResult != NULL)
		{
			pvResult = STD_LINEAR_ADD(pvResult, pstContainer->szSizeofItem);
		}
	}

	return szMaxItems;
}

/**
 * Pop a series of items off the back of a list container
 *
 * @param[in]	pstContainer	List container to pop items off
 * @param[in]	pvBase			Array of items to push onto the list
 * @param[in]	szNumItems		Number of items in the array
 *
 * @return Number of items popped off the back of a list container
 */
size_t stdlib_list_pop_back(std_container_t * pstContainer, void * pvResult, size_t szMaxItems)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_node_t * pstNode;
	void * pvItem;
	size_t i;

	if (szMaxItems > pstContainer->szNumItems)
	{
		szMaxItems = pstContainer->szNumItems;
	}

	for (i = 0; i < szMaxItems; i++)
	{
		pstNode = pstList->pstTail;
		node_disconnect(pstList, pstNode);
		pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
		std_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstContainer->szSizeofItem);
		std_memoryhandler_free(pstContainer->pstMemoryHandler, pstContainer->eHas, pstNode);
		pstContainer->szNumItems--;
		if (pvResult != NULL)
		{
			pvResult = STD_LINEAR_ADD(pvResult, pstContainer->szSizeofItem);
		}
	}

	return szMaxItems;
}

/**
 * Step a list iterator forwards through a list container
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
 * Step a list iterator backwards through a list container
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
 * @param[in]	pstContainer	List container
 * @param[in]	pstIterator		List iterator
 */
void stdlib_list_forwarditerator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_iterator_t * pstListIt = ITERATOR_TO_LISTIT(pstIterator);

	if (pstList->pstHead == NULL)
	{
		stdlib_iterator_construct_done(pstIterator);
	}
	else
	{
		std_list_node_t * pstNode = pstList->pstHead;
		void * pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
		stdlib_iterator_construct(pstIterator, pstContainer, pvItem);

		pstListIt->szLinkSize = pstList->szLinkSize;
		pstListIt->szPayloadOffset = pstList->szPayloadOffset;
		pstListIt->pstBegin = pstNode;
		pstListIt->pstEnd   = NULL;
		pstListIt->pstNext	= pstNode->pstNext;
		pstListIt->pstNode	= pstNode;
    }
}

/**
 * Set up a list iterator to step backwards through a list container
 *
 * @param[in]	pstContainer	List container
 * @param[in]	pstIterator		List iterator
 */
void stdlib_list_reverseiterator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator)
{
	std_list_t * pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_iterator_t * pstListIt = ITERATOR_TO_LISTIT(pstIterator);

	if (pstList->pstTail == NULL)
	{
		stdlib_iterator_construct_done(pstIterator);
	}
	else
	{
		std_list_node_t * pstNode = pstList->pstTail;
		void * pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
		stdlib_iterator_construct(pstIterator, pstContainer, pvItem);

		pstListIt->szLinkSize = pstList->szLinkSize;
		pstListIt->szPayloadOffset = pstList->szPayloadOffset;
		pstListIt->pstBegin = pstNode;
		pstListIt->pstEnd   = NULL;
		pstListIt->pstNext	= pstNode->pstPrev;
		pstListIt->pstNode	= pstNode;
    }
}

/**
 * Insert a linear series of items immediately after the current iterator item
 *
 * @param[in]	pstIterator		List iterator
 * @param[in]	pvBase			Start of the linear series of items
 * @param[in]	szNumItems		Number of items in the linear series
 * 
 * @return Number of items successfully inserted
 */
size_t stdlib_list_insert_after(std_iterator_t* pstIterator, const void* pvBase, size_t szNumItems)
{
	std_container_t* pstContainer = pstIterator->pstContainer;
	std_list_t* pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_node_t* pstNode;
	std_list_node_t* pstItNode;
	void* pvItem;
	size_t i;

	pstItNode = STD_LINEAR_SUB(pstIterator->pvRef, pstList->szPayloadOffset);

	for (i = 0; i < szNumItems; i++, pvBase = STD_LINEAR_ADD(pvBase, pstContainer->szSizeofItem))
	{
		pstNode = std_memoryhandler_malloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstList->szLinkSize);
		if (pstNode == NULL)
		{
			break;
		}
		node_insert_after(pstList, pstItNode, pstNode);

		pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
		memcpy(pvItem, pvBase, pstContainer->szSizeofItem);
		if (pstContainer->eHas & std_container_has_itemhandler)
		{
			std_item_relocate(pstContainer->pstItemHandler, pvItem, pvBase, pstList->stContainer.szSizeofItem);
		}
	}

	return i;
}

/**
 * Insert a linear series of items immediately before the current iterator item
 *
 * @param[in]	pstIterator		List iterator
 * @param[in]	pvBase			Start of the linear series of items
 * @param[in]	szNumItems		Number of items in the linear series
 *
 * @return Number of items successfully inserted
 */
size_t stdlib_list_insert_before(std_iterator_t* pstIterator, const void* pvBase, size_t szNumItems)
{
	std_container_t* pstContainer = pstIterator->pstContainer;
	std_list_t* pstList = CONTAINER_TO_LIST(pstContainer);
	std_list_node_t* pstNode;
	std_list_node_t* pstItNode;
	void* pvItem;
	size_t i;

	pstItNode = STD_LINEAR_SUB(pstIterator->pvRef, pstList->szPayloadOffset);

	for (i = 0; i < szNumItems; i++, pvBase = STD_LINEAR_ADD(pvBase, pstContainer->szSizeofItem))
	{
		pstNode = std_memoryhandler_malloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstList->szLinkSize);
		if (pstNode == NULL)
		{
			break;
		}
		node_insert_before(pstList, pstItNode, pstNode);

		pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
		memcpy(pvItem, pvBase, pstContainer->szSizeofItem);
		if (pstContainer->eHas & std_container_has_itemhandler)
		{
			std_item_relocate(pstContainer->pstItemHandler, pvItem, pvBase, pstList->stContainer.szSizeofItem);
		}
	}

	return i;
}

/**
 * Erase a single item at the current iterator item
 *
 * @param[in]	pstIterator		List iterator
 */
void stdlib_list_erase(std_iterator_t* pstIterator)
{
	std_container_t* pstContainer = pstIterator->pstContainer;
	std_list_t* pstList = CONTAINER_TO_LIST(pstContainer);

	node_disconnect(pstList, pstIterator->pvRef);
}

// -------------------------------------------------------------------------

static bool list_default_destruct(const std_item_handler_t* pstItemHandler, void* pvData)
{
	if (pstItemHandler) { /* Unused parameter */ }
	return stdlib_list_destruct((std_container_t*)pvData);
}

// typedef std_list(int) list_int_t;

const std_item_handler_t std_list_default_itemhandler =
{
	.szElementSize = sizeof(std_list_t),
	.pfn_Destructor = &list_default_destruct,
	.pfn_Relocator = NULL
};
