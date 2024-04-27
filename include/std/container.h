/*
 * std/container.h

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

#ifndef STD_CONTAINER_H_
#define STD_CONTAINER_H_

#include "std/iterator.h"
#include "std/lock.h"
#include "std/linear_series.h"

#include "std/vector.h"
#include "std/forward_list.h"
#include "std/list.h"
#include "std/deque.h"
#include "std/ring.h"
#include "std/set.h"

#define CONTAINER_TIMEOUT_DEFAULT	500		// FIXME (should probably move this into the lock handler?!)

#define STD_CONTAINER_CALL(CONTAINER_INDEX,PTRFUNC)	\
		(*std_container_jumptable_array[CONTAINER_INDEX].PTRFUNC)

#define STD_ITERATOR_CALL(CONTAINER_INDEX,ITERATOR_INDEX,PTRFUNC) \
		(*std_container_jumptable_array[CONTAINER_INDEX].astIterators[ITERATOR_INDEX].PTRFUNC)

#define STD_CONTAINER_CALL_EXISTS(V,IMPLEMENTS)	\
			STD_STATIC_ASSERT(STD_CONTAINER_IMPLEMENTS_GET(V) & std_container_implements_ ## IMPLEMENTS, \
					STD_CONCAT(IMPLEMENTS ## _is_not_implemented_for_this_type_of_container_,__COUNTER__) )

#define STD_CONTAINER_ENUM_GET_AND_CHECK(V,IMPLEMENTS)	\
	(STD_CONTAINER_CALL_EXISTS(V,IMPLEMENTS), STD_CONTAINER_ENUM_GET(V))

#define STD_ITERATOR_PARENT_ENUM_GET(IT)		STD_CONTAINER_ENUM_GET(IT.puParent[0])
#define STD_ITERATOR_PARENT_HAS_GET(IT)			STD_CONTAINER_HAS_GET(IT.puParent[0])
#define STD_ITERATOR_PARENT_IMPLEMENTS_GET(IT)	STD_CONTAINER_IMPLEMENTS_GET(IT.puParent[0])

#define STD_ITERATOR_IMPLEMENTS_GET(INDEX,IMPLEMENTS)	\
			((INDEX == std_iterator_enum_forward) ? std_container_implements_forward_ ## IMPLEMENTS : std_container_implements_reverse_ ## IMPLEMENTS)

#define STD_ITERATOR_CALL_EXISTS(IT, IMPLEMENTS) \
			STD_STATIC_ASSERT(STD_ITERATOR_PARENT_IMPLEMENTS_GET(IT) & STD_ITERATOR_IMPLEMENTS_GET(STD_ITERATOR_ENUM_GET(IT),IMPLEMENTS), \
					STD_CONCAT(IMPLEMENTS ## _is_not_implemented_for_this_type_of_iterator_,__COUNTER__) )

#define STD_ITERATOR_ENUM_GET_AND_CHECK(IT,IMPLEMENTS) \
	(	\
		STD_ITERATOR_CALL_EXISTS(IT,IMPLEMENTS), \
		STD_ITERATOR_ENUM_GET(IT)	\
	)

#define STD_CHECK_TYPE(CONTAINER,VAR,PFN)	\
			STD_STATIC_ASSERT(STD_TYPES_ARE_SAME(STD_ITEM_TYPEOF(CONTAINER),VAR), \
					PFN##_is_inconsistent_with_type_of_item_held_by_container_##__COUNTER__)

typedef struct
{
	const char * const pachContainerName;

	void	(* const pfn_construct)		(std_container_t * pstContainer, size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset,
											std_container_has_t eHas);
	bool	(* const pfn_reserve)		(std_container_t * pstContainer, size_t szNewSize);
	size_t	(* const pfn_push_front)	(std_container_t * pstContainer, const std_linear_series_t * pstSeries);
	size_t	(* const pfn_push_back)		(std_container_t * pstContainer, const std_linear_series_t * pstSeries);
	size_t	(* const pfn_pop_front)		(std_container_t * pstContainer, void * pvResult, size_t szMaxItems);
	size_t	(* const pfn_pop_back)		(std_container_t * pstContainer, void * pvResult, size_t szMaxItems);
	void	(* const pfn_range) 		(std_container_t * pstContainer, void * pvBegin, void * pvEnd, std_iterator_t * pstIterator);
	void	(* const pfn_ranged_sort)	(std_container_t * pstContainer, size_t szFirst, size_t szLast, pfn_std_compare_t pfn_Compare);
	void *	(* const pfn_at)			(std_container_t * pstContainer, size_t szIndex);
	bool	(* const pfn_destruct)		(std_container_t * pstContainer);

	void	(* const pfn_erase)			(std_iterator_t * pstIterator);

	std_container_iterate_jumptable_t	astIterators[std_iterator_enum_MAX];

	const std_item_handler_t			* pstDefaultItemHandler;
} std_container_jumptable_t;

STD_STATIC const std_container_jumptable_t std_container_jumptable_array[std_container_enum_MAX] =
{
	[std_container_enum_deque]			= { STD_DEQUE_JUMPTABLE },
	[std_container_enum_forward_list]	= { STD_FORWARD_LIST_JUMPTABLE },
	[std_container_enum_list]			= { STD_LIST_JUMPTABLE },
//	[std_container_enum_prioritydeque]	= { STD_PRIORITYDEQUE_JUMPTABLE },
//	[std_container_enum_priorityqueue]	= { STD_PRIORITYQUEUE_JUMPTABLE },
	[std_container_enum_ring]			= { STD_RING_JUMPTABLE },
	[std_container_enum_set]			= { STD_SET_JUMPTABLE },
	[std_container_enum_vector]			= { STD_VECTOR_JUMPTABLE },
};

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define std_size(V)				(V.stBody.stContainer.szNumItems)
#define std_is_empty(V)			(std_size(V) == 0U)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Get the name of a type of container (if it implements the name callback)
 * 
 * @param[in]	eContainer		The container type index
 * @param[in]	eImplements		Bitmask of implementation flags for this container type
 * 
 * @return Name of the container
 */
