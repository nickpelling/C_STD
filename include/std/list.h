/*
 * std_list.h
 *
 *  Created on: 16 Oct 2022
 *      Author: Nick Pelling
 */

#ifndef STD_LIST_H_
#define STD_LIST_H_

#include "std/common.h"
#include "std/item.h"
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
//	- a const type smuggle, used to give easy access to an inner (const TYPE *) cast
//	- an iterator smuggle, used to give easy access to an associated iterator
//	- a typed comparison function (for sorting)
//	- a typed equals function
#define STD_LIST(BASE, ITBASE, TYPE, ENUM, HAS_ENUM, IMPLEMENTS)		\
	union										\
	{											\
		BASE 	   			 stBody;			\
		TYPE			*	pstType;			\
		TYPE const		*	pstConstType;		\
		STD_COMPARE(const TYPE, pfnCompare);	\
		\
		STD_LIST_NODE(TYPE,) * pstLink;			\
		\
		STD_ITERATORS(ITBASE, TYPE, ENUM, HAS_ENUM, IMPLEMENTS);	\
		\
		STD_CONTAINER_ENUM_SET(ENUM);			\
		STD_CONTAINER_HAS_SET(HAS_ENUM);		\
		STD_CONTAINER_PAYLOAD_OFFSET_SET(STD_OFFSETOF(STD_TYPEOF(STD_LIST_NODE(TYPE,STD_FAKEVAR())), stPayload));	\
		STD_CONTAINER_WRAPPEDITEM_SIZEOF_SET(sizeof(STD_LIST_NODE(TYPE,STD_FAKEVAR())));	\
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
	size_t szNumItems;
} std_list_t;

typedef	struct
{
	std_iterator_t stIterator;
	size_t szLinkSize;
	size_t szPayloadOffset;
	std_list_node_t * pstBegin;
	std_list_node_t * pstNode;
	std_list_node_t * pstNext;
	std_list_node_t * pstEnd;
} std_list_iterator_t;

#define STD_LIST_DECLARE(T,HAS_ENUM)	STD_LIST(std_list_t, std_list_iterator_t, T, std_container_enum_list, HAS_ENUM, std_list_implements)

#define std_list(T)						STD_LIST_DECLARE(T,std_container_has_no_handlers)
#define std_list_handlers(T,HAS_ENUM)	STD_VECTOR_DECLARE(T,HAS_ENUM)


extern bool stdlib_list_construct(std_container_t* pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas, const std_container_handlers_t* pstHandlers);
extern bool stdlib_list_destruct(std_container_t* pstContainer);

extern void * stdlib_list_push_front(	std_container_t * pstContainer, const void * pvBase, size_t szNumElements);
extern void * stdlib_list_push_back(	std_container_t * pstContainer, const void * pvBase, size_t szNumElements);
extern void * stdlib_list_pop_front(	std_container_t * pstContainer, void * pvResult);
extern void * stdlib_list_pop_back(		std_container_t * pstContainer, void * pvResult);

extern void stdlib_list_forwarditerator_construct(std_container_t* pstContainer, std_iterator_t* pstIterator);
extern void stdlib_list_reverseiterator_construct(std_container_t* pstContainer, std_iterator_t* pstIterator);
extern void stdlib_list_next(std_iterator_t * pstIterator);
extern void stdlib_list_prev(std_iterator_t * pstIterator);

extern void * stdlib_list_front(std_container_t * pstContainer);
extern void * stdlib_list_back(std_container_t * pstContainer);
extern std_list_node_t * stdlib_list_begin(std_container_t * pstContainer);
extern std_list_node_t * stdlib_list_end(std_container_t * pstContainer);
extern std_list_node_t * stdlib_list_rbegin(std_container_t * pstContainer);
extern std_list_node_t * stdlib_list_rend(std_container_t * pstContainer);
extern bool stdlib_list_empty(std_container_t * pstContainer);

enum
{
	std_list_implements =
		( std_container_implements_name
		| std_container_implements_construct
		| std_container_implements_destruct
		| std_container_implements_pushpop_front
		| std_container_implements_pushpop_back
		| std_container_implements_empty
		| std_container_implements_forward_constructnextprev
		| std_container_implements_reverse_constructnextprev)
};

#define STD_LIST_JUMPTABLE \
	.pachContainerName = "list",					\
	.pfn_construct		= &stdlib_list_construct,	\
	.pfn_destruct		= &stdlib_list_destruct,	\
	.pfn_push_front		= &stdlib_list_push_front,	\
	.pfn_push_back		= &stdlib_list_push_back,	\
	.pfn_pop_front		= &stdlib_list_pop_front,	\
	.pfn_pop_back		= &stdlib_list_pop_back,	\
	.pfn_empty			= &stdlib_list_empty,		\
	.astIterators =									\
	{												\
		[std_iterator_enum_forward] =				\
		{											\
			.pfn_construct = &stdlib_list_forwarditerator_construct,	\
			.pfn_next = &stdlib_list_next,			\
			.pfn_prev = &stdlib_list_prev			\
		},											\
		[std_iterator_enum_reverse] =				\
		{											\
			.pfn_construct = &stdlib_list_reverseiterator_construct,	\
			.pfn_next = &stdlib_list_prev,			\
			.pfn_prev = &stdlib_list_next			\
		}											\
	}

#endif /* STD_LIST_H_ */
