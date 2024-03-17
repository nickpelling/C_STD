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

typedef struct
{
	const char * const pachContainerName;

	bool	(* const pfn_init)			(std_container_t * pstContainer, size_t szFullSizeof, size_t szPayloadOffset,
											std_container_has_t eHas, const std_container_handlers_t * pstHandlers);
	void	(* const pfn_reserve)		(std_container_t * pstContainer, size_t szNewSize);
	void	(* const pfn_fit)			(std_container_t * pstContainer);
	void *	(* const pfn_push_front)	(std_container_t * pstContainer);
	void *	(* const pfn_push_back)		(std_container_t * pstContainer);
	void *	(* const pfn_push)			(std_container_t * pstContainer);
	void *	(* const pfn_pop_front)		(std_container_t * pstContainer, void * pvResult);
	void *	(* const pfn_pop_back)		(std_container_t * pstContainer, void * pvResult);
	void *	(* const pfn_pop)			(std_container_t * pstContainer, void * pvResult);
	void	(* const pfn_range) 		(std_container_t * pstContainer, void * pvBegin, void * pvEnd, std_iterator_t * pstIterator);
	void	(* const pfn_ranged_sort)	(std_container_t * pstContainer, size_t szFirst, size_t szLast, pfn_std_compare_t pfn_Compare);
	void *	(* const pfn_at)			(std_container_t * pstContainer, int32_t iIndex);
	bool	(* const pfn_empty)			(std_container_t * pstContainer);

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

inline const char* std_container_name_get(std_container_enum_t eContainer)
{
	return std_container_jumptable_array[eContainer].pachContainerName;
}

inline bool std_container_call_init(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas,
				size_t szFullSizeof, size_t szPayloadOffset, const std_container_handlers_t * pstHandlers)
{
	return STD_CONTAINER_CALL(eContainer, pfn_init)(pstContainer, szFullSizeof, szPayloadOffset, eHas, pstHandlers);
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void std_container_lock_for_writing(std_container_t* pstContainer, std_container_has_t eHas, std_lock_state_t* peOldState)
{
	if (eHas & std_container_has_lockhandler)
	{
		*peOldState = std_lock_for_writing(pstContainer->pstLockHandler, pstContainer->phLock, CONTAINER_TIMEOUT_DEFAULT);
	}
}

inline void std_container_lock_for_reading(std_container_t* pstContainer, std_container_has_t eHas, std_lock_state_t * peOldState)
{
	if (eHas & std_container_has_lockhandler)
	{
		*peOldState = std_lock_for_reading(pstContainer->pstLockHandler, pstContainer->phLock, CONTAINER_TIMEOUT_DEFAULT);
	}
}

inline void std_container_lock_restore(std_container_t* pstContainer, std_container_has_t eHas, std_lock_state_t eOldState)
{
	if (eHas & std_container_has_lockhandler)
	{
		std_lock_update(pstContainer->pstLockHandler, pstContainer->phLock, eOldState, CONTAINER_TIMEOUT_DEFAULT);
	}
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void std_container_call_reserve(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, size_t szNewSize)
{
	std_lock_state_t eOldState;
	std_container_lock_for_writing(pstContainer, eHas, &eOldState);
	STD_CONTAINER_CALL(eContainer, pfn_reserve)(pstContainer, szNewSize);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

inline void std_container_call_fit(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas)
{
	std_lock_state_t eOldState;
	std_container_lock_for_writing(pstContainer, eHas, &eOldState);
	STD_CONTAINER_CALL(eContainer, pfn_fit)(pstContainer);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

inline void* std_container_call_push_front(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas)
{
	std_lock_state_t eOldState;
	std_container_lock_for_writing(pstContainer, eHas, &eOldState);
	void * pvPtr = STD_CONTAINER_CALL(eContainer, pfn_push_front)(pstContainer);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return pvPtr;
}

inline void* std_container_call_push_back(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas)
{
	std_lock_state_t eOldState;
	std_container_lock_for_writing(pstContainer, eHas, &eOldState);
	void * pvPtr = STD_CONTAINER_CALL(eContainer, pfn_push_back)(pstContainer);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return pvPtr;
}

inline void* std_container_call_push(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas)
{
	std_lock_state_t eOldState;
	std_container_lock_for_writing(pstContainer, eHas, &eOldState);
	void * pvPtr = STD_CONTAINER_CALL(eContainer, pfn_push)(pstContainer);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return pvPtr;
}

inline void* std_container_call_pop_front(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, void* pvResult)
{
	std_lock_state_t eOldState;
	std_container_lock_for_writing(pstContainer, eHas, &eOldState);
	void * pvPtr = STD_CONTAINER_CALL(eContainer, pfn_pop_front)(pstContainer, pvResult);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return pvPtr;
}

inline void* std_container_call_pop_back(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, void* pvResult)
{
	std_lock_state_t eOldState;
	std_container_lock_for_writing(pstContainer, eHas, &eOldState);
	void * pvPtr = STD_CONTAINER_CALL(eContainer, pfn_pop_back)(pstContainer, pvResult);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return pvPtr;
}

inline void* std_container_call_pop(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, void* pvResult)
{
	std_lock_state_t eOldState;
	std_container_lock_for_writing(pstContainer, eHas, &eOldState);
	void * pvPtr = STD_CONTAINER_CALL(eContainer, pfn_pop)(pstContainer, pvResult);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return pvPtr;
}

inline void std_container_call_range(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, void* pvBegin, void* pvEnd, std_iterator_t* pstIterator)
{
	std_lock_state_t eOldState;
	std_container_lock_for_reading(pstContainer, eHas, &eOldState);
	STD_CONTAINER_CALL(eContainer, pfn_range)(pstContainer, pvBegin, pvEnd, pstIterator);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

inline void std_container_call_ranged_sort(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, size_t szFirst, size_t szLast, pfn_std_compare_t pfn_Compare)
{
	std_lock_state_t eOldState;
	std_container_lock_for_writing(pstContainer, eHas, &eOldState);
	STD_CONTAINER_CALL(eContainer, pfn_ranged_sort)(pstContainer, szFirst, szLast, pfn_Compare);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

inline void* std_container_call_at(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, int32_t iIndex)
{
	std_lock_state_t eOldState;
	std_container_lock_for_reading(pstContainer, eHas, &eOldState);
	void * pvPtr = STD_CONTAINER_CALL(eContainer, pfn_at)(pstContainer, iIndex);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return pvPtr;
}

inline bool std_container_call_empty(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas)
{
	std_lock_state_t eOldState;
	std_container_lock_for_reading(pstContainer, eHas, &eOldState);
	bool bResult = STD_CONTAINER_CALL(eContainer, pfn_empty)(pstContainer);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return bResult;
}

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

inline void std_iterator_call_init(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, std_iterator_enum_t eIterator, std_iterator_t* pstIterator)
{
	STD_ITERATOR_CALL(eContainer, eIterator, pfn_init)(pstContainer, pstIterator);
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

// Client-side (typed) methods

#define std_container_name(V)	std_container_name_get(STD_CONTAINER_ENUM_GET(V))

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

#define std_init(V,...)			std_container_call_init(&V.stBody.stContainer, \
									STD_CONTAINER_ENUM_GET_AND_CHECK(V,init), STD_CONTAINER_HAS_GET(V), \
									STD_CONTAINER_FULLSIZEOF_GET(V), STD_CONTAINER_PAYLOAD_OFFSET_GET(V), \
									&(std_container_handlers_t){ __VA_ARGS__ })

#define std_reserve(V,N)		std_container_call_reserve(   &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,reserve), STD_CONTAINER_HAS_GET(V), N)
#define std_fit(V)				std_container_call_fit(       &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,fit), STD_CONTAINER_HAS_GET(V))
#define std_empty(V)			std_container_call_empty(     &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,empty), STD_CONTAINER_HAS_GET(V))

#define std_push_front(V)		STD_ITEM_PTR_CAST(V, std_container_call_push_front(&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,push_front), STD_CONTAINER_HAS_GET(V)))
#define std_push_back(V)		STD_ITEM_PTR_CAST(V, std_container_call_push_back( &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,push_back), STD_CONTAINER_HAS_GET(V)))
#define std_push(V)				STD_ITEM_PTR_CAST(V, std_container_call_push(      &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,push), STD_CONTAINER_HAS_GET(V)))
#define std_pop_front(V,RESULT)	STD_ITEM_PTR_CAST(V, std_container_call_pop_front( &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,pop_front), STD_CONTAINER_HAS_GET(V), RESULT))
#define std_pop_back(V,RESULT)	STD_ITEM_PTR_CAST(V, std_container_call_pop_back(  &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,pop_back), STD_CONTAINER_HAS_GET(V), RESULT))
#define std_pop(V,RESULT)		STD_ITEM_PTR_CAST(V, std_container_call_pop(       &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,pop), STD_CONTAINER_HAS_GET(V), RESULT))

#define std_insert(V,ELEMENT)	std_container_call_insert( &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,insert), STD_CONTAINER_HAS_GET(V), &ELEMENT)
#define std_erase(V)			std_container_call_erase( &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,erase), STD_CONTAINER_HAS_GET(V))

#define std_range(V,FIRST,LAST,IT)		std_container_call_range(&V.stBody, STD_CONTAINER_ENUM_GET_AND_CHECK(V,range), STD_CONTAINER_HAS_GET(V), FIRST(V), LAST(V), &IT)

#define std_sort(V,COMPARE)				std_container_call_ranged_sort( &V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,ranged_sort), STD_CONTAINER_HAS_GET(V), 0, V.stBody.szNumItems - 1,	\
		(pfn_std_compare_t)(void (*)(void))STD_CONST_COMPARE_CAST(V,COMPARE))

#define std_ranged_sort(V,A,B,COMPARE)	std_container_call_ranged_sort(&V.stBody.stContainer, STD_CONTAINER_ENUM_GET_AND_CHECK(V,ranged_sort), STD_CONTAINER_HAS_GET(V), A, B, 	\
		(pfn_std_compare_t)(void (*)(void))STD_CONST_COMPARE_CAST(V,COMPARE))

#define std_iterator_init(V, IT)			\
		std_iterator_call_init(&V.stBody.stContainer, STD_CONTAINER_ENUM_GET(IT), STD_CONTAINER_HAS_GET(V), std_iterator_enum(IT), &IT.stItBody.stIterator)
#define std_iterator_range(IT,BEGIN,END)	\
		std_iterator_call_range(&IT.stItBody.stIterator, STD_CONTAINER_ENUM_GET(IT), STD_CONTAINER_HAS_GET(IT), std_iterator_enum(IT), BEGIN, END)
#define std_iterator_next(IT)				\
		std_iterator_call_next(&IT.stItBody.stIterator, STD_CONTAINER_ENUM_GET(IT), STD_CONTAINER_HAS_GET(IT), std_iterator_enum(IT))
#define std_iterator_prev(IT)				\
		std_iterator_call_prev(&IT.stItBody.stIterator, STD_CONTAINER_ENUM_GET(IT), STD_CONTAINER_HAS_GET(IT), std_iterator_enum(IT))

#endif /* STD_CONTAINER_H_ */
