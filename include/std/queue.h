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

#include "std/deque.h"

#define STD_QUEUE_DECLARE(T,HAS_ENUM)	STD_DEQUE(std_deque_t, std_deque_iterator_t, T, std_container_enum_queue, HAS_ENUM, std_queue_implements, STD_FAKEVAR())

#define std_queue(T)											STD_QUEUE_DECLARE(T,std_container_has_no_handlers)
#define std_queue_itemhandler(T)								STD_QUEUE_DECLARE(T,std_container_has_itemhandler)
#define std_queue_memoryhandler(T)								STD_QUEUE_DECLARE(T,std_container_has_memoryhandler)
#define std_queue_memoryhandler_itemhandler(T)					STD_QUEUE_DECLARE(T,std_container_has_memoryhandler_itemhandler)
#define std_queue_lockhandler(T)								STD_QUEUE_DECLARE(T,std_container_has_lockhandler)
#define std_queue_lockhandler_itemhandler(T)					STD_QUEUE_DECLARE(T,std_container_has_lockhandler_itemhandler)
#define std_queue_lockhandler_memoryhandler(T)					STD_QUEUE_DECLARE(T,std_container_has_lockhandler_memoryhandler)
#define std_queue_lockhandler_memoryhandler_itemhandler(T)		STD_QUEUE_DECLARE(T,std_container_has_lockhandler_memoryhandler_itemhandler)

enum
{
	std_queue_implements =
		( std_container_implements_name
		| std_container_implements_construct
		| std_container_implements_destruct
		| std_container_implements_push
		| std_container_implements_pop
		| std_container_implements_at
		| std_container_implements_default_itemhandler)
};

#define STD_QUEUE_JUMPTABLE \
	.pachContainerName = "queue",					\
	.pfn_construct		= &stdlib_deque_construct,	\
	.pfn_destruct		= &stdlib_deque_destruct,	\
	.pfn_push			= &stdlib_deque_push_front,	\
	.pfn_pop			= &stdlib_deque_pop_back,	\
	.pfn_at				= &stdlib_deque_at,			\
	.pstDefaultItemHandler = &std_deque_default_itemhandler,

#endif /* STD_QUEUE_H_ */
