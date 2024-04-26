/*
 * src/std_forward_list.c

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

#include "std/forward_list.h"

 // Cast a generic container to a forward list, and a forward list to a generic container
#define CONTAINER_TO_FORWARD_LIST(CONTAINER)	STD_CONTAINER_OF(CONTAINER, std_forward_list_t, stContainer)
#define FORWARD_LIST_TO_CONTAINER(LIST)			&LIST->stContainer

// Cast a generic iterator to a forward list iterator, and a forward list iterator to a generic iterator
#define ITERATOR_TO_FORWARDLISTIT(IT)			STD_CONTAINER_OF(IT, std_forward_list_iterator_t, stIterator)
#define FORWARDLISTIT_TO_ITERATOR(LISTIT)		&LISTIT->stIterator

#define FORWARD_LIST_LINK_SIZEOF(LIST)			sizeof(LIST.pstLink[0])
#define FORWARD_LIST_LINK_TYPEOF(LIST)			STD_TYPEOF(LIST.pstLink[0])
#define FORWARD_LIST_LINK_CAST(LIST,X)			((STD_TYPEOF(LIST.pstLink))(X))
#define FORWARD_LIST_LINK_OFFSETOF(LIST)		STD_OFFSETOF(FORWARD_LIST_LINK_TYPEOF(LIST), stPayload)

// --------------------------------------------------------------------------
// Private functions
// --------------------------------------------------------------------------

/**
 * Disconnect a singly-linked node from its predecessor and successor nodes
 *
 * @param[in]	pstList		List to update
 * @param[in]	pstNode		Node to disconnect
 */
static void node_disconnect(std_forward_list_t* pstList, std_forward_list_node_t* pstNode, std_forward_list_node_t* pstPrev)
{
	std_forward_list_node_t* pstNext = pstNode->pstNext;
	if (pstPrev == NULL)
	{
		pstList->pstHead = pstNext;
	}
	else
	{
		pstPrev->pstNext = pstNext;
	}
}

/**
 * Insert a list link after an existing list link within a list
 *
 * @param[in]	pstList			List
 * @param[in]	pstPosition		Existing list link
 * @param[in]	pstNode			New list link to insert
 */