STD_INLINE const char* std_container_name_get(std_container_enum_t eContainer, std_container_implements_t eImplements)
{
	if (eImplements & std_container_implements_name)
	{
		return std_container_jumptable_array[eContainer].pachContainerName;
	}
	return "(Unnamed container)";
}

#define std_container_name(V)								\
			std_container_name_get(							\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V,name),	\
				STD_CONTAINER_IMPLEMENTS_GET(V)	)

/**
* Get the default itemhandler for a type of container
* 
* @param[in]	eContainer		The container type index
* @param[in]	eImplements		Bitmask of implementation flags for this container type
* 
* @return Default item handler for the container (or NULL if it doesn't implement one)
*/
STD_INLINE const std_item_handler_t * std_container_default_itemhandler_get(std_container_enum_t eContainer, std_container_implements_t eImplements)
{
	if (eImplements & std_container_implements_default_itemhandler)
	{
		return std_container_jumptable_array[eContainer].pstDefaultItemHandler;
	}
	return NULL;
}

#define std_container_default_itemhandler(V)				\
			std_container_default_itemhandler_get(			\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V,default_itemhandler), \
				STD_CONTAINER_IMPLEMENTS_GET(V)		)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Construct an untyped container
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	szSizeof		Size of the contained item
 * @param[in]	szWrappedSizeof	Size of the wrapped item (e.g. including linked list pointers)
 * @param[in]	szPayloadOffset	Offset to the start of the item payload (e.g. past the linked list pointers)
 */
STD_INLINE void std_container_call_construct(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas,
				size_t szSizeof, size_t szWrappedSizeof, size_t szPayloadOffset)
{
	STD_CONTAINER_CALL(eContainer, pfn_construct)(pstContainer, szSizeof, szWrappedSizeof, szPayloadOffset, eHas);
}

// Construct a typed container
#define STD_CONSTRUCT(V,HAS)							\
			STD_STATIC_ASSERT(STD_CONTAINER_HAS_GET(V) == HAS, STD_CONCAT(container_declaration_and_instantiation_are_inconsistent_,__COUNTER__));	\
			std_container_call_construct(					\
				&V.stBody.stContainer,						\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V, construct), \
				HAS,										\
				STD_ITEM_SIZEOF(V),							\
				STD_CONTAINER_WRAPPEDITEM_SIZEOF_GET(V),	\
				STD_CONTAINER_PAYLOAD_OFFSET_GET(V)		)

