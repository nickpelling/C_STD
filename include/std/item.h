/*
 * std/item.h

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
