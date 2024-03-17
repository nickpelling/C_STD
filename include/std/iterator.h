/*
 * std/iterator.h
 *
 *  Created on: 15 Oct 2022
 *      Author: Nick Pelling
 */

#ifndef STD_ITERATOR_H_
#define STD_ITERATOR_H_

#include "std/common.h"

typedef struct std_iterator_s std_iterator_t;

typedef enum
{
	std_iterator_enum_forward,	// i.e. this is a forward iterator
	std_iterator_enum_reverse,	// i.e. this is a reverse iterator

	std_iterator_enum_MAX
} std_iterator_enum_t;

typedef struct
{
	void (*pfn_init)(	std_container_t * pstContainer, std_iterator_t * pstIterator);
	void (*pfn_range)(	std_container_t * pstContainer, std_iterator_t * pstIterator, void * pvBegin, void * pvEnd);
	void (*pfn_next)(	std_iterator_t  * pstIterator);
	void (*pfn_prev)(	std_iterator_t  * pstIterator);
} std_container_iterate_jumptable_t;

// Shared base class for all iterators
struct std_iterator_s
{
	size_t szSizeofItem;
	void * pvBegin;
	void * pvEnd;
	void * pvRef;
	void * pvNext;
	bool bDone;
};

#define STD_ITERATOR_ENUM(ENUM)			uint8_t (*pau8IteratorEnum)[1U + (ENUM)]

// Note: every iterator class should include an std_iterator_enum_t stored as the length of a char[] - 1
#define std_iterator_enum(ITERATOR)		((std_iterator_enum_t)(sizeof(ITERATOR.pau8IteratorEnum[0]) - 1U))

#define STD_ITERATOR_IS_CONST(BOOL)		uint8_t (*pau8IteratorConst)[1U + (BOOL)]

#define std_iterator_is_const(ITERATOR)	((bool)(sizeof(ITERATOR.pau8IteratorConst[0]) - 1U))

// The STD_ITERATOR macro creates a union of five separate things
//	- an untyped base class, that gets passed down to library-side shared calls
//		- Note that this should always contain a std_iterator_t called "stIterator"!
//	- a type smuggle, used to give easy access to an item (TYPE *) cast
#define STD_ITERATOR(BASE, CONTAINER_ENUM, HAS_ENUM, ITERATOR_ENUM, IS_CONST, TYPE)	\
	union										\
	{											\
		BASE 	   		stItBody;				\
		TYPE		*	pstType;				\
		STD_CONTAINER_ENUM_SET(CONTAINER_ENUM);	\
		STD_CONTAINER_HAS_SET(HAS_ENUM);		\
		STD_ITERATOR_ENUM(ITERATOR_ENUM);		\
		STD_ITERATOR_IS_CONST(IS_CONST);		\
	}

#define STD_ITERATORS(ITBASE, TYPE, CONTAINER_ENUM, HAS_ENUM)	\
	STD_ITERATOR(ITBASE, CONTAINER_ENUM, HAS_ENUM, std_iterator_enum_forward, false, TYPE)		*	pstForwardIterator;			\
	STD_ITERATOR(ITBASE, CONTAINER_ENUM, HAS_ENUM, std_iterator_enum_forward, true,  TYPE const)	*	pstForwardConstIterator;	\
	STD_ITERATOR(ITBASE, CONTAINER_ENUM, HAS_ENUM, std_iterator_enum_reverse, false, TYPE)		*	pstReverseIterator;			\
	STD_ITERATOR(ITBASE, CONTAINER_ENUM, HAS_ENUM, std_iterator_enum_reverse, true,  TYPE const)	*	pstReverseConstIterator

// Because containers all use the same type smuggling mechanism for their
// associated iterators, the following macros work the same for ALL of them:
#define std_forward_iterator(CONTAINER)			STD_TYPEOF(CONTAINER.pstForwardIterator[0])
#define std_forward_const_iterator(CONTAINER)	STD_TYPEOF(CONTAINER.pstForwardConstIterator[0])
#define std_reverse_iterator(CONTAINER)			STD_TYPEOF(CONTAINER.pstReverseIterator[0])
#define std_reverse_const_iterator(CONTAINER)	STD_TYPEOF(CONTAINER.pstReverseConstIterator[0])

#define std_forward_iterator_cast(CONTAINER,X)			((std_forward_iterator(CONTAINER))(X))
#define std_forward_const_iterator_cast(CONTAINER,X)	((std_forward_const_iterator(CONTAINER))(X))
#define std_reverse_iterator_cast(CONTAINER,X)			((std_reverse_iterator(CONTAINER))(X))
#define std_reverse_const_iterator_cast(CONTAINER,X)	((std_reverse_const_iterator(CONTAINER))(X))

// Generic operations on a wrapped (and typed) iterator
#define std_iterator_done(IT)		IT.stItBody.stIterator.bDone
#define std_iterator_at(IT)			STD_ITEM_PTR_CAST(IT, IT.stItBody.stIterator.pvRef)[0]

// The generic std_for() macro
//	- instantiates a named iterator within the for-loop scope
//	- initialises that named iterator's fields
//	- steps the named iterator
//	- terminates the iterating when complete
#define std_for(CONTAINER, IT, IT_TYPE)	\
	for (IT_TYPE(CONTAINER) IT, * STD_UNUSED STD_FAKEVAR() = (std_iterator_init(CONTAINER,IT),NULL); \
		!std_iterator_done(IT); \
		std_iterator_next(IT))

#define std_for_range(CONTAINER, IT, IT_TYPE, BEGIN, END)	\
	for (IT_TYPE(CONTAINER) IT, * STD_UNUSED STD_FAKEVAR() = (std_iterator_range(CONTAINER,IT,BEGIN,END),NULL); \
		!iterator_done(IT); \
		std_iterator_next(IT))

#define for_each_forward(CONTAINER, IT)			std_for(CONTAINER, IT, std_forward_iterator)
#define for_each_const_forward(CONTAINER, IT)	std_for(CONTAINER, IT, std_forward_const_iterator)
#define for_each_reverse(CONTAINER, IT)			std_for(CONTAINER, IT, std_reverse_iterator)
#define for_each_const_reverse(CONTAINER, IT)	std_for(CONTAINER, IT, std_reverse_const_iterator)

#define for_each(CONTAINER, IT)			for_each_forward(CONTAINER, IT)
#define for_each_const(CONTAINER, IT)	for_each_const_forward(CONTAINER, IT)

#define for_range_forward(CONTAINER, IT, BEGIN, END)		std_for_range(CONTAINER, IT, std_forward_iterator,			BEGIN, END)
#define for_range_const_forward(CONTAINER, IT, BEGIN, END)	std_for_range(CONTAINER, IT, std_forward_const_iterator,	BEGIN, END)
#define for_range_reverse(CONTAINER, IT, BEGIN, END)		std_for_range(CONTAINER, IT, std_reverse_iterator,			BEGIN, END)
#define for_range_const_reverse(CONTAINER, IT, BEGIN, END)	std_for_range(CONTAINER, IT, std_reverse_const_iterator,	BEGIN, END)

#endif /* STD_ITERATOR_H_ */