#define std_construct(V)	\
			STD_CONSTRUCT(V, std_container_has_no_handlers)

#define std_construct_itemhandler(V,ITEMHANDLER)	\
			V.stBody.stContainer.pstItemHandler = ITEMHANDLER;	\
			STD_CONSTRUCT(V, std_container_has_itemhandler)

#define std_construct_memoryhandler(V,MEMORYHANDLER)	\
			V.stBody.stContainer.pstMemoryHandler = MEMORYHANDLER;	\
			STD_CONSTRUCT(V, std_container_has_memoryhandler)

#define std_construct_memoryhandler_itemhandler(V,MEMORYHANDLER,ITEMHANDLER)	\
			V.stBody.stContainer.pstMemoryHandler = MEMORYHANDLER;	\
			V.stBody.stContainer.pstItemHandler = ITEMHANDLER;	\
			STD_CONSTRUCT(V, std_container_has_memoryhandler_itemhandler)

#define std_construct_lockhandler(V,LOCKHANDLER)	\
			V.stBody.stContainer.pstLockHandler = LOCKHANDLER;	\
			STD_CONSTRUCT(V, std_container_has_lockhandler)

#define std_construct_lockhandler_itemhandler(V,LOCKHANDLER,ITEMHANDLER)	\
			V.stBody.stContainer.pstLockHandler = LOCKHANDLER;	\
			V.stBody.stContainer.pstItemHandler = ITEMHANDLER;	\
			STD_CONSTRUCT(V, std_container_has_lockhandler_itemhandler)

#define std_construct_lockhandler_memoryhandler(V,LOCKHANDLER,MEMORYHANDLER)	\
			V.stBody.stContainer.pstLockHandler = LOCKHANDLER;	\
			V.stBody.stContainer.pstMemoryHandler = MEMORYHANDLER;	\
			STD_CONSTRUCT(V, std_container_has_lockhandler_memoryhandler)

#define std_construct_lockhandler_memoryhandler_itemhandler(V,LOCKHANDLER,MEMORYHANDLER,ITEMHANDLER)	\
			V.stBody.stContainer.pstLockHandler = LOCKHANDLER;	\
			V.stBody.stContainer.pstMemoryHandler = MEMORYHANDLER;	\
			V.stBody.stContainer.pstItemHandler = ITEMHANDLER;	\
			STD_CONSTRUCT(V, std_container_has_lockhandler_memoryhandler_itemhandler)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Lock an untyped container
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	bReadOnly		True if the caller only wants to lock for reading, else false
 *
 * @return e_std_lock_NoRestoreNeeded if no lock restore needed, else the lock state to restore to
 */
STD_INLINE std_lock_state_t std_container_lock(std_container_t* pstContainer, std_container_has_t eHas, bool bReadOnly)
{
	if (eHas & std_container_has_lockhandler)
	{
		return (bReadOnly)
			? std_lock_for_reading(pstContainer->pstLockHandler, pstContainer->phLock, CONTAINER_TIMEOUT_DEFAULT)
			: std_lock_for_writing(pstContainer->pstLockHandler, pstContainer->phLock, CONTAINER_TIMEOUT_DEFAULT);
	}
	return e_std_lock_NoRestoreNeeded;
}

// Lock a typed container for reading
#define std_container_lock_for_reading(CONTAINER,HAS)	\
			std_container_lock(CONTAINER,HAS,true)

// Lock a typed container for writing
#define std_container_lock_for_writing(CONTAINER,HAS)	\
			std_container_lock(CONTAINER,HAS,false)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Restore a container's previous lock state
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	eOldState		Previous lock state to restore
 */
STD_INLINE void std_container_lock_restore(std_container_t* pstContainer, std_container_has_t eHas, std_lock_state_t eOldState)
{
	if (	(eHas & std_container_has_lockhandler)
		&&	(eOldState != e_std_lock_NoRestoreNeeded)	)
	{
		std_lock_update(pstContainer->pstLockHandler, pstContainer->phLock, eOldState, CONTAINER_TIMEOUT_DEFAULT);
	}
}

