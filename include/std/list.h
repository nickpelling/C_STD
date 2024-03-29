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
//	- a typed comparison function (for sorting)
//	- an iterator smuggle, used to give easy access to an associated iterator
#define STD_LIST(BASE, ITBASE, TYPE, ENUM, HAS_ENUM, IMPLEMENTS, TEMPNAME, NODENAME)		\
	union TEMPNAME								\
	{											\
		BASE 	   			 stBody;			\
		TYPE			*	pstType;			\
		STD_COMPARE(const TYPE, pfnCompare);	\
		\
		STD_LIST_NODE(TYPE,NODENAME) * pstLink;	\
		\
		STD_ITERATORS(ITBASE, TYPE, TEMPNAME);	\
		\
		STD_CONTAINER_ENUM_SET(ENUM);			\
		STD_CONTAINER_HAS_SET(HAS_ENUM);		\
		STD_CONTAINER_PAYLOAD_OFFSET_SET(sizeof(std_list_node_t));	\
		STD_CONTAINER_WRAPPEDITEM_SIZEOF_SET(sizeof(STD_LIST_NODE(TYPE,STD_FAKEVAR())));	\
		STD_CONTAINER_IMPLEMENTS_SET(IMPLEMENTS); \
	}
// Note: the payload offset should be set to STD_OFFSETOF(struct NODENAME, stPayload),
// but gcc complains like crazy if you do that, and I don't see how to fix this.
// Using sizeof(std_list_node_t) for this should be OK unless you start using item
// types with exotic alignments.

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
	std_list_node_t * pstBegin;
	std_list_node_t * pstNode;
	std_list_node_t * pstNext;
	std_list_node_t * pstEnd;
} std_list_iterator_t;

#define STD_LIST_DECLARE(T,HAS_ENUM)	STD_LIST(std_list_t, std_list_iterator_t, T, std_container_enum_list, HAS_ENUM, std_list_implements, STD_FAKEVAR(), STD_FAKEVAR())

#define std_list(T)											STD_LIST_DECLARE(T,std_container_has_no_handlers)
#define std_list_itemhandler(T)								STD_LIST_DECLARE(T,std_container_has_itemhandler)
#define std_list_memoryhandler(T)							STD_LIST_DECLARE(T,std_container_has_memoryhandler)
#define std_list_memoryhandler_itemhandler(T)				STD_LIST_DECLARE(T,std_container_has_memoryhandler_itemhandler)
#define std_list_lockhandler(T)								STD_LIST_DECLARE(T,std_container_has_lockhandler)
#define std_list_lockhandler_itemhandler(T)					STD_LIST_DECLARE(T,std_container_has_lockhandler_itemhandler)
#define std_list_lockhandler_memoryhandler(T)				STD_LIST_DECLARE(T,std_container_has_lockhandler_memoryhandler)
#define std_list_lockhandler_memoryhandler_itemhandler(T)	STD_LIST_DECLARE(T,std_container_has_lockhandler_memoryhandler_itemhandler)

extern void stdlib_list_construct(std_container_t* pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, std_container_has_t eHas);
extern bool stdlib_list_destruct(std_container_t* pstContainer);

extern size_t stdlib_list_push_front(	std_container_t * pstContainer, const void * pvBase, size_t szNumElements);
extern size_t stdlib_list_push_back(	std_container_t * pstContainer, const void * pvBase, size_t szNumElements);
extern size_t stdlib_list_pop_front(	std_container_t * pstContainer, void * pvResult, size_t szMaxItems);
extern size_t stdlib_list_pop_back(		std_container_t * pstContainer, void * pvResult, size_t szMaxItems);

extern void stdlib_list_forwarditerator_construct(std_container_t* pstContainer, std_iterator_t* pstIterator);
extern void stdlib_list_reverseiterator_construct(std_container_t* pstContainer, std_iterator_t* pstIterator);
extern void stdlib_list_next(std_iterator_t * pstIterator);
extern void stdlib_list_prev(std_iterator_t * pstIterator);
extern size_t stdlib_list_insert_after(std_iterator_t* pstIterator, const void* pvBase, size_t szNumItems);
extern size_t stdlib_list_insert_before(std_iterator_t* pstIterator, const void* pvBase, size_t szNumItems);
extern void stdlib_list_erase(std_iterator_t* pstIterator);

extern const std_item_handler_t std_list_default_itemhandler;

enum
{
	std_list_implements =
		( std_container_implements_name
		| std_container_implements_construct
		| std_container_implements_destruct
		| std_container_implements_pushpop_front
		| std_container_implements_pushpop_back
		| std_container_implements_forward_constructnextprev
		| std_container_implements_forward_insert_after
		| std_container_implements_forward_insert_before
		| std_container_implements_reverse_constructnextprev
		| std_container_implements_reverse_insert_after
		| std_container_implements_reverse_insert_before
		| std_container_implements_erase
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
	.pfn_erase = &stdlib_list_erase,				\
	.astIterators =									\
	{												\
		[std_iterator_enum_forward] =				\
		{											\
			.pfn_construct = &stdlib_list_forwarditerator_construct,	\
			.pfn_next = &stdlib_list_next,			\
			.pfn_prev = &stdlib_list_prev,			\
			.pfn_insert_after = &stdlib_list_insert_after,		\
			.pfn_insert_before = &stdlib_list_insert_before,	\
		},											\
		[std_iterator_enum_reverse] =				\
		{											\
			.pfn_construct = &stdlib_list_reverseiterator_construct,	\
			.pfn_next = &stdlib_list_prev,			\
			.pfn_prev = &stdlib_list_next,			\
			.pfn_insert_after = &stdlib_list_insert_before,	\
			.pfn_insert_before = &stdlib_list_insert_after,	\
		}											\
	},												\
	.pstDefaultItemHandler = &std_list_default_itemhandler

#endif /* STD_LIST_H_ */
