/*
 * std/item.h
 *
 *  Created on: 21 Oct 2022
 *      Author: Nick Pelling
 */

#ifndef STD_ITEM_H_
#define STD_ITEM_H_

#include <stddef.h>		// for size_t
#include <stdbool.h>	// for bool
#include <string.h>		// for memcpy

#include "std/config.h"
#include "std/enums.h"

typedef struct std_item_handler_s std_item_handler_t;

struct std_item_handler_s
{
	// Size of an individual element
	size_t szElementSize;

	// In-place destructor (optional)
	bool (*pfn_Destructor)(const std_item_handler_t * pstItemHandler, void * pvData);

	// Object relocator (optional)
	void (*pfn_Relocator)(const std_item_handler_t * pstItemHandler, void * pvNewAddr, const void * pvOldAddr);
};

extern void std_item_destruct(const std_item_handler_t* pstItemHandler, void* pvData, size_t szNumElements);
extern void std_item_relocate(const std_item_handler_t* pstItemHandler, void* pvNewAddr, const void* pvOldAddr, size_t szTotalSize);

extern void std_item_pop(std_container_has_t eHas, const std_item_handler_t* pstItemHandler, void* pvResult, void* pvItem, size_t szSize);

#endif /* STD_ITEM_H_ */
