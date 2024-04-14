/*
 * std_item.c

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

#include <stdint.h>		// for uint8_t
#include <stddef.h>		// for size_t

#include "std/item.h"
#include "std/container.h"

/**
 * Support code to pop an item (destructing it or relocating as appropriate)
 * 
 * @param[in]	eHas			Flags controlling which handlers are attached to a container
 * @param[in]	pstItemHandler	Item handler jumptable (NULL if not needed)
 * @param[out]	pvResult		Empty area of memory to pop item into (can be NULL)
 * @param[in]	pvItem			Item
 * @param[in]	szSize			Size of the item
 */
void stdlib_item_pop(std_container_has_t eHas, const std_item_handler_t* pstItemHandler, void* pvResult, void* pvItem, size_t szSize)
{
	if (pvResult != NULL)
	{
		memcpy(pvResult, pvItem, szSize);
		if (eHas & std_container_has_itemhandler)
		{
			stdlib_item_relocate(pstItemHandler, pvResult, pvItem, szSize);
		}
	}
	else
	{
		if (eHas & std_container_has_itemhandler)
		{
			stdlib_item_destruct(pstItemHandler, pvItem, szSize);
		}
	}
}

/**
 * Destruct one or more items using an item handler
 *
 * @param[in]	pstItemHandler		Item handler
 * @param[in]	pvData				Pre-allocated area of memory holding item(s)
 * @param[in]	szNumElements		Number of items to destruct
 */
void stdlib_item_destruct(const std_item_handler_t* pstItemHandler, void* pvData, size_t szNumElements)
{
	if (pstItemHandler && pstItemHandler->pfn_Destructor && pvData && (szNumElements != 0U))
	{
		uint8_t* pau8Data = pvData;
		for (size_t i = 0U; i < szNumElements; i++)
		{
			(*pstItemHandler->pfn_Destructor)(pstItemHandler, pau8Data);
			pau8Data += pstItemHandler->szElementSize;
		}
	}
}

/**
 * Relocate one or more items using an item handler (note: items have already been moved!)
 *
 * @param[in]	pstItemHandler		Item handler
 * @param[in]	pvNewAddr			Address of memory now holding item(s)
 * @param[in]	pvOldAddr			Address of memory that used to hold item(s)
 * @param[in]	szNumElements		Number of items to relocate
 */
void stdlib_item_relocate(const std_item_handler_t* pstItemHandler, void* pvNewAddr, const void* pvOldAddr, size_t szTotalSize)
{
	if ((pvOldAddr != NULL) && (pvNewAddr != pvOldAddr) && pstItemHandler && pstItemHandler->pfn_Relocator)
	{
		size_t szSize = pstItemHandler->szElementSize;
		uint8_t* pau8NewAddr = pvNewAddr;
		const uint8_t* pau8OldAddr = pvOldAddr;
		for (size_t i = 0; i < szTotalSize; i += szSize)
		{
			(*pstItemHandler->pfn_Relocator)(pstItemHandler, &pau8NewAddr[i], &pau8OldAddr[i]);
		}
	}
}

/**
 * Relocate a series of items within a container
 *
 * @param[in]	pstContainer
 * @param[in]	pvNewAddr
 * @param[in]	pvOldAddr
 * @param[in]	szNumItems
 */
void stdlib_container_relocate_items(std_container_t* pstContainer, void* pvNewAddr, const void* pvOldAddr, size_t szNumItems)
{
	size_t szSizeofItem = pstContainer->szSizeofItem;
	size_t i;

	if (pstContainer->eHas & std_container_has_itemhandler)
	{
		if (pvNewAddr < pvOldAddr)
		{
			for (i = 0; i < szNumItems; i++, pvNewAddr = STD_LINEAR_ADD(pvNewAddr, szSizeofItem), pvOldAddr = STD_LINEAR_ADD(pvOldAddr, szSizeofItem))
			{
				memcpy(pvNewAddr, pvOldAddr, szSizeofItem);
				stdlib_item_relocate(pstContainer->pstItemHandler, pvNewAddr, pvOldAddr, szSizeofItem);
			}
		}
		else
		{
			pvNewAddr = STD_LINEAR_ADD(pvNewAddr, szSizeofItem * (szNumItems - 1U));
			pvOldAddr = STD_LINEAR_ADD(pvOldAddr, szSizeofItem * (szNumItems - 1U));
			for (i = 0; i < szNumItems; i++, pvNewAddr = STD_LINEAR_SUB(pvNewAddr, szSizeofItem), pvOldAddr = STD_LINEAR_SUB(pvOldAddr, szSizeofItem))
			{
				memcpy(pvNewAddr, pvOldAddr, szSizeofItem);
				stdlib_item_relocate(pstContainer->pstItemHandler, pvNewAddr, pvOldAddr, szSizeofItem);
			}
		}
	}
	else
	{
		memmove(pvNewAddr, pvOldAddr, szSizeofItem * szNumItems);
	}
}
