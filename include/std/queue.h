/*
 * std/queue.h
 *
 *  Created on: 17 Oct 2022
 *      Author: Nick Pelling
 */

#ifndef STD_QUEUE_H_
#define STD_QUEUE_H_

#include "std/deque.h"

#define STD_QUEUE_DECLARE(T,HAS_ENUM)	STD_DEQUE(std_deque_t, std_deque_iterator_t, T, std_container_enum_queue, HAS_ENUM, std_queue_implements)

#define std_queue(T)					STD_QUEUE_DECLARE(T,std_container_has_no_handlers)
#define std_queue_handlers(T,HAS_ENUM)	STD_QUEUE_DECLARE(T,HAS_ENUM)

enum
{
	std_queue_implements =
		( std_container_implements_name
		| std_container_implements_init
		| std_container_implements_push
		| std_container_implements_pop
		| std_container_implements_at
		| std_container_implements_empty)
};

#define STD_QUEUE_JUMPTABLE \
	.pachContainerName = "queue",					\
	.pfn_init			= &stdlib_deque_init,		\
	.pfn_push			= &stdlib_deque_push_front,	\
	.pfn_pop			= &stdlib_deque_pop_back,	\
	.pfn_at				= &stdlib_deque_at,			\
	.pfn_empty			= &stdlib_deque_empty,

#endif /* STD_QUEUE_H_ */
