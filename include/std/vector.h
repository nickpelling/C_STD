/*
 * std/vector.h
 *
 *  Created on: 15 Oct 2022
 *      Author: Nick Pelling
 */

#ifndef STD_VECTOR_H_
#define STD_VECTOR_H_

#include "std/common.h"
#include "std/item.h"
#include "std/iterator.h"

// The STD_VECTOR macro creates a union of many separate things
//	- an untyped base class, that gets passed down to library-side shared calls
//	- a type smuggle, used to give easy access to an inner (TYPE *) cast
//	- a const type smuggle, used to give easy access to an inner (const TYPE *) cast
//	- an iterator smuggle, used to give easy access to an associated iterator
//	- a typed comparison function (for sorting)
//	- a typed equals function
#define STD_VECTOR(BASE, ITBASE, TYPE, ENUM, HAS_ENUM, IMPLEMENTS)	\
	union										\
	{											\
		BASE 	   			 stBody;			\
		TYPE			*	pstType;			\
		TYPE const		*	pstConstType;		\
		STD_COMPARE(const TYPE, pfnCompare);	\
		\
		STD_ITERATORS(ITBASE, TYPE, ENUM, HAS_ENUM, IMPLEMENTS);	\
		\
		STD_CONTAINER_ENUM_SET(ENUM);			\
		STD_CONTAINER_HAS_SET(HAS_ENUM);		\
		STD_CONTAINER_PAYLOAD_OFFSET_SET(0);	\
		STD_CONTAINER_WRAPPEDITEM_SIZEOF_SET(sizeof(TYPE));		\
		STD_CONTAINER_IMPLEMENTS_SET(IMPLEMENTS);		\
	}

typedef struct
{
	std_container_t stContainer;

	size_t szNumAlloced;
	void * pvStartAddr;
} std_vector_t;

typedef	struct
{
	std_iterator_t stIterator;
} std_vector_iterator_t;

// Client-side declaration code

#define STD_VECTOR_DECLARE(T,HAS_ENUM)	STD_VECTOR(std_vector_t, std_vector_iterator_t, T, std_container_enum_vector, HAS_ENUM, std_vector_implements)

#define std_vector(T)					STD_VECTOR_DECLARE(T,std_container_has_no_handlers)
#define std_vector_handlers(T,HAS_ENUM)	STD_VECTOR_DECLARE(T,HAS_ENUM)

// Library-side (untyped) methods

extern bool stdlib_vector_construct(	std_container_t * pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas, const std_container_handlers_t* pstHandlers);
extern bool stdlib_vector_destruct(		std_container_t * pstContainer);	
extern void stdlib_vector_reserve(		std_container_t * pstContainer, size_t szNewSize);
extern void stdlib_vector_fit(			std_container_t * pstContainer);
extern void stdlib_vector_push_front(	std_container_t * pstContainer, const void * pvBase, size_t szNumItems);
extern void stdlib_vector_push_back(	std_container_t * pstContainer, const void * pvBase, size_t szNumItems);
extern size_t stdlib_vector_pop_back(	std_container_t * pstContainer, void * pvResult, size_t szMaxItems);
extern void stdlib_vector_ranged_sort(	std_container_t * pstContainer, size_t szFirst, size_t szLast, pfn_std_compare_t pfn_Compare);
extern void * stdlib_vector_at(std_container_t * pstContainer, int32_t iIndex);

extern void stdlib_vector_forwarditerator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator);
extern void stdlib_vector_forwarditerator_range(std_container_t * pstContainer, std_iterator_t * pstIterator, void *pvBegin, void * pvEnd);
extern void stdlib_vector_reverseiterator_construct(std_container_t * pstContainer, std_iterator_t * pstIterator);
extern void stdlib_vector_reverseiterator_range(std_container_t * pstContainer, std_iterator_t * pstIterator, void *pvBegin, void * pvEnd);
extern void stdlib_vector_iterator_next(std_iterator_t * pstIterator);
extern void stdlib_vector_iterator_prev(std_iterator_t * pstIterator);

enum 
{
	std_vector_implements = 
		( std_container_implements_name
		| std_container_implements_construct
		| std_container_implements_destruct
		| std_container_implements_pushpop_back
		| std_container_implements_at
		| std_container_implements_reserve
		| std_container_implements_fit
		| std_container_implements_ranged_sort
		| std_container_implements_forward_constructnextprev
		| std_container_implements_forward_range
		| std_container_implements_reverse_constructnextprev
		| std_container_implements_reverse_range)
};

#define STD_VECTOR_JUMPTABLE \
	.pachContainerName = "vector",						\
	.pfn_construct		= &stdlib_vector_construct,		\
	.pfn_destruct		= &stdlib_vector_destruct,		\
	.pfn_push_back		= &stdlib_vector_push_back,		\
	.pfn_pop_back		= &stdlib_vector_pop_back,		\
	.pfn_at				= &stdlib_vector_at,			\
	.pfn_reserve		= &stdlib_vector_reserve,		\
	.pfn_fit			= &stdlib_vector_fit,			\
	.pfn_ranged_sort	= &stdlib_vector_ranged_sort,	\
	.astIterators =										\
	{													\
		[std_iterator_enum_forward] =					\
		{												\
			.pfn_construct	= &stdlib_vector_forwarditerator_construct,	\
			.pfn_range		= &stdlib_vector_forwarditerator_range,	\
			.pfn_next		= &stdlib_vector_iterator_next,			\
			.pfn_prev		= &stdlib_vector_iterator_prev			\
		},												\
		[std_iterator_enum_reverse] =					\
		{												\
			.pfn_construct	= &stdlib_vector_reverseiterator_construct,	\
			.pfn_range		= &stdlib_vector_reverseiterator_range,	\
			.pfn_next		= &stdlib_vector_iterator_prev,			\
			.pfn_prev		= &stdlib_vector_iterator_next			\
		}												\
	}
#endif /* STD_VECTOR_H_ */
