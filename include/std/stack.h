/*
 * std/stack.h
 *
 *  Created on: 17 Oct 2022
 *      Author: Nick Pelling
 */

#ifndef STD_STACK_H_
#define STD_STACK_H_

#include "std/deque.h"

#define STD_STACK_DECLARE(T,HAS_ENUM)	STD_DEQUE(std_deque_t, std_deque_iterator_t, T, std_container_enum_stack, HAS_ENUM, std_stack_implements, STD_FAKEVAR())

#define std_stack(T)											STD_STACK_DECLARE(T,std_container_has_no_handlers)
#define std_stack_itemhandler(T)								STD_STACK_DECLARE(T,std_container_has_itemhandler)
#define std_stack_memoryhandler(T)								STD_STACK_DECLARE(T,std_container_has_memoryhandler)
#define std_stack_memoryhandler_itemhandler(T)					STD_STACK_DECLARE(T,std_container_has_memoryhandler_itemhandler)
#define std_stack_lockhandler(T)								STD_STACK_DECLARE(T,std_container_has_lockhandler)
#define std_stack_lockhandler_itemhandler(T)					STD_STACK_DECLARE(T,std_container_has_lockhandler_itemhandler)
#define std_stack_lockhandler_memoryhandler(T)					STD_STACK_DECLARE(T,std_container_has_lockhandler_memoryhandler)
#define std_stack_lockhandler_memoryhandler_itemhandler(T)		STD_STACK_DECLARE(T,std_container_has_lockhandler_memoryhandler_itemhandler)

enum
{
	std_stack_implements =
		( std_container_implements_name
		| std_container_implements_construct
		| std_container_implements_destruct
		| std_container_implements_pushpop
		| std_container_implements_at
		| std_container_implements_default_itemhandler)
};

#define STD_STACK_JUMPTABLE \
	.pachContainerName = "stack",					\
	.pfn_construct		= &stdlib_deque_construct,	\
	.pfn_destruct		= &stdlib_deque_destruct,	\
	.pfn_push			= &stdlib_deque_push_back,	\
	.pfn_pop			= &stdlib_deque_pop_back,	\
	.pfn_at				= &stdlib_deque_at,			\
	.pstDefaultItemHandler = &std_deque_default_itemhandler,

#endif /* STD_STACK_H_ */
