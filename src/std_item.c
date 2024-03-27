/*
 * std_item.c
 *
 *  Created on: 22 Oct 2022
 *      Author: Nick Pelling
 */

#include <stdint.h>		// for uint8_t
#include <stddef.h>		// for size_t

#include "std/item.h"

/**
 * Support code to pop an item (destructing it or relocating as appropriate)
 * 
 * @param[in]	eHas			Flags controlling which handlers are attached to a container
 * @param[in]	pstItemHandler	Item handler jumptable (NULL if not needed)
 * @param[out]	pvResult		Empty area of memory to pop item into (can be NULL)
 * @param[in]	pvItem			Item
 * @param[in]	szSize			Size of the item
 */
void std_item_pop(std_container_has_t eHas, const std_item_handler_t* pstItemHandler, void* pvResult, void* pvItem, size_t szSize)
{
	if (pvResult != NULL)
	{
		memcpy(pvResult, pvItem, szSize);
		if (eHas & std_container_has_itemhandler)
		{
			std_item_relocate(pstItemHandler, pvResult, pvItem, szSize);
		}
	}
	else
	{
		if (eHas & std_container_has_itemhandler)
		{
			std_item_destruct(pstItemHandler, pvItem, szSize);
		}
	}
}

/**
 * Construct one or more items using an item handler
 * 
 * @param[in]	pstItemHandler		Item handler
 * @param[in]	pvData				Pre-allocated area of memory for item(s)
 * @param[in]	szNumElements		Number of items to construct
 */
void std_item_construct(const std_item_handler_t* pstItemHandler, void * pvData, size_t szNumElements)
{
	if (pstItemHandler && pstItemHandler->pfn_Constructor && pvData && (szNumElements != 0U))
	{
		uint8_t * pau8Data = pvData;
		for (size_t i = 0U; i < szNumElements; i++)
		{
			(*pstItemHandler->pfn_Constructor)(pstItemHandler, pau8Data);
			pau8Data += pstItemHandler->szElementSize;
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
void std_item_destruct(const std_item_handler_t* pstItemHandler, void* pvData, size_t szNumElements)
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
void std_item_relocate(const std_item_handler_t* pstItemHandler, void* pvNewAddr, const void* pvOldAddr, size_t szTotalSize)
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
