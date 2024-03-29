/*
 * std/queue.h
 *
 *  Created on: 17 Oct 2022
 *      Author: Nick Pelling
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
