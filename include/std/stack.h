/*
 * std/stack.h
 *
 *  Created on: 17 Oct 2022
 *      Author: Nick Pelling
 */

#ifndef STD_STACK_H_
#define STD_STACK_H_

#include "std/deque.h"

#define STD_STACK_DECLARE(T,HAS_ENUM)	STD_DEQUE(std_deque_t, std_deque_iterator_t, T, std_container_enum_stack, HAS_ENUM, std_stack_implements)

#define std_stack(T)					STD_STACK_DECLARE(T,std_container_has_no_handlers)
#define std_stack_handlers(T,HAS_ENUM)	STD_STACK_DECLARE(T,HAS_ENUM)

enum
{
	std_stack_implements =
		( std_container_implements_name
		| std_container_implements_init
		| std_container_implements_pushpop
		| std_container_implements_at
		| std_container_implements_empty)
};

#define STD_STACK_JUMPTABLE \
	.pachContainerName = "stack",					\
	.pfn_init			= &stdlib_deque_init,		\
	.pfn_push			= &stdlib_deque_push_back,	\
	.pfn_pop			= &stdlib_deque_pop_back,	\
	.pfn_at				= &stdlib_deque_at,			\
	.pfn_empty			= &stdlib_deque_empty,

#endif /* STD_STACK_H_ */
