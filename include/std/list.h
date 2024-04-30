/*
 * std/list.h

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

#ifndef STD_LIST_H_
#define STD_LIST_H_

#include "std/common.h"
#include "std/item.h"
#include "std/linear_series.h"
#include "std/iterator.h"

#define STD_LIST_NODE(TYPE,NAME)	\
	struct NAME						\
	{								\
		std_list_node_t	stLink;		\
		TYPE			stPayload;	\
	}

// The STD_LIST macro creates a union of many separate things
//	- an untyped base class, that gets passed down to library-side shared calls
//	- a type smuggle, used to give easy access to an inner (TYPE *) cast
//	- a typed comparison function (for sorting)
//	- an iterator smuggle, used to give easy access to an associated iterator
#define STD_LIST(BASE, ITBASE, TYPE, ENUM, HAS_ENUM, IMPLEMENTS, UNIONNAME, NODENAME, TYPEWRAPPER)		\
	union UNIONNAME										\
	{													\
		STD_TYPE_SET(TYPEWRAPPER,TYPE) * pstWrapper;	\
		STD_LIST_NODE(STD_TYPE_GET(TYPEWRAPPER),NODENAME) * pstLink;	\
		\
		BASE 	   			 stBody;					\
		STD_TYPE_GET(TYPEWRAPPER)	*	pstType;		\
		STD_COMPARE(STD_TYPE_GET(TYPEWRAPPER) const, pfnCompare);	\
		\
		STD_ITERATORS(ITBASE, STD_TYPE_GET(TYPEWRAPPER), UNIONNAME);	\
		\
		STD_CONTAINER_ENUM_SET(ENUM);			\
		STD_CONTAINER_HAS_SET(HAS_ENUM);		\
		STD_CONTAINER_PAYLOAD_OFFSET_SET(STD_OFFSETOF(struct NODENAME, stPayload));	\
		STD_CONTAINER_WRAPPEDITEM_SIZEOF_SET(sizeof(struct NODENAME));	\
		STD_CONTAINER_IMPLEMENTS_SET(IMPLEMENTS); \
	}

typedef struct stListLink_s std_list_node_t;

struct stListLink_s
{
	std_list_node_t * pstPrev;
	std_list_node_t * pstNext;
};

typedef struct
{
	std_container_t stContainer;
	size_t szLinkSize;
	size_t szPayloadOffset;
	std_list_node_t * pstHead;
	std_list_node_t * pstTail;
} std_list_t;

typedef	struct
{
	std_iterator_t stIterator;
	size_t szLinkSize;
	size_t szPayloadOffset;
	std_list_node_t * pstNode;
} std_list_iterator_t;

#define STD_LIST_DECLARE(T,HAS_ENUM,...)	\
	STD_LIST(std_list_t, std_list_iterator_t, T, std_container_enum_list, HAS_ENUM, STD_DEFAULT_PARAMETER(std_list_implements,__VA_ARGS__), STD_FAKEUNION(), STD_FAKESTRUCT(), STD_FAKESTRUCT())

#define std_list(T,...)											STD_LIST_DECLARE(T,std_container_has_no_handlers,__VA_ARGS__)
#define std_list_itemhandler(T,...)								STD_LIST_DECLARE(T,std_container_has_itemhandler,__VA_ARGS__)
#define std_list_memoryhandler(T,...)							STD_LIST_DECLARE(T,std_container_has_memoryhandler,__VA_ARGS__)
#define std_list_memoryhandler_itemhandler(T,...)				STD_LIST_DECLARE(T,std_container_has_memoryhandler_itemhandler,__VA_ARGS__)
#define std_list_lockhandler(T,...)								STD_LIST_DECLARE(T,std_container_has_lockhandler,__VA_ARGS__)
#define std_list_lockhandler_itemhandler(T,...)					STD_LIST_DECLARE(T,std_container_has_lockhandler_itemhandler,__VA_ARGS__)
#define std_list_lockhandler_memoryhandler(T,...)				STD_LIST_DECLARE(T,std_container_has_lockhandler_memoryhandler,__VA_ARGS__)
#define std_list_lockhandler_memoryhandler_itemhandler(T,...)	STD_LIST_DECLARE(T,std_container_has_lockhandler_memoryhandler_itemhandler,__VA_ARGS__)

extern void stdlib_list_construct(std_container_t* pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas);
extern bool stdlib_list_destruct(std_container_t* pstContainer);

extern size_t stdlib_list_push_front(	std_container_t * pstContainer, const std_linear_series_t * pstSeries);
extern size_t stdlib_list_push_back(	std_container_t * pstContainer, const std_linear_series_t * pstSeries);
extern size_t stdlib_list_pop_front(	std_container_t * pstContainer, void * pvResult, size_t szMaxItems);
extern size_t stdlib_list_pop_back(		std_container_t * pstContainer, void * pvResult, size_t szMaxItems);

extern void stdlib_list_forwarditerator_construct(std_container_t* pstContainer, std_iterator_t* pstIterator);
extern void stdlib_list_reverseiterator_construct(std_container_t* pstContainer, std_iterator_t* pstIterator);
extern size_t stdlib_list_push_after(std_iterator_t* pstIterator, const std_linear_series_t* pstSeries);
extern size_t stdlib_list_push_before(std_iterator_t* pstIterator, const std_linear_series_t* pstSeries);
extern void stdlib_list_pop_at(std_iterator_t* pstIterator, void * pvResult);

extern const std_item_handler_t std_list_default_itemhandler;

// Cast a generic iterator to a list iterator, and a list iterator to a generic iterator
#define ITERATOR_TO_LISTIT(IT)			STD_CONTAINER_OF(IT, std_list_iterator_t, stIterator)
#define LISTIT_TO_ITERATOR(LISTIT)		&LISTIT->stIterator

/**
 * Step a list iterator forwards through a list container
 *
 * @param[in]	pstIterator	List iterator
 */
