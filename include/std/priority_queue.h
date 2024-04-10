/*
 * std/priority_queue.h

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

#ifndef STD_PRIORITY_QUEUE_H_
#define STD_PRIORITY_QUEUE_H_

#include "std/config.h"
#include "std/vector.h"

typedef struct
{
	std_container_t stContainer;
	STD_VECTOR_FIELDS;
	int (*pfnCompare)(const void* a, const void* b);
} std_priorityqueue_t;

#define STD_PRIORITYQUEUE_DECLARE(T,HAS_ENUM)	\
		STD_VECTOR(std_priorityqueue_t, std_vector_iterator_t, T, std_container_enum_priorityqueue, HAS_ENUM, std_priorityqueue_implements, STD_FAKEVAR())

#define std_priorityqueue(T)											STD_PRIORITYQUEUE_DECLARE(T,std_container_has_no_handlers)
#define std_priorityqueue_itemhandler(T)								STD_PRIORITYQUEUE_DECLARE(T,std_container_has_itemhandler)
#define std_priorityqueue_memoryhandler(T)								STD_PRIORITYQUEUE_DECLARE(T,std_container_has_memoryhandler)
#define std_priorityqueue_memoryhandler_itemhandler(T)					STD_PRIORITYQUEUE_DECLARE(T,std_container_has_memoryhandler_itemhandler)
#define std_priorityqueue_lockhandler(T)								STD_PRIORITYQUEUE_DECLARE(T,std_container_has_lockhandler)
#define std_priorityqueue_lockhandler_itemhandler(T)					STD_PRIORITYQUEUE_DECLARE(T,std_container_has_lockhandler_itemhandler)
#define std_priorityqueue_lockhandler_memoryhandler(T)					STD_PRIORITYQUEUE_DECLARE(T,std_container_has_lockhandler_memoryhandler)
#define std_priorityqueue_lockhandler_memoryhandler_itemhandler(T)		STD_PRIORITYQUEUE_DECLARE(T,std_container_has_lockhandler_memoryhandler_itemhandler)

extern size_t stdlib_priorityqueue_push(std_container_t* pstContainer, std_linear_series_t* pstSeries);

extern void stdlib_priorityqueue_compare_set(std_priorityqueue_t* pstPriorityQueue, pfn_std_compare_t pfnCompare);

#define std_priorityqueue_compare_set(PRIORITYQUEUE,COMPARE)				\
	(																		\
		STD_STATIC_ASSERT(STD_TYPES_ARE_SAME(COMPARE, PRIORITYQUEUE.pfnCompare), STD_CONCAT(Incompatible_comparison_functions_,__COUNTER__)), \
		stdlib_priorityqueue_compare_set(&PRIORITYQUEUE.stBody,(pfn_std_compare_t)(COMPARE))	\
	)

enum
{
	std_priorityqueue_implements =
	(std_container_implements_name
		| std_container_implements_construct
		| std_container_implements_destruct
		| std_container_implements_push
		| std_container_implements_pop
		| std_container_implements_at
		| std_container_implements_default_itemhandler)
};

#define STD_PRIORITYQUEUE_JUMPTABLE \
	.pachContainerName = "priority queue",				\
	.pfn_construct		= &stdlib_vector_construct,		\
	.pfn_destruct		= &stdlib_vector_destruct,		\
	.pfn_push			= &stdlib_priorityqueue_push,	\
	.pfn_pop			= &stdlib_vector_pop_front,		\
	.pfn_at				= &stdlib_vector_at,			\
	.pstDefaultItemHandler = &std_vector_default_itemhandler,

#endif /* STD_PRIORITY_QUEUE_H_ */
