/*
 * std/container.h
 *
 *  Created on: 5 Nov 2022
 *      Author: Nick Pelling
 */

#ifndef STD_CONTAINER_H_
#define STD_CONTAINER_H_

#include "std/iterator.h"
#include "std/lock.h"

#include "std/vector.h"
#include "std/list.h"
#include "std/deque.h"
#include "std/queue.h"
#include "std/priority_queue.h"
#include "std/set.h"
#include "std/stack.h"

#define CONTAINER_TIMEOUT_DEFAULT	500		// FIXME (should probably move this inside container!?)

#define STD_CONTAINER_CALL(CONTAINER_INDEX,PTRFUNC)	\
		(*std_container_jumptable_array[CONTAINER_INDEX].PTRFUNC)

#define STD_ITERATOR_CALL(CONTAINER_INDEX,ITERATOR_INDEX,PTRFUNC) \
		(*std_container_jumptable_array[CONTAINER_INDEX].astIterators[ITERATOR_INDEX].PTRFUNC)

#define STD_CONTAINER_CALL_EXISTS(V,IMPLEMENTS)	\
			STD_EXPR_ASSERT(STD_CONTAINER_IMPLEMENTS_GET(V) & std_container_implements_ ## IMPLEMENTS, \
					STD_CONCAT(IMPLEMENTS ## _is_not_implemented_for_this_type_of_container_,__COUNTER__) )

#define STD_CONTAINER_ENUM_GET_AND_CHECK(V,IMPLEMENTS)	\
	(STD_CONTAINER_CALL_EXISTS(V,IMPLEMENTS), STD_CONTAINER_ENUM_GET(V))

#define STD_ITERATOR_IMPLEMENTS_GET(INDEX,IMPLEMENTS)	\
			((INDEX == std_iterator_enum_forward) ? std_container_implements_forward_ ## IMPLEMENTS : std_container_implements_reverse_ ## IMPLEMENTS)

#define STD_ITERATOR_CALL_EXISTS(IT, IMPLEMENTS) \
			STD_EXPR_ASSERT(STD_CONTAINER_IMPLEMENTS_GET(IT) & STD_ITERATOR_IMPLEMENTS_GET(STD_ITERATOR_ENUM_GET(IT),IMPLEMENTS), \
					STD_CONCAT(IMPLEMENTS ## _is_not_implemented_for_this_type_of_iterator_,__COUNTER__) )

#define STD_ITERATOR_ENUM_GET_AND_CHECK(IT,IMPLEMENTS) \
	(STD_ITERATOR_CALL_EXISTS(IT,IMPLEMENTS), STD_ITERATOR_ENUM_GET(IT))

typedef struct
{
	const char * const pachContainerName;

	bool	(* const pfn_construct)		(std_container_t * pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset,
											std_container_has_t eHas, const std_container_handlers_t * pstHandlers);
	void	(* const pfn_reserve)		(std_container_t * pstContainer, size_t szNewSize);
	void	(* const pfn_fit)			(std_container_t * pstContainer);
	void 	(* const pfn_push_front)	(std_container_t * pstContainer, const void * pvBase, size_t szNumElements);
	void 	(* const pfn_push_back)		(std_container_t * pstContainer, const void * pvBase, size_t szNumElements);
	void 	(* const pfn_push)			(std_container_t * pstContainer, const void * pvBase, size_t szNumElements);
	size_t	(* const pfn_pop_front)		(std_container_t * pstContainer, void * pvResult, size_t szMaxItems);
	size_t	(* const pfn_pop_back)		(std_container_t * pstContainer, void * pvResult, size_t szMaxItems);
	size_t	(* const pfn_pop)			(std_container_t * pstContainer, void * pvResult, size_t szMaxItems);
	void	(* const pfn_range) 		(std_container_t * pstContainer, void * pvBegin, void * pvEnd, std_iterator_t * pstIterator);
	void	(* const pfn_ranged_sort)	(std_container_t * pstContainer, size_t szFirst, size_t szLast, pfn_std_compare_t pfn_Compare);
	void *	(* const pfn_at)			(std_container_t * pstContainer, int32_t iIndex);
	bool	(* const pfn_empty)			(std_container_t * pstContainer);
	bool	(* const pfn_destruct)		(std_container_t * pstContainer);

	std_container_iterate_jumptable_t	astIterators[std_iterator_enum_MAX];

} std_container_jumptable_t;

static const std_container_jumptable_t std_container_jumptable_array[std_container_enum_MAX] =
{
	[std_container_enum_deque]			= { STD_DEQUE_JUMPTABLE },
	[std_container_enum_list]			= { STD_LIST_JUMPTABLE },
	[std_container_enum_priority_queue]	= { STD_PRIORITYQUEUE_JUMPTABLE },
	[std_container_enum_queue]			= { STD_QUEUE_JUMPTABLE },
	[std_container_enum_set]			= { STD_SET_JUMPTABLE },
	[std_container_enum_stack]			= { STD_STACK_JUMPTABLE },
	[std_container_enum_vector]			= { STD_VECTOR_JUMPTABLE },
};

inline const char* std_container_name_get(std_container_enum_t eContainer, std_container_implements_t eImplements)
{
	if (eImplements & std_container_implements_name)
	{
		return std_container_jumptable_array[eContainer].pachContainerName;
	}
	return "(Unnamed container)";
}

inline bool std_container_call_construct(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas,
				size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset, const std_container_handlers_t * pstHandlers)
{
	return STD_CONTAINER_CALL(eContainer, pfn_construct)(pstContainer, szSizeof, szWrappedSizeof, szPayloadOffset, eHas, pstHandlers);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline std_lock_state_t std_container_lock(std_container_t* pstContainer, std_container_has_t eHas, bool bReadOnly)
{
	if (eHas & std_container_has_lockhandler)
	{
		return (bReadOnly)
			? std_lock_for_reading(pstContainer->pstLockHandler, pstContainer->phLock, CONTAINER_TIMEOUT_DEFAULT)
			: std_lock_for_writing(pstContainer->pstLockHandler, pstContainer->phLock, CONTAINER_TIMEOUT_DEFAULT);
	}
	return e_std_lock_NoRestoreNeeded;
}

#define std_container_lock_for_reading(CONTAINER,HAS)	std_container_lock(CONTAINER,HAS,true)
#define std_container_lock_for_writing(CONTAINER,HAS)	std_container_lock(CONTAINER,HAS,false)

inline void std_container_lock_restore(std_container_t* pstContainer, std_container_has_t eHas, std_lock_state_t eOldState)
{
	if (	(eHas & std_container_has_lockhandler)
		&&	(eOldState != e_std_lock_NoRestoreNeeded)	)
	{
		std_lock_update(pstContainer->pstLockHandler, pstContainer->phLock, eOldState, CONTAINER_TIMEOUT_DEFAULT);
	}
}

// Wrap an untyped container lock/unlock for-loop around the actions that follow
// Note: this must be wrapped by a for(....)!
#define STD_CONTAINER_LOCK_WRAPPER(CONTAINER,HAS,READONLY,VARNAME)	\
	std_lock_state_t VARNAME = std_container_lock(CONTAINER, HAS, READONLY);	\
		VARNAME != e_std_lock_Invalid;	\
		VARNAME = (std_container_lock_restore(CONTAINER, HAS, VARNAME), e_std_lock_Invalid)

// Wrap a typed container lock/unlock for-loop around the actions that follow
// Note: this must be wrapped by a for(....)!
#define std_container_lock_wrapper(CONTAINER,READONLY)	\
	STD_CONTAINER_LOCK_WRAPPER(&CONTAINER.stBody.stContainer, STD_CONTAINER_HAS_GET(CONTAINER), READONLY, STD_FAKEVAR())

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline bool std_container_call_destruct(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	return STD_CONTAINER_CALL(eContainer, pfn_destruct)(pstContainer);
}

inline void std_container_call_reserve(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, size_t szNewSize)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	STD_CONTAINER_CALL(eContainer, pfn_reserve)(pstContainer, szNewSize);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

inline void std_container_call_fit(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	STD_CONTAINER_CALL(eContainer, pfn_fit)(pstContainer);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

inline void std_container_call_push_front(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, const void* pvBase, size_t szNumElements)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	STD_CONTAINER_CALL(eContainer, pfn_push_front)(pstContainer, pvBase, szNumElements);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

inline void std_container_call_push_back(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, const void * pvBase, size_t szNumElements)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	STD_CONTAINER_CALL(eContainer, pfn_push_back)(pstContainer, pvBase, szNumElements);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

inline void std_container_call_push(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, const void* pvBase, size_t szNumElements)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	STD_CONTAINER_CALL(eContainer, pfn_push)(pstContainer, pvBase, szNumElements);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

inline size_t std_container_call_pop_front(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, void* pvResult, size_t szMaxItems)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	size_t szNum = STD_CONTAINER_CALL(eContainer, pfn_pop_front)(pstContainer, pvResult, szMaxItems);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return szNum;
}

inline size_t std_container_call_pop_back(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, void* pvResult, size_t szMaxItems)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	size_t szNum = STD_CONTAINER_CALL(eContainer, pfn_pop_back)(pstContainer, pvResult, szMaxItems);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return szNum;
}

inline size_t std_container_call_pop(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, void* pvResult, size_t szMaxItems)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	size_t szNum = STD_CONTAINER_CALL(eContainer, pfn_pop)(pstContainer, pvResult, szMaxItems);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return szNum;
}

inline void std_container_call_range(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, void* pvBegin, void* pvEnd, std_iterator_t* pstIterator)
{
	std_lock_state_t eOldState = std_container_lock_for_reading(pstContainer, eHas);
	STD_CONTAINER_CALL(eContainer, pfn_range)(pstContainer, pvBegin, pvEnd, pstIterator);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

inline void std_container_call_ranged_sort(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, size_t szFirst, size_t szLast, pfn_std_compare_t pfn_Compare)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	STD_CONTAINER_CALL(eContainer, pfn_ranged_sort)(pstContainer, szFirst, szLast, pfn_Compare);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

inline void* std_container_call_at(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, int32_t iIndex)
{
	std_lock_state_t eOldState = std_container_lock_for_reading(pstContainer, eHas);
	void* pvPtr = STD_CONTAINER_CALL(eContainer, pfn_at)(pstContainer, iIndex);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return pvPtr;
}

inline bool std_container_call_empty(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas)
{
	std_lock_state_t eOldState = std_container_lock_for_reading(pstContainer, eHas);
	bool bResult = STD_CONTAINER_CALL(eContainer, pfn_empty)(pstContainer);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return bResult;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void std_iterator_call_construct(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, std_iterator_enum_t eIterator, std_iterator_t* pstIterator)
{
	pstIterator->pstContainer = pstContainer;
	pstIterator->szSizeofItem = pstContainer->szSizeofItem;
	STD_ITERATOR_CALL(eContainer, eIterator, pfn_construct)(pstContainer, pstIterator);
}

inline void std_iterator_call_range(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, std_iterator_enum_t eIterator, std_iterator_t* pstIterator, void* pvBegin, void* pvEnd)
{
	STD_ITERATOR_CALL(eContainer, eIterator, pfn_range)(pstContainer, pstIterator, pvBegin, pvEnd);
}

inline void std_iterator_call_next(std_iterator_t* pstIterator, std_container_enum_t eContainer, std_container_has_t eHas, std_iterator_enum_t eIterator)
{
	STD_ITERATOR_CALL(eContainer, eIterator, pfn_next)(pstIterator);
}

inline void std_iterator_call_prev(std_iterator_t* pstIterator, std_container_enum_t eContainer, std_container_has_t eHas, std_iterator_enum_t eIterator)
{
	STD_ITERATOR_CALL(eContainer, eIterator, pfn_prev)(pstIterator);
}

inline void* std_container_item_construct(std_container_t* pstContainer, std_container_has_t eHas, size_t szSize)
{
	void * pvPtr = std_memoryhandler_malloc(pstContainer->pstMemoryHandler, eHas, szSize);
	if (eHas & std_container_has_itemhandler)
	{
		std_item_construct(pstContainer->pstItemHandler, pvPtr, 1U);
	}
	return pvPtr;
}

inline void std_container_item_destruct(std_container_t* pstContainer, std_container_has_t eHas, void *pvPtr)
{
	if (eHas & std_container_has_itemhandler)
	{
		std_item_destruct(pstContainer->pstItemHandler, pvPtr, 1U);
	}
	std_memoryhandler_free(pstContainer->pstMemoryHandler, eHas, pvPtr);
}
// Client-side (typed) methods

#define std_container_name(V)	std_container_name_get(STD_CONTAINER_ENUM_GET_AND_CHECK(V,name), STD_CONTAINER_IMPLEMENTS_GET(V))

#define std_at(V,INDEX)			STD_ITEM_PTR_CAST(V, std_container_call_at(&V.stBody.stContainer,  	 STD_CONTAINER_ENUM_GET_AND_CHECK(V,at), STD_CONTAINER_HAS_GET(V), INDEX))[0]
#define std_front(V)			STD_ITEM_PTR_CAST(V, std_container_call_front(&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,front), STD_CONTAINER_HAS_GET(V)))
#define std_back(V)				STD_ITEM_PTR_CAST(V, std_container_call_back((&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,back), STD_CONTAINER_HAS_GET(V)))
#define std_data(V)				std_front(V)[0]

#define std_const_at(V,INDEX)	STD_CONST_ITEM_PTR_CAST(V, std_container_call_at(&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,at), STD_CONTAINER_HAS_GET(V), INDEX))[0]
#define std_const_front(V)		STD_CONST_ITEM_PTR_CAST(V, std_container_call_front(&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,front), STD_CONTAINER_HAS_GET(V)))
#define std_const_back(V)		STD_CONST_ITEM_PTR_CAST(V, std_container_call_back(&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,back), STD_CONTAINER_HAS_GET(V)))
#define std_const_data(V)		std_const_front(V)[0]

#define std_read_front(V,RESULT)	(RESULT)[0] = std_const_front(V)[0]
#define std_read_back(V,RESULT)		(RESULT)[0] = std_const_back(V)[0]

#define std_construct(V,...)	\
			std_container_call_construct(\
				& V.stBody.stContainer,	\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V, construct), \
				STD_CONTAINER_HAS_GET(V), \
				STD_ITEM_SIZEOF(V), \
				STD_CONTAINER_WRAPPEDITEM_SIZEOF_GET(V), \
				STD_CONTAINER_PAYLOAD_OFFSET_GET(V), \
				& (std_container_handlers_t) { __VA_ARGS__ }	\
			)

#define std_item_new(V)			std_container_item_construct(&V.stBody.stContainer, STD_CONTAINER_HAS_GET(V), STD_ITEM_SIZEOF(V))
#define std_item_delete(V)		std_container_item_destruct( &V.stBody.stContainer, STD_CONTAINER_HAS_GET(V), STD_ITEM_SIZEOF(V))

#define std_destruct(V)			std_container_call_destruct(&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,destruct), STD_CONTAINER_HAS_GET(V))

#define std_reserve(V,N)		std_container_call_reserve(   &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,reserve), STD_CONTAINER_HAS_GET(V), N)
#define std_fit(V)				std_container_call_fit(       &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,fit), STD_CONTAINER_HAS_GET(V))
#define std_empty(V)			std_container_call_empty(     &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,empty), STD_CONTAINER_HAS_GET(V))

#define std_push_front(V,...)	std_container_call_push_front(&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,push_front), STD_CONTAINER_HAS_GET(V), \
									&(STD_ITEM_TYPEOF(V)[]){ __VA_ARGS__ }, \
									STD_NUM_ELEMENTS(((STD_ITEM_TYPEOF(V)[]) { __VA_ARGS__ })))
#define std_push_back(V,...)	std_container_call_push_back( &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,push_back), STD_CONTAINER_HAS_GET(V), \
									&(STD_ITEM_TYPEOF(V)[]){ __VA_ARGS__ }, \
									STD_NUM_ELEMENTS(((STD_ITEM_TYPEOF(V)[]) { __VA_ARGS__ })))
#define std_push(V,...)			std_container_call_push(      &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,push), STD_CONTAINER_HAS_GET(V), \
									&(STD_ITEM_TYPEOF(V)[]){ __VA_ARGS__ }, \
									STD_NUM_ELEMENTS(((STD_ITEM_TYPEOF(V)[]) { __VA_ARGS__ })))

#define STD_CHECK_TYPE(CONTAINER,VAR,PFN)	\
	STD_EXPR_ASSERT(STD_TYPES_ARE_SAME(STD_ITEM_TYPEOF(CONTAINER),VAR), \
		PFN##_is_inconsistent_with_type_of_item_held_by_container_##__COUNTER__)

#define std_pop_front(V,RESULT,MAXITEMS)	\
	(	\
		STD_CHECK_TYPE(V,(RESULT)[0], pop_front_result_parameter), \
		std_container_call_pop_front(	&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,pop_front), STD_CONTAINER_HAS_GET(V), RESULT, MAXITEMS) \
	)

#define std_pop_back(V,RESULT,MAXITEMS)		\
	(	\
		STD_CHECK_TYPE(V, (RESULT)[0], pop_back_result_parameter), \
		std_container_call_pop_back(	&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,pop_back), STD_CONTAINER_HAS_GET(V), RESULT, MAXITEMS)	\
	)

#define std_pop(V,RESULT,MAXITEMS)			\
	(	\
		STD_CHECK_TYPE(V, (RESULT)[0], pop_result_parameter), \
		std_container_call_pop(			&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,pop), STD_CONTAINER_HAS_GET(V), RESULT, MAXITEMS) \
	)