static void node_insert_after(std_forward_list_t* pstList, std_forward_list_node_t* pstPosition, std_forward_list_node_t* pstNode)
{
	if (pstList->stContainer.szNumItems == 0)
	{
		pstList->pstHead = pstNode;
		pstList->pstLast = pstNode;
		pstNode->pstNext = NULL;
	}
	else
	{
		pstNode->pstNext = pstPosition->pstNext;
		pstPosition->pstNext = pstNode;
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
static void node_insert_before(std_forward_list_t* pstList, std_forward_list_node_t* pstPosition, std_forward_list_node_t* pstNode)
{
	if (pstList->stContainer.szNumItems == 0)
	{
		pstList->pstHead = pstNode;
		pstList->pstLast = pstNode;
		pstNode->pstNext = NULL;
	}
	else
	{
		pstNode->pstNext = pstPosition;
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
 * Initialise a newly-allocated forward list
 *
 * @param[in]	pstContainer	Forward list container to initialise
 * @param[in]	szSizeofItem	Size of a raw (unwrapped) item
 * @param[in]	szWrappedSizeof	Size of a wrapped item (i.e. including its node header)
 * @param[in]	szPayloadOffset	Offset of a raw (payload) item inside a wrapped (node) item
 * @param[in]	eHas			Specifies which kinds of handlers this container uses
 */
void stdlib_forward_list_construct(std_container_t* pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas)
{
	std_container_constructor(pstContainer, szSizeof, eHas);
	std_forward_list_t* pstList = CONTAINER_TO_FORWARD_LIST(pstContainer);
	pstList->szLinkSize = szWrappedSizeof;
	pstList->szPayloadOffset = szPayloadOffset;
	pstList->pstHead = NULL;
	pstList->pstLast = NULL;
}

/**
 * Destruct a forward list container (and all the things inside it)
 *
 * @param[in]	pstContainer		Forward list container to destruct
 *
 * @return		True if forward list container was able to be destructed, else false
 */
bool stdlib_forward_list_destruct(std_container_t* pstContainer)
{
	if (pstContainer == NULL)
	{
		return false;
	}

	stdlib_forward_list_pop_front(pstContainer, NULL, pstContainer->szNumItems);

	return true;
}

/**
 * Push a series of items onto the front of a forward list container
 *
 * @param[in]	pstContainer	Forward list container to push onto
 * @param[in]	pstSeries		Linear series of items to push onto the list
 *
 * @return Number of items pushed onto the container
 */
size_t stdlib_forward_list_push_front(std_container_t* pstContainer, std_linear_series_t* pstSeries)
{
	std_forward_list_t* pstList = CONTAINER_TO_FORWARD_LIST(pstContainer);
	std_forward_list_node_t* pstNode;
	void* pvItem;
	size_t i;

	for (i = 0; !std_linear_series_done(pstSeries); i++, std_linear_series_next(pstSeries))
	{
		pstNode = std_memoryhandler_malloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstList->szLinkSize);
		if (pstNode == NULL)
		{
			break;
		}
		pstNode->pstNext = pstList->pstHead;
		pstList->pstHead = pstNode;
		pstList->stContainer.szNumItems++;

		pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
		stdlib_container_relocate_items(pstContainer, pvItem, pstSeries->pvData, 1U);
	}

	return i;
}

/**
 * Push a series of items onto the back of a forward list container
 *
 * @param[in]	pstContainer	Forward list container to push items onto
 * @param[in]	pstSeries		Linear series of items to push onto the list
 */
size_t stdlib_forward_list_push_back(std_container_t* pstContainer, std_linear_series_t* pstSeries)
{
	std_forward_list_t* pstList = CONTAINER_TO_FORWARD_LIST(pstContainer);
	std_forward_list_node_t* pstNode;
	void* pvItem;
	size_t i;

	for (i = 0; !std_linear_series_done(pstSeries); i++, std_linear_series_next(pstSeries))
	{
		pstNode = std_memoryhandler_malloc(pstContainer->pstMemoryHandler, pstContainer->eHas, pstList->szLinkSize);
		if (pstNode == NULL)
		{
			break;
		}
		// If the forward list is empty
		if (pstList->pstHead == NULL)
		{
			// Set the forward list head to this node
			pstList->pstHead = pstNode;
		}
		else
		{
			// Link the final node in the list forward to this node
			pstList->pstLast->pstNext = pstNode;
		}
		pstList->pstLast = pstNode;
		pstNode->pstNext = NULL;
		pstList->stContainer.szNumItems++;

		pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
		stdlib_container_relocate_items(pstContainer, pvItem, pstSeries->pvData, 1);
	}

	return i;
}

/**
 * Pop a series of items off the front of a forward list container
 *
 * @param[in]	pstContainer	Forward list container to pop items off
 * @param[in]	pvBase			Array of items to push onto the list
 * @param[in]	szNumItems		Number of items in the array
 *
 * @return Number of items popped off the front of the container
 */
size_t stdlib_forward_list_pop_front(std_container_t* pstContainer, void* pvResult, size_t szMaxItems)
{
	std_forward_list_t* pstList = CONTAINER_TO_FORWARD_LIST(pstContainer);
	std_forward_list_node_t* pstNode;
	void* pvItem;
	size_t i;

	for (i = 0; i < szMaxItems; i++)
	{
		pstNode = pstList->pstHead;
		if (pstNode == NULL)
		{
			break;
		}
		pstList->pstHead = pstNode->pstNext;
		if (pstList->pstHead == NULL)
		{
			pstList->pstLast = NULL;
		}
		pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
		stdlib_item_pop(pstContainer->eHas, pstContainer->pstItemHandler, pvResult, pvItem, pstContainer->szSizeofItem);
		std_memoryhandler_free(pstContainer->pstMemoryHandler, pstContainer->eHas, pstNode);
		pstContainer->szNumItems--;
		if (pvResult != NULL)
		{
			pvResult = STD_LINEAR_ADD(pvResult, pstContainer->szSizeofItem);
		}
	}

	return i;
}

/**
 * Step a forward list iterator forwards through a forward list container
 *
 * @param[in]	pstIterator		Forward list iterator
 */
void stdlib_forward_list_next(std_iterator_t* pstIterator)
{
	std_forward_list_iterator_t* pstListIt = ITERATOR_TO_FORWARDLISTIT(pstIterator);
	std_forward_list_node_t * pstNode;

	if (pstListIt->pstNext == pstListIt->pstEnd)
	{
		pstListIt->stIterator.bDone = true;
	}
	else
	{
		// Keep a copy of the previous node
		pstListIt->pstPrev = pstListIt->pstNode;

		pstNode = pstListIt->pstNext;
		pstListIt->pstNode = pstNode;
		pstListIt->stIterator.pvRef = STD_LINEAR_ADD(pstNode, pstListIt->szPayloadOffset);
		pstListIt->pstNext = pstNode->pstNext;
	}
}

/**
 * Set up a forward list iterator to step forwards through a forward list
 *
 * @param[in]	pstContainer	List container
 * @param[in]	pstIterator		List iterator
 */
void stdlib_forward_list_forwarditerator_construct(std_container_t* pstContainer, std_iterator_t* pstIterator)
{
	std_forward_list_t* pstList = CONTAINER_TO_FORWARD_LIST(pstContainer);
	std_forward_list_iterator_t* pstListIt = ITERATOR_TO_FORWARDLISTIT(pstIterator);

	if (pstList->pstHead == NULL)
	{
		stdlib_iterator_construct_done(pstIterator);
	}
	else
	{
		std_forward_list_node_t* pstNode = pstList->pstHead;
		void* pvItem = STD_LINEAR_ADD(pstNode, pstList->szPayloadOffset);
		stdlib_iterator_construct(pstIterator, pstContainer, pvItem);

		pstListIt->szLinkSize = pstList->szLinkSize;
		pstListIt->szPayloadOffset = pstList->szPayloadOffset;
		pstListIt->pstBegin = pstNode;
		pstListIt->pstEnd = NULL;
		pstListIt->pstNext = pstNode->pstNext;
		pstListIt->pstNode = pstNode;
		pstListIt->pstPrev = NULL;
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
size_t stdlib_forward_list_insert_after(std_iterator_t* pstIterator, const void* pvBase, size_t szNumItems)
{
	std_container_t* pstContainer = pstIterator->pstContainer;
	std_forward_list_t* pstList = CONTAINER_TO_FORWARD_LIST(pstContainer);
	std_forward_list_node_t* pstNode;
	std_forward_list_node_t* pstItNode;
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
		stdlib_container_relocate_items(pstContainer, pvItem, pvBase, 1);
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
size_t stdlib_forward_list_insert_before(std_iterator_t* pstIterator, const void* pvBase, size_t szNumItems)
{
	std_container_t* pstContainer = pstIterator->pstContainer;
	std_forward_list_t* pstList = CONTAINER_TO_FORWARD_LIST(pstContainer);
	std_forward_list_node_t* pstNode;
	std_forward_list_node_t* pstItNode;
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
		stdlib_container_relocate_items(pstContainer, pvItem, pvBase, 1);
	}

	return i;
}

/**
 * Erase a single item at the current iterator item
 *
 * @param[in]	pstIterator		List iterator
 */
void stdlib_forward_list_erase(std_iterator_t* pstIterator)
{
	std_container_t* pstContainer = pstIterator->pstContainer;
	std_forward_list_t* pstList = CONTAINER_TO_FORWARD_LIST(pstContainer);

	node_disconnect(pstList, pstIterator->pvRef, NULL /* FIXME */);
}

// -------------------------------------------------------------------------

static bool forward_list_default_destruct(const std_item_handler_t* pstItemHandler, void* pvData)
{
	if (pstItemHandler) { /* Unused parameter */ }
	return stdlib_forward_list_destruct((std_container_t*)pvData);
}

const std_item_handler_t std_forward_list_default_itemhandler =
{
	.szElementSize = sizeof(std_forward_list_t),
	.pfn_Destructor = &forward_list_default_destruct,
	.pfn_Relocator = NULL
};
