/*
 * std/ring.h

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

#ifndef STD_RING_H_
#define STD_RING_H_

#include "std/common.h"
#include "std/item.h"
#include "std/linear_series.h"
#include "std/iterator.h"

#define STD_RING(BASE, ITBASE, TYPE, ENUM, HAS_ENUM, IMPLEMENTS, UNIONNAME, TYPEWRAPPER)	\
	union UNIONNAME										\
	{													\
		STD_TYPE_SET(TYPEWRAPPER,TYPE) * pstWrapper;	\
		\
		BASE 	   			 stBody;					\
		STD_TYPE_GET(TYPEWRAPPER)	*	pstType;		\
		STD_COMPARE(const STD_TYPE_GET(TYPEWRAPPER), pfnCompare);	\
		\
		STD_ITERATORS(ITBASE, STD_TYPE_GET(TYPEWRAPPER), UNIONNAME);	\
		\
		STD_CONTAINER_ENUM_SET(ENUM);					\
		STD_CONTAINER_HAS_SET(HAS_ENUM);				\
		STD_CONTAINER_PAYLOAD_OFFSET_SET(0);			\
		STD_CONTAINER_WRAPPEDITEM_SIZEOF_SET(sizeof(STD_TYPE_GET(TYPEWRAPPER)));		\
		STD_CONTAINER_IMPLEMENTS_SET(IMPLEMENTS);		\
	}

typedef struct
{
	std_container_t stContainer;
	size_t szNumAlloced;
	void* pvStartAddr;
	size_t szStartOffset;		// Offset of start of ring data within vector
} std_ring_t;

typedef	struct
{
	std_iterator_t stIterator;
	void * pvRingStart;
	void * pvRingEnd;
} std_ring_iterator_t;

#define STD_RING_DECLARE(T,HAS_ENUM,...)	\
	STD_RING(std_ring_t, std_ring_iterator_t, T, std_container_enum_ring, HAS_ENUM, STD_DEFAULT_PARAMETER(std_ring_implements,__VA_ARGS__), STD_FAKEVAR(), STD_FAKEVAR())

#define std_ring(T,...)											STD_RING_DECLARE(T,std_container_has_no_handlers,__VA_ARGS__)
#define std_ring_itemhandler(T,...)								STD_RING_DECLARE(T,std_container_has_itemhandler,__VA_ARGS__)
#define std_ring_memoryhandler(T,...)							STD_RING_DECLARE(T,std_container_has_memoryhandler,__VA_ARGS__)
#define std_ring_memoryhandler_itemhandler(T,...)				STD_RING_DECLARE(T,std_container_has_memoryhandler_itemhandler,__VA_ARGS__)
#define std_ring_lockhandler(T,...)								STD_RING_DECLARE(T,std_container_has_lockhandler,__VA_ARGS__)
#define std_ring_lockhandler_itemhandler(T,...)					STD_RING_DECLARE(T,std_container_has_lockhandler_itemhandler,__VA_ARGS__)
#define std_ring_lockhandler_memoryhandler(T,...)				STD_RING_DECLARE(T,std_container_has_lockhandler_memoryhandler,__VA_ARGS__)
#define std_ring_lockhandler_memoryhandler_itemhandler(T,...)	STD_RING_DECLARE(T,std_container_has_lockhandler_memoryhandler_itemhandler,__VA_ARGS__)

extern void stdlib_ring_construct(std_container_t* pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas);
extern bool stdlib_ring_destruct(std_container_t* pstContainer);
extern bool stdlib_ring_reserve(std_container_t* pstContainer, size_t szNewSize);
extern size_t stdlib_ring_push_front(std_container_t* pstContainer, std_linear_series_t* pstSeries);
extern size_t stdlib_ring_push_back(std_container_t* pstContainer, std_linear_series_t* pstSeries);
extern size_t stdlib_ring_pop_front(std_container_t* pstContainer, void* pvResult, size_t szMaxItems);
extern size_t stdlib_ring_pop_back(std_container_t* pstContainer, void* pvResult, size_t szMaxItems);
extern void* stdlib_ring_at(std_container_t* pstContainer, size_t szIndex);

extern void stdlib_ring_forwarditerator_construct(std_container_t* pstContainer, std_iterator_t* pstIterator);
extern void stdlib_ring_reverseiterator_construct(std_container_t* pstContainer, std_iterator_t* pstIterator);
extern void stdlib_ring_iterator_next(std_iterator_t* pstIterator);
extern void stdlib_ring_iterator_prev(std_iterator_t* pstIterator);

extern const std_item_handler_t std_ring_default_itemhandler;

enum
{
	std_ring_implements =
		( std_container_implements_name
		| std_container_implements_construct
		| std_container_implements_destruct
		| std_container_implements_pushpop_front
		| std_container_implements_pushpop_back
		| std_container_implements_at
		| std_container_implements_reserve
		| std_container_implements_forward_constructnextprev
		| std_container_implements_reverse_constructnextprev
		| std_container_implements_default_itemhandler )
};

#define STD_RING_JUMPTABLE \
	.pachContainerName = "ring",						\
	.pfn_construct		= &stdlib_ring_construct,		\
	.pfn_destruct		= &stdlib_ring_destruct,		\
	.pfn_push_front		= &stdlib_ring_push_front,		\
	.pfn_push_back		= &stdlib_ring_push_back,		\
	.pfn_pop_front		= &stdlib_ring_pop_front,		\
	.pfn_pop_back		= &stdlib_ring_pop_back,		\
	.pfn_at				= &stdlib_ring_at,				\
	.pfn_reserve		= &stdlib_ring_reserve,			\
	.astIterators =										\
	{													\
		[std_iterator_enum_forward] =					\
		{												\
			.pfn_construct	= &stdlib_ring_forwarditerator_construct,	\
			.pfn_next		= &stdlib_ring_iterator_next,				\
			.pfn_prev		= &stdlib_ring_iterator_prev				\
		},												\
		[std_iterator_enum_reverse] =					\
		{												\
			.pfn_construct	= &stdlib_ring_reverseiterator_construct,	\
			.pfn_next		= &stdlib_ring_iterator_prev,				\
			.pfn_prev		= &stdlib_ring_iterator_next				\
		}												\
	},													\
	.pstDefaultItemHandler = &std_ring_default_itemhandler

#endif /* STD_RING_H_ */