STD_INLINE void stdlib_list_next(std_iterator_t* pstIterator)
{
	std_list_iterator_t* pstListIt = ITERATOR_TO_LISTIT(pstIterator);
	if (pstIterator->pvNext == pstIterator->pvEnd)
	{
		pstListIt->stIterator.bDone = true;
	}
	else
	{
		pstListIt->pstNode = pstIterator->pvNext;
		pstListIt->stIterator.pvRef = STD_LINEAR_ADD(pstListIt->pstNode, pstListIt->szPayloadOffset);
		pstIterator->pvNext = pstListIt->pstNode->pstNext;
	}
}

/**
 * Step a list iterator backwards through a list container
 *
 * @param[in]	pstIterator			List iterator
 */
STD_INLINE void stdlib_list_prev(std_iterator_t* pstIterator)
{
	std_list_iterator_t* pstListIt = ITERATOR_TO_LISTIT(pstIterator);
	if (pstIterator->pvNext == pstIterator->pvEnd)
	{
		pstListIt->stIterator.bDone = true;
	}
	else
	{
		pstListIt->pstNode = pstIterator->pvNext;
		pstListIt->stIterator.pvRef = STD_LINEAR_ADD(pstListIt->pstNode, pstListIt->szPayloadOffset);
		pstIterator->pvNext = pstListIt->pstNode->pstPrev;
	}
}


enum
{
	std_list_implements =
		( std_container_implements_name
		| std_container_implements_construct
		| std_container_implements_destruct
		| std_container_implements_pushpop_front
		| std_container_implements_pushpop_back
		| std_container_implements_forward_constructnextprev
		| std_container_implements_forward_push_after
		| std_container_implements_forward_push_before
		| std_container_implements_forward_pop_at
		| std_container_implements_reverse_constructnextprev
		| std_container_implements_reverse_push_after
		| std_container_implements_reverse_push_before
		| std_container_implements_reverse_pop_at
		| std_container_implements_default_itemhandler)
};

#define STD_LIST_JUMPTABLE \
	.pachContainerName = "list",					\
	.pfn_construct		= &stdlib_list_construct,	\
	.pfn_destruct		= &stdlib_list_destruct,	\
	.pfn_push_front		= &stdlib_list_push_front,	\
	.pfn_push_back		= &stdlib_list_push_back,	\
	.pfn_pop_front		= &stdlib_list_pop_front,	\
	.pfn_pop_back		= &stdlib_list_pop_back,	\
	.astIterators =									\
	{												\
		[std_iterator_enum_forward] =				\
		{											\
			.pfn_construct = &stdlib_list_forwarditerator_construct,	\
			.pfn_next = &stdlib_list_next,			\
			.pfn_prev = &stdlib_list_prev,			\
			.pfn_push_after = &stdlib_list_push_after,		\
			.pfn_push_before = &stdlib_list_push_before,	\
			.pfn_pop_at = &stdlib_list_pop_at,		\
		},											\
		[std_iterator_enum_reverse] =				\
		{											\
			.pfn_construct = &stdlib_list_reverseiterator_construct,	\
			.pfn_next = &stdlib_list_prev,			\
			.pfn_prev = &stdlib_list_next,			\
			.pfn_push_after = &stdlib_list_push_before,	\
			.pfn_push_before = &stdlib_list_push_after,	\
			.pfn_pop_at = &stdlib_list_pop_at,		\
		}											\
	},												\
	.pstDefaultItemHandler = &std_list_default_itemhandler

#endif /* STD_LIST_H_ */
