/*
 * std/vector.h

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

#ifndef STD_VECTOR_H_
#define STD_VECTOR_H_

#include "std/common.h"
#include "std/item.h"
#include "std/linear_series.h"
#include "std/iterator.h"

// The STD_VECTOR macro creates a union of many separate things
//	- an untyped base class, that gets passed down to library-side shared calls
//	- a type smuggle, used to give easy access to an inner (TYPE *) cast
//	- a typed comparison function type (e.g. for sorting)
//	- an iterator smuggle, used to give easy access to associated iterators
#define STD_VECTOR(BASE, ITBASE, TYPE, ENUM, HAS_ENUM, IMPLEMENTS, UNIONNAME, TYPEWRAPPER)	\
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

#define STD_VECTOR_FIELDS			\
	size_t szNumAlloced;			\
	void* pvStartAddr				\

typedef struct
{
	std_container_t stContainer;
	size_t szNumAlloced;
	void* pvStartAddr;
} std_vector_t;

typedef	struct
{
	std_iterator_t stIterator;
} std_vector_iterator_t;

// Client-side declaration code

#define STD_VECTOR_DECLARE(T,HAS_ENUM,...)	\
	STD_VECTOR(std_vector_t, std_vector_iterator_t, T, std_container_enum_vector, HAS_ENUM, STD_DEFAULT_PARAMETER(std_vector_implements,__VA_ARGS__), STD_FAKEUNION(), STD_FAKESTRUCT())

#define std_vector(T,...)											STD_VECTOR_DECLARE(T,std_container_has_no_handlers,__VA_ARGS__)
#define std_vector_itemhandler(T,...)								STD_VECTOR_DECLARE(T,std_container_has_itemhandler,__VA_ARGS__)
#define std_vector_memoryhandler(T,...)								STD_VECTOR_DECLARE(T,std_container_has_memoryhandler,__VA_ARGS__)
#define std_vector_memoryhandler_itemhandler(T,...)					STD_VECTOR_DECLARE(T,std_container_has_memoryhandler_itemhandler,__VA_ARGS__)
#define std_vector_lockhandler(T,...)								STD_VECTOR_DECLARE(T,std_container_has_lockhandler,__VA_ARGS__)
#define std_vector_lockhandler_itemhandler(T,...)					STD_VECTOR_DECLARE(T,std_container_has_lockhandler_itemhandler,__VA_ARGS__)
#define std_vector_lockhandler_memoryhandler(T,...)					STD_VECTOR_DECLARE(T,std_container_has_lockhandler_memoryhandler,__VA_ARGS__)
#define std_vector_lockhandler_memoryhandler_itemhandler(T,...)		STD_VECTOR_DECLARE(T,std_container_has_lockhandler_memoryhandler_itemhandler,__VA_ARGS__)

// Library-side (untyped) methods

extern void stdlib_vector_construct(	std_container_t * pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas);
extern bool stdlib_vector_destruct(		std_container_t * pstContainer);	
extern bool stdlib_vector_reserve(		std_container_t * pstContainer, size_t szNewSize);
extern size_t stdlib_vector_push_front(	std_container_t * pstContainer, const std_linear_series_t * pstSeries);
extern size_t stdlib_vector_push_back(	std_container_t * pstContainer, const std_linear_series_t * pstSeries);
extern size_t stdlib_vector_pop_front(	std_container_t * pstContainer, void * pvResult, size_t szMaxItems);
extern size_t stdlib_vector_pop_back(	std_container_t * pstContainer, void * pvResult, size_t szMaxItems);
extern void stdlib_vector_ranged_sort(	std_container_t * pstContainer, size_t szFirst, size_t szLast, pfn_std_compare_t pfn_Compare);
extern void * stdlib_vector_at(std_container_t * pstContainer, size_t szIndex);

extern void stdlib_vector_forwarditerator_seek(std_iterator_t* pstIterator, size_t szIndex);
extern void stdlib_vector_forwarditerator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator, size_t szFirst, size_t szLast);
extern void stdlib_vector_reverseiterator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator, size_t szFirst, size_t szLast);

extern size_t stdlib_vector_heap_insert(std_container_t* pstContainer, const std_linear_series_t* pstSeries, pfn_std_compare_t pfnCompare);

extern const std_item_handler_t std_vector_default_itemhandler;

/**
 * Step a vector iterator forwards in memory
 *
 * @param[in]	pstIterator		Vector iterator
 */
STD_INLINE void stdlib_vector_forwarditerator_next(std_iterator_t* pstIterator)
{
	if (pstIterator->pvNext == pstIterator->pvEnd)
	{
		pstIterator->bDone = true;
	}
	else
	{
		pstIterator->pvRef = pstIterator->pvNext;
		pstIterator->pvNext = STD_LINEAR_ADD(pstIterator->pvRef, pstIterator->szSizeofItem);
	}
}

/**
 * Step a vector iterator backwards in memory
 *
 * @param[in]	pstIterator		Vector iterator
 */
STD_INLINE void stdlib_vector_reverseiterator_next(std_iterator_t* pstIterator)
{
	if (pstIterator->pvNext == pstIterator->pvEnd)
	{
		pstIterator->bDone = true;
	}
	else
	{
		pstIterator->pvRef = pstIterator->pvNext;
		pstIterator->pvNext = STD_LINEAR_SUB(pstIterator->pvRef, pstIterator->szSizeofItem);
	}
}


enum 
{
	std_vector_implements = 
		( std_container_implements_name
		| std_container_implements_construct
		| std_container_implements_destruct
		| std_container_implements_pushpop_front
		| std_container_implements_pushpop_back
		| std_container_implements_at
		| std_container_implements_reserve
		| std_container_implements_ranged_sort
		| std_container_implements_forward_constructnext
		| std_container_implements_forward_seek
		| std_container_implements_reverse_constructnext
		| std_container_implements_default_itemhandler
		| std_container_implements_ranged_iterator	)
};

#define STD_VECTOR_JUMPTABLE \
	.pachContainerName = "vector",						\
	.pfn_construct		= &stdlib_vector_construct,		\
	.pfn_destruct		= &stdlib_vector_destruct,		\
	.pfn_push_front		= &stdlib_vector_push_front,	\
	.pfn_push_back		= &stdlib_vector_push_back,		\
	.pfn_pop_front		= &stdlib_vector_pop_front,		\
	.pfn_pop_back		= &stdlib_vector_pop_back,		\
	.pfn_at				= &stdlib_vector_at,			\
	.pfn_reserve		= &stdlib_vector_reserve,		\
	.pfn_ranged_sort	= &stdlib_vector_ranged_sort,	\
	.astIterators =										\
	{													\
		[std_iterator_enum_forward] =					\
		{												\
			.pfn_construct	= &stdlib_vector_forwarditerator_construct,	\
			.pfn_seek		= &stdlib_vector_forwarditerator_seek,		\
			.pfn_next		= &stdlib_vector_forwarditerator_next,		\
		},												\
		[std_iterator_enum_reverse] =					\
		{												\
			.pfn_construct	= &stdlib_vector_reverseiterator_construct,	\
			.pfn_next		= &stdlib_vector_reverseiterator_next,		\
		}												\
	},													\
	.pstDefaultItemHandler = &std_vector_default_itemhandler

#endif /* STD_VECTOR_H_ */