// Wrap an untyped container lock/unlock for-loop around the actions that follow
// Note: this must ultimately be wrapped by a for(....)!
#define STD_CONTAINER_LOCK_WRAPPER(CONTAINER,HAS,READONLY,VARNAME)	\
	std_lock_state_t VARNAME = std_container_lock(CONTAINER, HAS, READONLY);	\
		VARNAME != e_std_lock_Invalid;	\
		VARNAME = (std_container_lock_restore(CONTAINER, HAS, VARNAME), e_std_lock_Invalid)

// Wrap a typed container lock/unlock for-loop around the actions that follow
#define std_container_lock_wrapper(CONTAINER,READONLY)	\
			STD_CONTAINER_LOCK_WRAPPER(					\
				&CONTAINER.stBody.stContainer,			\
				STD_CONTAINER_HAS_GET(CONTAINER),		\
				READONLY,								\
				STD_FAKEVAR()	)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Destruct an untyped container
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 */
STD_INLINE bool std_container_call_destruct(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas)
{
	/* std_lock_state_t eOldState = */ std_container_lock_for_writing(pstContainer, eHas);
	return STD_CONTAINER_CALL(eContainer, pfn_destruct)(pstContainer);
}

// Destruct a typed container
#define std_destruct(V)											\
			std_container_call_destruct(						\
				&V.stBody.stContainer,							\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V,destruct),	\
				STD_CONTAINER_HAS_GET(V)	)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Reserve space in an untyped container
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	szNewSize		New number of items to allocate in the container
 * 
 * @return True if was able to reserve space for the requested number of items, else false
 */
STD_INLINE bool std_container_call_reserve(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, size_t szNewSize)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	bool bRetVal = STD_CONTAINER_CALL(eContainer, pfn_reserve)(pstContainer, szNewSize);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return bRetVal;
}

// Reserve space in a typed container
#define std_reserve(V,N)					\
			std_container_call_reserve(		\
				&V.stBody.stContainer,		\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V,reserve),	\
				STD_CONTAINER_HAS_GET(V), N)

// Fit a typed container to the number of elements previously pushed to it
#define std_fit(V)		std_reserve(V, std_size(V))

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

#define STD_PUSH_DATA(CONTAINER,...)	\
			&(STD_ITEM_TYPEOF(CONTAINER)[]){ __VA_ARGS__ },			\
			STD_NUM_ELEMENTS(((STD_ITEM_TYPEOF(CONTAINER)[]) { __VA_ARGS__ }))

/**
 * Push a linear series of items to the front of an untyped container
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	pvBase			Start of a linear series of items
 * @param[in]	szNumElements	Number of items in the linear series
 * 
 * @return Number of items pushed onto the container
 */
STD_INLINE size_t std_container_call_push_front(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, bool bReverse, const void* pvBase, size_t szNumElements)
{
	std_linear_series_t stSeries;
	std_linear_series_construct(&stSeries, pvBase, pstContainer->szSizeofItem, szNumElements, bReverse);
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	size_t szNumPushed = STD_CONTAINER_CALL(eContainer, pfn_push_front)(pstContainer, &stSeries);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return szNumPushed;
}

// Push a linear series of items to the front of a typed container
#define std_push_front(V,...)									\
			std_container_call_push_front(						\
				&V.stBody.stContainer,							\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V,push_front), \
				STD_CONTAINER_HAS_GET(V),						\
				false,											\
				STD_PUSH_DATA(V,__VA_ARGS__)	)

// Prepend a linear series of items to the front of a typed container
#define std_prepend(V,...)										\
			std_container_call_push_front(						\
				&V.stBody.stContainer,							\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V,push_front), \
				STD_CONTAINER_HAS_GET(V),						\
				true,											\
				STD_PUSH_DATA(V,__VA_ARGS__)	)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Push a linear series of items to the back of a container
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	bReverse		If true, reverse the order of the linear series
 * @param[in]	pvBase			Start of a linear series of items
 * @param[in]	szNumElements	Number of items in the linear series
 *
 * @return Number of items pushed onto the container
 */
STD_INLINE size_t std_container_call_push_back(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, bool bReverse, const void * pvBase, size_t szNumElements)
{
	std_linear_series_t stSeries;
	std_linear_series_construct(&stSeries, pvBase, pstContainer->szSizeofItem, szNumElements, bReverse);
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	size_t szNumPushed = STD_CONTAINER_CALL(eContainer, pfn_push_back)(pstContainer, &stSeries);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return szNumPushed;
}

