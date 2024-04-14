/*
 * std/ring.h

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

#ifndef STD_RING_H_
#define STD_RING_H_

#include "std/vector.h"

typedef struct
{
	std_container_t stContainer;
	STD_VECTOR_FIELDS;
	size_t szStartOffset;		// Offset of start of ring data within vector
} std_ring_t;

#define STD_RING_DECLARE(T,HAS_ENUM)	STD_VECTOR(std_ring_t, std_vector_iterator_t, T, std_container_enum_ring, HAS_ENUM, std_ring_implements, STD_FAKEVAR())

#define std_ring(T)											STD_RING_DECLARE(T,std_container_has_no_handlers)
#define std_ring_itemhandler(T)								STD_RING_DECLARE(T,std_container_has_itemhandler)
#define std_ring_memoryhandler(T)							STD_RING_DECLARE(T,std_container_has_memoryhandler)
#define std_ring_memoryhandler_itemhandler(T)				STD_RING_DECLARE(T,std_container_has_memoryhandler_itemhandler)
#define std_ring_lockhandler(T)								STD_RING_DECLARE(T,std_container_has_lockhandler)
#define std_ring_lockhandler_itemhandler(T)					STD_RING_DECLARE(T,std_container_has_lockhandler_itemhandler)
#define std_ring_lockhandler_memoryhandler(T)				STD_RING_DECLARE(T,std_container_has_lockhandler_memoryhandler)
#define std_ring_lockhandler_memoryhandler_itemhandler(T)	STD_RING_DECLARE(T,std_container_has_lockhandler_memoryhandler_itemhandler)

enum
{
	std_ring_implements =
		( std_container_implements_name
		| std_container_implements_construct
		| std_container_implements_destruct
		| std_container_implements_pushpop_front
		| std_container_implements_pushpop_back
		| std_container_implements_at
		| std_container_implements_reserve
		| std_container_implements_ranged_sort
		| std_container_implements_forward_constructnextprev
		| std_container_implements_forward_range
		| std_container_implements_reverse_constructnextprev
		| std_container_implements_reverse_range
		| std_container_implements_default_itemhandler )
};

#define STD_RING_JUMPTABLE \
	.pachContainerName = "ring",						\
	.pfn_construct		= &stdlib_vector_construct,		\
	.pfn_destruct		= &stdlib_vector_destruct,		\
	.pfn_push_front		= &stdlib_vector_push_front,	\
	.pfn_push_back		= &stdlib_vector_push_back,		\
	.pfn_pop_front		= &stdlib_vector_pop_front,		\
	.pfn_pop_back		= &stdlib_vector_pop_back,		\
	.pfn_at				= &stdlib_vector_at,			\
	.pfn_reserve		= &stdlib_vector_reserve,		\
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
	},													\
	.pstDefaultItemHandler = &std_vector_default_itemhandler

#endif /* STD_RING_H_ */
