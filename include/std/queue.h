/*
 * std/queue.h

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

#ifndef STD_QUEUE_H_
#define STD_QUEUE_H_

#include "std/container.h"

 /**
  * Enqueue a linear series of items onto a queue container
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
STD_INLINE size_t std_container_call_enqueue(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, const void* pvBase, size_t szNumElements)
{
	std_linear_series_t stSeries;
	std_linear_series_construct(&stSeries, pvBase, pstContainer->szSizeofItem, szNumElements, false);
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	size_t szNumPushed = STD_CONTAINER_CALL(eContainer, pfn_push_back)(pstContainer, &stSeries);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return szNumPushed;
}

#define std_enqueue(V,...)						\
			std_container_call_enqueue(		\
				&V.stBody.stContainer,		\
				STD_CONTAINER_ENUM_GET_AND_CHECK(V,enqueue_dequeue),	\
				STD_CONTAINER_HAS_GET(V),	\
				STD_PUSH_DATA(V,__VA_ARGS__)	)

// - - - - - - - - - - - - - - - - - - - - - - - - - - - -

/**
 * Dequeue a number of items from a queue container into a linear area of memory
 *
 * @param[in]	pstContainer	The container
 * @param[in]	eContainer		The container type index
 * @param[in]	eHas			Bitmask of flags denoting which handlers this container has
 * @param[out]	pvBase			Start of the destination memory area (can be NULL)
 * @param[in]	szMaxItems		Maximum number of items allowed in the linear series
 *
 * @return Number of items actually popped from the container
 */
STD_INLINE size_t std_container_call_dequeue(std_container_t* pstContainer, std_container_enum_t eContainer, std_container_has_t eHas, void* pvResult, size_t szMaxItems)
{
	std_lock_state_t eOldState = std_container_lock_for_writing(pstContainer, eHas);
	size_t szNum = STD_CONTAINER_CALL(eContainer, pfn_pop_front)(pstContainer, pvResult, szMaxItems);
	std_container_lock_restore(pstContainer, eHas, eOldState);
	return szNum;
}

#define std_dequeue(V,RESULT,MAXITEMS)		\
	(										\
		STD_CHECK_TYPE(V, (RESULT)[0], pop_result_parameter), \
		std_container_call_dequeue(			\
			&V.stBody.stContainer,			\
			STD_CONTAINER_ENUM_GET_AND_CHECK(V,enqueue_dequeue),	\
			STD_CONTAINER_HAS_GET(V),		\
			RESULT,							\
			MAXITEMS)						\
	)

#define std_queue(CONTAINER,TYPE)	CONTAINER(TYPE,std_queue_implements)

enum
{
	std_queue_implements =
		( std_container_implements_name
		| std_container_implements_construct
		| std_container_implements_destruct
		| std_container_implements_enqueue_dequeue
		| std_container_implements_default_itemhandler)
};

#endif /* STD_QUEUE_H_ */
