/*
 * std/deque.h

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

#ifndef STD_DEQUE_H_
#define STD_DEQUE_H_

#include "std/common.h"
#include "std/item.h"
#include "std/linear_series.h"
#include "std/iterator.h"

#define STD_DEQUE_USE_DEFAULT_ITEMS_PER_BUCKET	0

// The std_DEQUE macro creates a union of many separate things
//	- an untyped base class, that gets passed down to library-side shared calls
//	- a type smuggle, used to give easy access to an inner (TYPE *) cast
//	- a const type smuggle, used to give easy access to an inner (const TYPE *) cast
//	- an iterator smuggle, used to give easy access to an associated iterator
//	- a typed comparison function (for sorting)
//	- a typed equals function
#define STD_DEQUE(BASE, ITBASE, TYPE, ENUM, HAS_ENUM, IMPLEMENTS, UNIONNAME, TYPEWRAPPER)	\
	union UNIONNAME										\
	{													\
		STD_TYPE_SET(TYPEWRAPPER,TYPE) * pstWrapper;	\
		\
		BASE 	   			 stBody;					\
		STD_TYPE_GET(TYPEWRAPPER)	*	pstType;		\
		STD_COMPARE(STD_TYPE_GET(TYPEWRAPPER) const, pfnCompare);	\
		\
		STD_ITERATORS(ITBASE, STD_TYPE_GET(TYPEWRAPPER), UNIONNAME);	\
		\
		STD_CONTAINER_ENUM_SET(ENUM);					\
		STD_CONTAINER_HAS_SET(HAS_ENUM);				\
		STD_CONTAINER_PAYLOAD_OFFSET_SET(0);			\
		STD_CONTAINER_WRAPPEDITEM_SIZEOF_SET(sizeof(STD_TYPE_GET(TYPEWRAPPER)));	\
		STD_CONTAINER_IMPLEMENTS_SET(IMPLEMENTS);		\
	}

#define STD_DEQUE_FIELDS				\
	/* Number of items per bucket */	\
	size_t szItemsPerBucket;			\
										\
	/* Pointer to an allocated linear array of pointers to buckets */	\
	void** papvBuckets;					\
										\
	/* Number of buckets in the allocated linear array of pointers to buckets */	\
	size_t szNumBuckets;				\
										\
	/* Starting offset within the deque's first bucket	*/	\
	size_t szStartOffset

typedef struct
{
	std_container_t stContainer;
	STD_DEQUE_FIELDS;
} std_deque_t;

typedef struct
{
	std_iterator_t stIterator;

    size_t szIndex;
	size_t szRangeEnd;
	void * pvBucketStart;
	void * pvBucketEnd;
} std_deque_iterator_t;

#define STD_DEQUE_DECLARE(T,HAS_ENUM,...)	\
			STD_DEQUE(std_deque_t, std_deque_iterator_t, T, std_container_enum_deque, HAS_ENUM, STD_DEFAULT_PARAMETER(std_deque_implements,__VA_ARGS__), STD_FAKEUNION(), STD_FAKESTRUCT())

#define std_deque(T,...)											STD_DEQUE_DECLARE(T,std_container_has_no_handlers,__VA_ARGS__)
#define std_deque_itemhandler(T,...)								STD_DEQUE_DECLARE(T,std_container_has_itemhandler,__VA_ARGS__)
#define std_deque_memoryhandler(T,...)								STD_DEQUE_DECLARE(T,std_container_has_memoryhandler,__VA_ARGS__)
#define std_deque_memoryhandler_itemhandler(T,...)					STD_DEQUE_DECLARE(T,std_container_has_memoryhandler_itemhandler,__VA_ARGS__)
#define std_deque_lockhandler(T,...)								STD_DEQUE_DECLARE(T,std_container_has_lockhandler,__VA_ARGS__)
#define std_deque_lockhandler_itemhandler(T,...)					STD_DEQUE_DECLARE(T,std_container_has_lockhandler_itemhandler,__VA_ARGS__)
#define std_deque_lockhandler_memoryhandler(T,...)					STD_DEQUE_DECLARE(T,std_container_has_lockhandler_memoryhandler,__VA_ARGS__)
#define std_deque_lockhandler_memoryhandler_itemhandler(T,...)		STD_DEQUE_DECLARE(T,std_container_has_lockhandler_memoryhandler_itemhandler,__VA_ARGS__)

extern void stdlib_deque_setbucketsize(std_container_t * pstContainer, size_t szBucketSize);

extern void stdlib_deque_construct(std_container_t* pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas);
extern bool stdlib_deque_destruct(std_container_t* pstContainer);

extern size_t stdlib_deque_push_front(std_container_t * pstContainer, const std_linear_series_t * pstSeries);
extern size_t stdlib_deque_push_back( std_container_t * pstContainer, const std_linear_series_t * pstSeries);

extern size_t stdlib_deque_pop_front(std_container_t * pstContainer, void * pvResult, size_t szMaxItems);
extern size_t stdlib_deque_pop_back( std_container_t * pstContainer, void * pvResult, size_t szMaxItems);

extern void * stdlib_deque_at(std_container_t * pstContainer, size_t szIndex);