#define std_push_back(V,...)				\
			std_container_call_push_back(	\
				&V.stBody.stContainer,		\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V,push_back),	\
				STD_CONTAINER_HAS_GET(V),	\
				false,						\
				STD_PUSH_DATA(V,__VA_ARGS__)	)

#define std_append_reversed(V,...)			\
			std_container_call_push_back(	\
				&V.stBody.stContainer,		\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V,push_back),	\
				STD_CONTAINER_HAS_GET(V),	\
				true,						\
				STD_PUSH_DATA(V,__VA_ARGS__)	)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Pop a number of items from the front of a container into a linear area of memory
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[out]	pvBase			Start of the destination memory area (can be NULL)
 * @param[in]	szMaxItems		Maximum number of items allowed in the linear series
 * 
 * @return Number of items actually popped from the container
 */
STD_INLINE size_t std_container_call_pop_front(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, void* pvResult, size_t szMaxItems)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	size_t szNum = STD_CONTAINER_CALL(eContainer, pfn_pop_front)(pstContainer, pvResult, szMaxItems);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return szNum;
}

#define std_pop_front(V,RESULT,MAXITEMS)	\
	(										\
		STD_CHECK_TYPE(V,(RESULT)[0], pop_front_result_parameter), \
		std_container_call_pop_front(		\
			&V.stBody.stContainer,			\
			STD_CONTAINER_ENUM_GET_AND_CHECK(V,pop_front),	\
			STD_CONTAINER_HAS_GET(V),		\
			RESULT,							\
			MAXITEMS)						\
	)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Pop a number of items from the back of a container into a linear area of memory
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[out]	pvBase			Start of the destination memory area (can be NULL)
 * @param[in]	szMaxItems		Maximum number of items allowed in the linear series
 *
 * @return Number of items actually popped from the container
 */
STD_INLINE size_t std_container_call_pop_back(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, void* pvResult, size_t szMaxItems)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	size_t szNum = STD_CONTAINER_CALL(eContainer, pfn_pop_back)(pstContainer, pvResult, szMaxItems);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return szNum;
}

#define std_pop_back(V,RESULT,MAXITEMS)		\
	(										\
		STD_CHECK_TYPE(V, (RESULT)[0], pop_back_result_parameter), \
		std_container_call_pop_back(		\
			&V.stBody.stContainer,			\
			STD_CONTAINER_ENUM_GET_AND_CHECK(V,pop_back),	\
			STD_CONTAINER_HAS_GET(V),		\
			RESULT,							\
			MAXITEMS)						\
	)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Construct an iterator to step through a specified range of entries in a container
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	pvBegin			First entry in range
 * @param[in]	pvEnd			Entry after the final entry in range
 * @param[out]	pstIterator		Iterator to construct
 */
STD_INLINE void std_container_call_range(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, void* pvBegin, void* pvEnd, std_iterator_t* pstIterator)
{
	std_lock_state_t eOldState = std_container_lock_for_reading(pstContainer, eHas);
	STD_CONTAINER_CALL(eContainer, pfn_range)(pstContainer, pvBegin, pvEnd, pstIterator);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

#define std_range(V,FIRST,LAST,IT)			\
			std_container_call_range(		\
				&V.stBody,					\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V,range),	\
				STD_CONTAINER_HAS_GET(V),	\
				FIRST(V),					\
				LAST(V),					\
				&IT)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Destruct a container
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	szFirst			First index
 * @param[in]	szLast			Last index
 * @param[in]	pfn_Compare		Comparison callback function
 */
STD_INLINE void std_container_call_ranged_sort(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, size_t szFirst, size_t szLast, pfn_std_compare_t pfn_Compare)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	STD_CONTAINER_CALL(eContainer, pfn_ranged_sort)(pstContainer, szFirst, szLast, pfn_Compare);
	std_container_lock_restore(pstContainer, eHas, eOldState);
}

#define std_sort(V,COMPARE)					\
			std_container_call_ranged_sort(	\
				&V.stBody.stContainer,		\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V,ranged_sort),	\
				STD_CONTAINER_HAS_GET(V),	\
				0,							\
				std_size(V) - 1,			\
				(pfn_std_compare_t)(void (*)(void))STD_CONST_COMPARE_CAST(V,COMPARE)	)

