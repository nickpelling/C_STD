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

typedef struct
{
	void (*pfn_construct)(	std_container_t * pstContainer, std_iterator_t * pstIterator);
	void (*pfn_range)(	std_container_t * pstContainer, std_iterator_t * pstIterator, void * pvBegin, void * pvEnd);
	void (*pfn_next)(	std_iterator_t  * pstIterator);
	void (*pfn_prev)(	std_iterator_t  * pstIterator);
	size_t (*pfn_insert_after)(std_iterator_t* pstIterator, const void* pvBase, size_t szNumItems);
	size_t (*pfn_insert_before)(std_iterator_t* pstIterator, const void* pvBase, size_t szNumItems);
	void (*pfn_erase)(std_iterator_t* pstIterator);
} std_container_iterate_jumptable_t;

// Shared base class for all iterators
struct std_iterator_s
{
	std_container_t* pstContainer;
	size_t szSizeofItem;
	void * pvBegin;
	void * pvEnd;
	void * pvRef;
	void * pvNext;
	bool bDone;
};

// Macro accessors to store & retrieve the type of an iterator inside its metadata
#define STD_ITERATOR_ENUM_SET(ENUM)			STD_ENUM_ITERATOR_SET(ENUM) pau8IteratorEnum
#define STD_ITERATOR_ENUM_GET(ITERATOR)		STD_ENUM_ITERATOR_GET(ITERATOR.pau8IteratorEnum)

// Macro accessors to store & retrieve the constness of an iterator inside its metadata
#define STD_ITERATOR_IS_CONST_SET(BOOL)		STD_ENUM_BOOL_SET(BOOL)	pau8IteratorConst
#define STD_ITERATOR_IS_CONST_GET(ITERATOR)	STD_ENUM_BOOL_GET(ITERATOR.pau8IteratorConst)

// The STD_ITERATOR macro creates a union of many separate things
//	- an untyped base class, that gets passed down to library-side shared calls
//		- Note that this should always contain a std_iterator_t called "stIterator"!
//	- a type smuggle, used to give easy access to an item (TYPE *) cast
#define STD_ITERATOR(BASE, PARENT, ITERATOR_ENUM, IS_CONST, TYPE)	\
	union										\
	{											\
		BASE 	   		stItBody;				\
		TYPE		*	pstType;				\
		union PARENT *	puParent;				\
		STD_ITERATOR_ENUM_SET(ITERATOR_ENUM);	\
		STD_ITERATOR_IS_CONST_SET(IS_CONST);	\
	}

#define STD_ITERATORS(ITBASE, TYPE, PARENT)	\
	STD_ITERATOR(ITBASE, PARENT, std_iterator_enum_forward, false, TYPE)		*	pstForwardIterator;			\
	STD_ITERATOR(ITBASE, PARENT, std_iterator_enum_forward, true,  TYPE const)	*	pstForwardConstIterator;	\
	STD_ITERATOR(ITBASE, PARENT, std_iterator_enum_reverse, false, TYPE)		*	pstReverseIterator;			\
	STD_ITERATOR(ITBASE, PARENT, std_iterator_enum_reverse, true,  TYPE const)	*	pstReverseConstIterator

STD_INLINE void stdlib_iterator_construct_done(std_iterator_t* pstIterator)
{
	pstIterator->bDone = true;
}

STD_INLINE void stdlib_iterator_construct(std_iterator_t* pstIterator, std_container_t* pstContainer, void* pvRef)
{
	pstIterator->pstContainer	= pstContainer;
	pstIterator->szSizeofItem	= pstContainer->szSizeofItem;
	pstIterator->pvRef			= pvRef;
	pstIterator->bDone			= false;
}

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
#define std_iterator_at(IT)			STD_ITEM_PTR_CAST(IT, IT.stItBody.stIterator.pvRef)

// The container library's generic std_for() macro
//	- locks the container (if the container has a lockhandler)
//	- instantiates a named iterator in the inner for-loop scope
//	- initialises that named iterator's fields
//	- steps the named iterator through the container
//	- terminates the iterating when complete
//	- unlocks the container (if the container has a lockhandler)
// Note: this allows break commands in the innermost (iterator) loop,
// BUT the container will stay locked if you execute a return command
// inside the inner loop. So don't do this!
#define std_for(CONTAINER, IT, IS_CONST, IT_TYPE)	\
	std_container_lock_wrapper(CONTAINER, IS_CONST))	\
		for (IT_TYPE(CONTAINER) IT, * STD_UNUSED STD_FAKEVAR() = (std_iterator_construct(CONTAINER,IT),NULL); \
			!std_iterator_done(IT); \
			std_iterator_next(IT)

#define std_for_range(CONTAINER, IT, IS_CONST, IT_TYPE, BEGIN, END)	\
	std_container_lock_wrapper(CONTAINER, IS_CONST))	\
		for (IT_TYPE(CONTAINER) IT, * STD_UNUSED STD_FAKEVAR() = (std_iterator_range(CONTAINER,IT,BEGIN,END),NULL); \
			!std_iterator_done(IT); \
			std_iterator_next(IT)

#define std_each_forward(CONTAINER, IT)			std_for(CONTAINER, IT, false, std_forward_iterator)
#define std_each_forward_const(CONTAINER, IT)	std_for(CONTAINER, IT, true,  std_forward_const_iterator)
#define std_each_reverse(CONTAINER, IT)			std_for(CONTAINER, IT, false, std_reverse_iterator)
#define std_each_reverse_const(CONTAINER, IT)	std_for(CONTAINER, IT, true,  std_reverse_const_iterator)

#define std_each(CONTAINER, IT)			std_each_forward(CONTAINER, IT)
#define std_each_const(CONTAINER, IT)	std_each_forward_const(CONTAINER, IT)

#define std_for_range_forward(CONTAINER, IT, BEGIN, END)		std_for_range(CONTAINER, IT, false, std_forward_iterator,		BEGIN, END)
#define std_for_range_forward_const(CONTAINER, IT, BEGIN, END)	std_for_range(CONTAINER, IT, true,  std_forward_const_iterator,	BEGIN, END)
#define std_for_range_reverse(CONTAINER, IT, BEGIN, END)		std_for_range(CONTAINER, IT, false, std_reverse_iterator,		BEGIN, END)
#define std_for_range_reverse_const(CONTAINER, IT, BEGIN, END)	std_for_range(CONTAINER, IT, true,  std_reverse_const_iterator,	BEGIN, END)

#endif /* STD_ITERATOR_H_ */
