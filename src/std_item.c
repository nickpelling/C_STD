/*
 * std_item.c
 *
 *  Created on: 22 Oct 2022
 *      Author: Nick Pelling
 */

#include <stdint.h>		// for uint8_t
#include <stddef.h>		// for size_t

#include "std/item.h"

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

void std_item_relocate(const std_item_handler_t* pstItemHandler, void* pvNewAddr, const void* pvOldAddr, size_t szTotalSize)
{
	if ((pvNewAddr != pvOldAddr) && pstItemHandler && pstItemHandler->pfn_Relocator)
	{
		size_t szSize = pstItemHandler->szElementSize;
		uint8_t* pau8NewAddr = pvNewAddr;
		const uint8_t* pau8OldAddr = pvOldAddr;
		size_t szElementSize = pstItemHandler->szElementSize;
		int iStep = szElementSize;
		if (pau8NewAddr > pau8OldAddr)
		{
			pau8NewAddr += szSize - iStep;
			pau8OldAddr += szSize - iStep;
			iStep = -iStep;
		}
		for (size_t i = 0; i < szSize; i += szElementSize)
		{
			(*pstItemHandler->pfn_Relocator)(pstItemHandler, pau8NewAddr, pau8OldAddr);
			pau8OldAddr += iStep;
			pau8NewAddr += iStep;
		}
	}
}
