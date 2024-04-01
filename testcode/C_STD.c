/*
 * C_STD.c
 * C_STD library test code
 * Nick Pelling, 12th October 2022
 */

#include <stdio.h>
#include <stdlib.h>

#include "std/container.h"

#define CRLF	"\r\n"

static int int_compare(const int* a, const int* b)
{
    return (*b < *a);
}

#define PRINT_ALL(CONTAINER, MSG, ITERATOR)		\
	printf("%s:", MSG);							\
	for (ITERATOR(CONTAINER, it))				\
		printf(" %d", std_iterator_at(it)[0]);	\
	printf(CRLF)

#define POP_ALL(CONTAINER, MSG, POPFUNC, MAXITEMS)	\
	int aiNum[MAXITEMS];							\
	size_t szNum = POPFUNC(CONTAINER, aiNum, STD_NUM_ELEMENTS(aiNum));	\
	printf("%s:", MSG);								\
	for (size_t i = 0; i < szNum; i++)				\
		printf(" %d", aiNum[i]);					\
	printf(CRLF)

void vector_test(void)
{
	std_vector(int) v;

	printf("V0: v is a %s" CRLF, std_container_name(v));

	std_construct(v);
	std_reserve(v, 5);
	std_push_back(v, 2, 1, 4, 3, 5);

	printf("%s:", "V1");
	for (std_lock_state_t _fakevar_2141 = std_container_lock(
		&v.stBody.stContainer,
		((std_container_has_t)(sizeof(v.pau8HasHandler[0]) - 1U)),
		0);
		_fakevar_2141 != e_std_lock_Invalid;
		_fakevar_2141 = (std_container_lock_restore(
			&v.stBody.stContainer,
			((std_container_has_t)(sizeof(v.pau8HasHandler[0]) - 1U)),
			_fakevar_2141),
		e_std_lock_Invalid))
		
		for (__typeof__(v.pstForwardIterator[0]) it, *_fakevar_2142 = (std_iterator_call_construct(
			&v.stBody.stContainer,
			((std_container_enum_t)(sizeof(it.puParent[0].pau8ContainerEnum[0]) - 1U)),
			((std_container_has_t)(sizeof(it.puParent[0].pau8HasHandler[0]) - 1U)),
			((void)sizeof(struct {
				int construct_is_not_implemented_for_this_type_of_iterator_2143 : ((((std_container_implements_t)(sizeof(it.puParent[0].pau8Implements[0]) - 1U))& ((((std_iterator_enum_t)(sizeof(it.pau8IteratorEnum[0]) - 1U)) == std_iterator_enum_forward) ? std_container_implements_forward_construct : std_container_implements_reverse_construct)) ? 1 : -1);
			}),
			((std_iterator_enum_t)(sizeof(it.pau8IteratorEnum[0]) - 1U))),
			&it.stItBody.stIterator),
			((void*)0));
			!it.stItBody.stIterator.bDone;
			(void)_fakevar_2142,
			std_iterator_call_next(
					&it.stItBody.stIterator,
					((std_container_enum_t)(sizeof(it.puParent[0].pau8ContainerEnum[0]) - 1U)),
					((std_container_has_t)(sizeof(it.puParent[0].pau8HasHandler[0]) - 1U)),
					((void)sizeof(struct _fakevar_2146
					{
						int next_is_not_implemented_for_this_type_of_iterator_2145 : ((((std_container_implements_t)(sizeof(it.puParent[0].pau8Implements[0]) - 1U))& ((((std_iterator_enum_t)(sizeof(it.pau8IteratorEnum[0]) - 1U)) == std_iterator_enum_forward) ? std_container_implements_forward_next : std_container_implements_reverse_next)) ? 1 : -1);
					}),
					((std_iterator_enum_t)(sizeof(it.pau8IteratorEnum[0]) - 1U)))))
			
			printf(" %d", ((__typeof__(it.pstType[0])*)(it.stItBody.stIterator.pvRef))[0]);
	
	printf("\r\n");

	PRINT_ALL(v, "V1", std_each_forward);
	PRINT_ALL(v, "V2", std_each_forward_const);
	PRINT_ALL(v, "V3", std_each_reverse);
	PRINT_ALL(v, "V4", std_each_reverse_const);

	std_sort(v, &int_compare);
	PRINT_ALL(v, "V5", std_each);

	POP_ALL(v, "V6", std_pop_back, 5);

	std_destruct(v);
}

void list_test(void)
{
	std_list(int) list;

	printf("L0: list is a %s" CRLF, std_container_name(list));

	std_construct(list);
	std_push_back(list, 2, 1, 4, 3, 5);

	PRINT_ALL(list, "L1", std_each_forward);
	PRINT_ALL(list, "L2", std_each_forward_const);
	PRINT_ALL(list, "L3", std_each_reverse);
	PRINT_ALL(list, "L4", std_each_reverse_const);

	for (std_each(list, it))
	{
		if (std_iterator_at(it)[0] == 4)
		{
			std_insert_before(it, -3);
			std_insert_after(it, 6);
			break;
		}
	}

	POP_ALL(list, "L5", std_pop_back, 10);

	std_destruct(list);
}

void deque_test(void)
{
	std_deque(int) deque;

	printf("D0: deque is a %s" CRLF, std_container_name(deque));

	std_construct(deque);
	std_push_back(deque, 2, 1, 4, 3, 5);

	PRINT_ALL(deque, "D1", std_each_forward);
	PRINT_ALL(deque, "D2", std_each_forward_const);
	PRINT_ALL(deque, "D3", std_each_reverse);
	PRINT_ALL(deque, "D4", std_each_reverse_const);

	POP_ALL(deque, "D5", std_pop_back, 5);

	std_destruct(deque);
}

void queue_test(void)
{
	std_queue(int) queue;

	printf("Q0: queue is a %s" CRLF, std_container_name(queue));

	std_construct(queue);
	std_push(queue, 2, 1, 4, 3, 5);

	POP_ALL(queue, "Q1", std_pop, 5);

	std_destruct(queue);
}

void stack_test(void)
{
	std_stack(int) stack;

	printf("S0: stack is a %s" CRLF, std_container_name(stack));

	std_construct(stack);
	std_push(stack, 2, 1, 4, 3, 5);

	POP_ALL(stack, "S1", std_pop, 5);

	std_destruct(stack);
}

typedef std_list(int) list_int_t;

void vector_of_lists_test(void)
{
	std_vector_itemhandler(list_int_t) v;
	std_construct_itemhandler(v, std_container_default_itemhandler(STD_ITEM(v)));

	list_int_t list1;
	std_construct(list1);
	std_push_back(list1, 1, 2, 3);
	std_push_back(v, list1);

	list_int_t list2;
	std_construct(list2);
	std_push_back(list2, 4, 3, 2, 1);
	std_push_back(v, list2);

	printf("veclist sizes =");
	for (std_each_const(v, it))
	{
		const list_int_t* list3 = std_iterator_at(it);
		printf(" %d", (int)std_size(list3[0]));
	}
	printf(CRLF);

	std_destruct(v);
}

int main(void)
{
	vector_test();
	list_test();
	deque_test();
	queue_test();
	stack_test();
	vector_of_lists_test();

	return EXIT_SUCCESS;
}