#define std_ranged_sort(V,A,B,COMPARE)		\
			std_container_call_ranged_sort(	\
				&V.stBody.stContainer,		\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V,ranged_sort),	\
				STD_CONTAINER_HAS_GET(V),	\
				A,							\
				B,							\
				(pfn_std_compare_t)(void (*)(void))STD_CONST_COMPARE_CAST(V,COMPARE)	)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Calculate a pointer to an indexed entry within a container
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	szIndex			Index of entry
 */
STD_INLINE void* std_container_call_at(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, size_t szIndex)
{
	std_lock_state_t eOldState = std_container_lock_for_reading(pstContainer, eHas);
	void* pvPtr = STD_CONTAINER_CALL(eContainer, pfn_at)(pstContainer, szIndex);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return pvPtr;
}

#define std_at(V,INDEX)										\
			STD_ITEM_PTR_CAST(V,							\
				std_container_call_at(						\
					&V.stBody.stContainer,					\
					STD_CONTAINER_ENUM_GET_AND_CHECK(V,at), \
					STD_CONTAINER_HAS_GET(V),				\
					INDEX)	)

#define std_at_const(V,INDEX)								\
			STD_ITEM_PTR_CAST_CONST(V,						\
				std_container_call_at(						\
					&V.stBody.stContainer,					\
					STD_CONTAINER_ENUM_GET_AND_CHECK(V,at),	\
					STD_CONTAINER_HAS_GET(V),				\
					INDEX)	)

#define std_front(V)			std_at(V, 0)
#define std_back(V)				std_at(V, std_size(V) - 1U)

#define std_front_const(V)		std_at_const(V, 0)
#define std_back_const(V)		std_at_const(V, std_size(V) - 1U)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Construct an iterator
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	eIterator		Which iterator type to use
 * @param[in]	pstIterator		Iterator to construct
 */
STD_INLINE void std_iterator_call_construct(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, std_iterator_enum_t eIterator, std_iterator_t* pstIterator)
{
	if (eHas) { /* Unused parameter */ }
	pstIterator->pstContainer = pstContainer;
	pstIterator->szSizeofItem = pstContainer->szSizeofItem;
	STD_ITERATOR_CALL(eContainer, eIterator, pfn_construct)(pstContainer, pstIterator);
}

#define std_iterator_construct(V, IT)				\
			std_iterator_call_construct(			\
				&V.stBody.stContainer,				\
				STD_ITERATOR_PARENT_ENUM_GET(IT),	\
				STD_ITERATOR_PARENT_HAS_GET(IT),	\
				STD_ITERATOR_ENUM_GET_AND_CHECK(IT,construct),	\
				&IT.stItBody.stIterator)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Construct an iterator to step through a range of items inside a container
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	eIterator		Which iterator type to use
 * @param[out]	pstIterator		Iterator to construct
 * @param[in]	pvBegin			First item in the range
 * @param[in]	pvEnd			Item immediately after the last item in the range
 */
STD_INLINE void std_iterator_call_range(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, std_iterator_enum_t eIterator, std_iterator_t* pstIterator, void* pvBegin, void* pvEnd)
{
	if (eHas) { /* Unused parameter */ }
	STD_ITERATOR_CALL(eContainer, eIterator, pfn_range)(pstContainer, pstIterator, pvBegin, pvEnd);
}

#define std_iterator_range(IT,BEGIN,END)			\
			std_iterator_call_range(				\
				&IT.stItBody.stIterator,			\
				STD_ITERATOR_PARENT_ENUM_GET(IT),	\
				STD_ITERATOR_PARENT_HAS_GET(IT),	\
				STD_ITERATOR_ENUM_GET_AND_CHECK(IT,range),	\
				BEGIN,								\
				END)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Step an iterator forwards by one
 * 
 * @param[in]	pstIterator		Iterator
 * @param[in]	eContainer		Container type
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	eIterator		Iterator type (e.g. forward or reverse)
 */
STD_INLINE void std_iterator_call_next(std_iterator_t* pstIterator, std_container_enum_t eContainer, std_container_has_t eHas, std_iterator_enum_t eIterator)
{
	if (eHas) { /* Unused parameter */ }
	STD_ITERATOR_CALL(eContainer, eIterator, pfn_next)(pstIterator);
}