extern void stdlib_deque_forwarditerator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator, size_t szFirst, size_t szLast);
extern void stdlib_deque_reverseiterator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator, size_t szFirst, size_t szLast);

extern const std_item_handler_t std_deque_default_itemhandler;

#define CONTAINER_TO_DEQUE(CONTAINER)	STD_CONTAINER_OF(CONTAINER, std_deque_t, stContainer)
#define DEQUE_TO_CONTAINER(DEQUE)		&DEQUE->stContainer

#define ITERATOR_TO_DEQUEIT(IT)			STD_CONTAINER_OF(IT, std_deque_iterator_t, stIterator)
#define DEQUEIT_TO_ITERATOR(DEQUEIT)	&DEQUEIT->stIterator

/**
 *
 */
STD_INLINE void stdlib_deque_next(std_iterator_t* pstIterator)
{
	std_deque_iterator_t* pstDequeIt = ITERATOR_TO_DEQUEIT(pstIterator);
	std_container_t* pstContainer = pstDequeIt->stIterator.pstContainer;
	std_deque_t* pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	size_t szIndex;
	size_t szQuotient;

	szIndex = pstDequeIt->szIndex;
	if (szIndex == pstDequeIt->szRangeEnd)
	{
		pstDequeIt->stIterator.bDone = true;
	}
	else
	{
		pstDequeIt->szIndex = ++szIndex;

		pstIterator->pvRef = STD_LINEAR_ADD(pstIterator->pvRef, pstIterator->szSizeofItem);
		if (pstIterator->pvRef >= pstDequeIt->pvBucketEnd)
		{
			szIndex += pstDeque->szStartOffset;
			szQuotient = szIndex / pstDeque->szItemsPerBucket;

			pstDequeIt->pvBucketStart = pstDeque->papvBuckets[szQuotient];
			pstDequeIt->pvBucketEnd = STD_LINEAR_ADD(pstDequeIt->pvBucketStart, pstIterator->szSizeofItem * pstDeque->szItemsPerBucket);

			pstIterator->pvRef = pstDequeIt->pvBucketStart;
		}
	}
}

/**
 *
 */
STD_INLINE void stdlib_deque_prev(std_iterator_t* pstIterator)
{
	std_deque_iterator_t* pstDequeIt = ITERATOR_TO_DEQUEIT(pstIterator);
	std_container_t* pstContainer = pstIterator->pstContainer;
	std_deque_t* pstDeque = CONTAINER_TO_DEQUE(pstContainer);
	size_t szIndex;
	size_t szQuotient;

	szIndex = pstDequeIt->szIndex;
	if (szIndex == pstDequeIt->szRangeEnd)
	{
		pstDequeIt->stIterator.bDone = true;
	}
	else
	{
		pstDequeIt->szIndex = --szIndex;

		pstIterator->pvRef = STD_LINEAR_SUB(pstIterator->pvRef, pstIterator->szSizeofItem);
		if (pstIterator->pvRef < pstDequeIt->pvBucketStart)
		{
			szIndex += pstDeque->szStartOffset;
			szQuotient = szIndex / pstDeque->szItemsPerBucket;

			pstDequeIt->pvBucketStart = pstDeque->papvBuckets[szQuotient];
			pstDequeIt->pvBucketEnd = STD_LINEAR_ADD(pstDequeIt->pvBucketStart, pstIterator->szSizeofItem * pstDeque->szItemsPerBucket);

			pstIterator->pvRef = STD_LINEAR_SUB(pstDequeIt->pvBucketEnd, pstIterator->szSizeofItem);
		}
	}
}



enum
{
	std_deque_implements =
		( std_container_implements_name
		| std_container_implements_construct
		| std_container_implements_destruct
		| std_container_implements_pushpop_front
		| std_container_implements_pushpop_back
		| std_container_implements_at
		| std_container_implements_forward_constructnextprev
		| std_container_implements_reverse_constructnextprev
		| std_container_implements_default_itemhandler)
};

#define STD_DEQUE_JUMPTABLE								\
	.pachContainerName = "deque",						\
	.pfn_construct		= &stdlib_deque_construct,		\
	.pfn_destruct		= &stdlib_deque_destruct,		\
	.pfn_push_front		= &stdlib_deque_push_front,		\
	.pfn_push_back		= &stdlib_deque_push_back,		\
	.pfn_pop_front		= &stdlib_deque_pop_front,		\
	.pfn_pop_back		= &stdlib_deque_pop_back,		\
	.pfn_at				= &stdlib_deque_at,				\
	.astIterators =										\
	{													\
		[std_iterator_enum_forward] =					\
		{												\
			.pfn_construct = &stdlib_deque_forwarditerator_construct,	\
			.pfn_next = &stdlib_deque_next,				\
			.pfn_prev = &stdlib_deque_prev				\
		},												\
		[std_iterator_enum_reverse] =					\
		{												\
			.pfn_construct = &stdlib_deque_reverseiterator_construct,	\
			.pfn_next = &stdlib_deque_prev,				\
			.pfn_prev = &stdlib_deque_next				\
		}												\
	},													\
	.pstDefaultItemHandler = &std_deque_default_itemhandler

#endif /* STD_DEQUE_H_ */
