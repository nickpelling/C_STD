/*
 * std/stack.h

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

#ifndef STD_STACK_H_
#define STD_STACK_H_

#include "std/vector.h"

#define STD_STACK_DECLARE(T,HAS_ENUM)	STD_VECTOR(std_vector_t, std_vector_iterator_t, T, std_container_enum_stack, HAS_ENUM, std_stack_implements, STD_FAKEVAR())

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
	.pfn_construct		= &stdlib_vector_construct,	\
	.pfn_destruct		= &stdlib_vector_destruct,	\
	.pfn_push			= &stdlib_vector_push_back,	\
	.pfn_pop			= &stdlib_vector_pop_back,	\
	.pfn_at				= &stdlib_vector_at,		\
	.pstDefaultItemHandler = &std_vector_default_itemhandler,

#endif /* STD_STACK_H_ */