#define std_iterator_next(IT)						\
			std_iterator_call_next(					\
				&IT.stItBody.stIterator,			\
				STD_ITERATOR_PARENT_ENUM_GET(IT),	\
				STD_ITERATOR_PARENT_HAS_GET(IT),	\
				STD_ITERATOR_ENUM_GET_AND_CHECK(IT,next)	)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Step an iterator backwards by one
 *
 * @param[in]	pstIterator		Iterator
 * @param[in]	eContainer		Container type
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[in]	eIterator		Iterator type (e.g. forward or reverse)
 */
STD_INLINE void std_iterator_call_prev(std_iterator_t* pstIterator, std_container_enum_t eContainer, std_container_has_t eHas, std_iterator_enum_t eIterator)
{
	if (eHas) { /* Unused parameter */ }
	STD_ITERATOR_CALL(eContainer, eIterator, pfn_prev)(pstIterator);
}

#define std_iterator_prev(IT)						\
			std_iterator_call_prev(					\
				&IT.stItBody.stIterator,			\
				STD_ITERATOR_PARENT_ENUM_GET(IT),	\
				STD_ITERATOR_PARENT_HAS_GET(IT),	\
				STD_ITERATOR_ENUM_GET_AND_CHECK(IT,prev)	)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Insert a linear series of items after the current iterator
 *
 * @param[in]	pstIterator		Iterator
 * @param[in]	eContainer		The container type index
 * @param[in]	eIterator		Iterator type (e.g. forward or reverse)
 * @param[in]	pvBase			Linear series of items
 * @param[in]	szNumElements	Number of items
 *
 * @return Number of items inserted
 */
STD_INLINE size_t std_iterator_call_insert_after(std_iterator_t* pstIterator, std_container_enum_t eContainer, std_iterator_enum_t eIterator, const void* pvBase, size_t szNumElements)
{
	std_linear_series_t stSeries;
	std_linear_series_construct(&stSeries, pvBase, pstIterator->szSizeofItem, szNumElements, false);
	return STD_ITERATOR_CALL(eContainer, eIterator, pfn_insert_after)(pstIterator, &stSeries);
}

#define std_insert_after(IT,...)							\
			std_iterator_call_insert_after(					\
				&IT.stItBody.stIterator,					\
				STD_ITERATOR_PARENT_ENUM_GET(IT),			\
				STD_ITERATOR_ENUM_GET_AND_CHECK(IT,next),	\
				STD_PUSH_DATA(V,__VA_ARGS__)	)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Insert a linear series of items before the current iterator
 *
 * @param[in]	pstIterator		Iterator
 * @param[in]	eContainer		The container type index
 * @param[in]	eIterator		Iterator type (e.g. forward or reverse)
 * @param[in]	pvBase			Linear series of items
 * @param[in]	szNumElements	Number of items
 *
 * @return Number of items inserted
 */
STD_INLINE size_t std_iterator_call_insert_before(std_iterator_t* pstIterator, std_container_enum_t eContainer, std_iterator_enum_t eIterator, const void* pvBase, size_t szNumElements)
{
	std_linear_series_t stSeries;
	std_linear_series_construct(&stSeries, pvBase, pstIterator->szSizeofItem, szNumElements, false);
	return STD_ITERATOR_CALL(eContainer, eIterator, pfn_insert_before)(pstIterator, &stSeries);
}

#define std_insert_before(IT,...)							\
			std_iterator_call_insert_before(				\
				&IT.stItBody.stIterator,					\
				STD_ITERATOR_PARENT_ENUM_GET(IT),			\
				STD_ITERATOR_ENUM_GET_AND_CHECK(IT,next),	\
				STD_PUSH_DATA(V,__VA_ARGS__)	)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Erase the item currently pointed to by an iterator
 *
 * @param[in]	pstIterator		Iterator
 * @param[in]	eContainer		The container type index
 */
STD_INLINE void std_iterator_call_erase(std_iterator_t* pstIterator, std_container_enum_t eContainer)
{
	STD_CONTAINER_CALL(eContainer, pfn_erase)(pstIterator);
}

#define std_erase(IT)						\
			std_iterator_call_erase(		\
				&V.stBody.stContainer,		\
				STD_ITERATOR_PARENT_ENUM_GET(IT,erase)	)	// FIXME: should be ..._AND_CHECK(

#endif /* STD_CONTAINER_H_ */
