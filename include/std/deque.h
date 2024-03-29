/*
 * std/deque.h
 *
 *  Created on: 17 Oct 2022
 *      Author: Nick Pelling
 */

#ifndef STD_DEQUE_H_
#define STD_DEQUE_H_

#include "std/common.h"
#include "std/item.h"
#include "std/iterator.h"

#define STD_DEQUE_USE_DEFAULT_ITEMS_PER_BUCKET	0

// The std_DEQUE macro creates a union of many separate things
//	- an untyped base class, that gets passed down to library-side shared calls
//	- a type smuggle, used to give easy access to an inner (TYPE *) cast
//	- a const type smuggle, used to give easy access to an inner (const TYPE *) cast
//	- an iterator smuggle, used to give easy access to an associated iterator
//	- a typed comparison function (for sorting)
//	- a typed equals function
#define STD_DEQUE(BASE, ITBASE, TYPE, ENUM, HAS_ENUM, IMPLEMENTS, TEMPNAME)	\
	union TEMPNAME								\
	{											\
		BASE 	   			 stBody;			\
		TYPE			*	pstType;			\
		STD_COMPARE(TYPE const, pfnCompare);	\
		\
		STD_ITERATORS(ITBASE, TYPE, TEMPNAME);	\
		\
		STD_CONTAINER_ENUM_SET(ENUM);			\
		STD_CONTAINER_HAS_SET(HAS_ENUM);		\
		STD_CONTAINER_PAYLOAD_OFFSET_SET(0);	\
		STD_CONTAINER_WRAPPEDITEM_SIZEOF_SET(sizeof(TYPE));	\
		STD_CONTAINER_IMPLEMENTS_SET(IMPLEMENTS); \
	}

typedef struct
{
	std_container_t stContainer;

	// Number of items per bucket
	size_t szItemsPerBucket;

	// Pointer to an allocated linear array of pointers to buckets
	void * * papvBuckets;

	// Number of buckets in the allocated linear array of pointers to buckets
    size_t szNumBuckets;

    // Starting offset within the deque's first bucket
    size_t szStartOffset;
} std_deque_t;

typedef struct
{
	std_iterator_t stIterator;

    size_t szIndex;
    size_t szRangeLen;
} std_deque_iterator_t;

#define STD_DEQUE_DECLARE(T,HAS_ENUM)	STD_DEQUE(std_deque_t, std_deque_iterator_t, T, std_container_enum_deque, HAS_ENUM, std_deque_implements, STD_FAKEVAR())

#define std_deque(T)					STD_DEQUE_DECLARE(T,std_container_has_no_handlers)
#define std_deque_handlers(T,HAS_ENUM)	STD_DEQUE_DECLARE(T,HAS_ENUM)

extern void stdlib_deque_setbucketsize(std_container_t * pstContainer, size_t szBucketSize);

extern bool stdlib_deque_construct(std_container_t* pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas);
extern bool stdlib_deque_destruct(std_container_t* pstContainer);

extern size_t stdlib_deque_push_front(std_container_t * pstContainer, const void* pvBase, size_t szNumItems);
extern size_t stdlib_deque_push_back(std_container_t * pstContainer, const void* pvBase, size_t szNumItems);

extern size_t stdlib_deque_pop_front(std_container_t * pstContainer, void * pvResult, size_t szMaxItems);
extern size_t stdlib_deque_pop_back(std_container_t * pstContainer, void * pvResult, size_t szMaxItems);

extern void * stdlib_deque_at(std_container_t * pstContainer, size_t szIndex);

extern void stdlib_deque_forwarditerator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator);
extern void stdlib_deque_reverseiterator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator);
extern void stdlib_deque_next(std_iterator_t * pstIterator);
extern void stdlib_deque_prev(std_iterator_t * pstIterator);

extern const std_item_handler_t std_deque_default_itemhandler;

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
