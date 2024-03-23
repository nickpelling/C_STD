/*
 * C_STD.c
 * C_STD library test code
 * Nick Pelling, 12th October 2022
 */

#include <stdio.h>
#include <stdlib.h>

#include "std/container.h"
#include "std/deque.h"
#include "std/list.h"
#include "std/queue.h"
#include "std/stack.h"
#include "std/vector.h"

#define CRLF	"\r\n"

static int int_compare(const int* a, const int* b)
{
    return (*b < *a);
}

void vector_test(void)
{
	std_vector(int) v;

	printf("V0: v is a %s" CRLF, std_container_name(v));

	std_construct(v);
	std_reserve(v, 5);
	std_push_back(v, 2, 1, 4, 3, 5);
	std_fit(v);

	printf("V1: ");
	for_each(v, it)
		printf("%d ", std_iterator_at(it));
	printf(CRLF);

	std_sort(v, &int_compare);

	printf("V2: ");
	for_each(v, it)
		printf("%d ", std_iterator_at(it));
	printf(CRLF);

	std_destruct(v);

	std_vector(int) v2;
	std_construct(v2);
	std_push_back(v, 25);
	std_destruct(v2);
}

void list_test(void)
{
	std_list(int) list;

	printf("L0: list is a %s" CRLF, std_container_name(list));

	std_construct(list);
	std_push_back(list, 2, 1, 4, 3, 5);

	printf("L1: ");
	for_each(list, it)
		printf("%d ", std_iterator_at(it));
	printf(CRLF);

	printf("L2: ");
	for_each_const(list, it2)
		printf("%d ", std_iterator_at(it2));
	printf(CRLF);

	printf("L3: ");
	for_each_reverse(list, it2)
		printf("%d ", std_iterator_at(it2));
	printf(CRLF);

	std_destruct(list);
}

void deque_test(void)
{
	std_deque(int) deque;
	int iTemp;

	printf("D0: deque is a %s" CRLF, std_container_name(deque));

	std_construct(deque);

	printf("D1: ");
	std_push_back(deque, 2, 1, 4, 3, 5);
	while (!std_empty(deque))
	{
		std_pop_front(deque, &iTemp, 1);
		printf("%d ", iTemp);
	}
	printf(CRLF);

	printf("D2: ");
	std_push_front(deque, 2, 1, 4, 3, 5);
	while (!std_empty(deque))
	{
		std_pop_back(deque, &iTemp, 1);
		printf("%d ", iTemp);
	}
	printf(CRLF);

	printf("D3: ");
	std_push_back(deque, 2, 1, 4, 3, 5);
	for_each_const(deque, it)
		printf("%d ", std_iterator_at(it));
	printf(CRLF);

	printf("D4: ");
	for_each_const_reverse(deque, it)
		printf("%d ", std_iterator_at(it));
	printf(CRLF);

	std_destruct(deque);
}

void queue_test(void)
{
	std_queue(int) queue;
	int num;

	printf("Q0: queue is a %s" CRLF, std_container_name(queue));

	std_construct(queue);
	std_push(queue, 2, 1, 4, 3, 5);

	printf("Q1: ");
	while (!std_empty(queue))
	{
		std_pop(queue, &num, 1);
		printf("%d ", num);
	}
	printf(CRLF);

	std_destruct(queue);
}

void stack_test(void)
{
	std_stack(int) stack;
	int num;

	printf("S0: stack is a %s" CRLF, std_container_name(stack));

	std_construct(stack);
	std_push(stack, 2, 1, 4, 3, 5);

	printf("S1: ");
	while (!std_empty(stack))
	{
		std_pop(stack, &num, 1);
		printf("%d ", num);
	}
	printf(CRLF);

	std_destruct(stack);
}

typedef std_list(int) list_int_t;

void vector_of_lists_test(void)
{
	int num;

	std_vector(list_int_t) v;
	std_construct(v);

	list_int_t list1;
	std_construct(list1);
	std_push_back(list1, 1, 2, 3);
	std_push_back(v, list1);

	list_int_t list2;
	std_construct(list2);
	std_push_back(list2, 4, 3, 2, 1);
	std_push_back(v, list2);

	list_int_t* list3 = std_at(v, 0);
	num = std_size(list3[0]);
	printf("veclist#0 size = %d" CRLF, num);

	list_int_t * list4 = std_at(v, 1);
	num = std_size(list4[0]);
	printf("veclist#1 size = %d" CRLF, num);

	std_destruct(v);	// FIXME: the contained lists will need an item handler with a deconstructor!
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