#define std_insert(V,ELEMENT)	std_container_call_insert( &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,insert), STD_CONTAINER_HAS_GET(V), &ELEMENT)
#define std_erase(V)			std_container_call_erase( &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,erase), STD_CONTAINER_HAS_GET(V))

#define std_range(V,FIRST,LAST,IT)		std_container_call_range(&V.stBody, STD_CONTAINER_ENUM_GET_AND_CHECK(V,range), STD_CONTAINER_HAS_GET(V), FIRST(V), LAST(V), &IT)

#define std_sort(V,COMPARE)				std_container_call_ranged_sort( &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,ranged_sort), STD_CONTAINER_HAS_GET(V), 0, V.stBody.szNumItems - 1,	\
		(pfn_std_compare_t)(void (*)(void))STD_CONST_COMPARE_CAST(V,COMPARE))

#define std_ranged_sort(V,A,B,COMPARE)	std_container_call_ranged_sort(&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,ranged_sort), STD_CONTAINER_HAS_GET(V), A, B, 	\
		(pfn_std_compare_t)(void (*)(void))STD_CONST_COMPARE_CAST(V,COMPARE))

#define std_iterator_construct(V, IT)			\
		std_iterator_call_construct(&V.stBody.stContainer, STD_CONTAINER_ENUM_GET(IT), STD_CONTAINER_HAS_GET(V), STD_ITERATOR_ENUM_GET_AND_CHECK(IT,construct), &IT.stItBody.stIterator)
#define std_iterator_range(IT,BEGIN,END)	\
		std_iterator_call_range(&IT.stItBody.stIterator, STD_CONTAINER_ENUM_GET(IT), STD_CONTAINER_HAS_GET(IT), STD_ITERATOR_ENUM_GET_AND_CHECK(IT,range), BEGIN, END)
#define std_iterator_next(IT)				\
		std_iterator_call_next(&IT.stItBody.stIterator, STD_CONTAINER_ENUM_GET(IT), STD_CONTAINER_HAS_GET(IT), STD_ITERATOR_ENUM_GET_AND_CHECK(IT,next))
#define std_iterator_prev(IT)				\
		std_iterator_call_prev(&IT.stItBody.stIterator, STD_CONTAINER_ENUM_GET(IT), STD_CONTAINER_HAS_GET(IT), STD_ITERATOR_ENUM_GET_AND_CHECK(IT,prev))

#endif /* STD_CONTAINER_H_ */
