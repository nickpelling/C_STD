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

#define PRINT_ALL(CONTAINER, MSG, ITERATOR)	\
	printf("%s:", MSG);						\
	ITERATOR(CONTAINER, it)					\
		printf(" %d", std_iterator_at(it));	\
	printf(CRLF)


void vector_test(void)
{
	std_vector(int) v;
	int aiNum[5];
	size_t szNum;

	printf("V0: v is a %s" CRLF, std_container_name(v));

	std_construct(v);
	std_reserve(v, 5);
	std_push_back(v, 2, 1, 4, 3, 5);

	PRINT_ALL(v, "V1", std_for_each_forward);
	PRINT_ALL(v, "V2", std_for_each_forward_const);
	PRINT_ALL(v, "V3", std_for_each_reverse);
	PRINT_ALL(v, "V4", std_for_each_reverse_const);

	std_sort(v, &int_compare);

	PRINT_ALL(v, "V5", std_for_each);

	szNum = std_pop_back(v, aiNum, STD_NUM_ELEMENTS(aiNum));
	printf("V6:");
	for (size_t i = 0; i < szNum; i++)
		printf(" %d", aiNum[i]);
	printf(CRLF);

	std_destruct(v);
}

void list_test(void)
{
	std_list(int) list;
	int aiNum[5];
	size_t szNum;

	printf("L0: list is a %s" CRLF, std_container_name(list));

	std_construct(list);
	std_push_back(list, 2, 1, 4, 3, 5);

	PRINT_ALL(list, "L1", std_for_each_forward);
	PRINT_ALL(list, "L2", std_for_each_forward_const);
	PRINT_ALL(list, "L3", std_for_each_reverse);
	PRINT_ALL(list, "L4", std_for_each_reverse_const);

	szNum = std_pop_back(list, aiNum, STD_NUM_ELEMENTS(aiNum));
	printf("L5:");
	for (size_t i = 0; i < szNum; i++)
		printf(" %d", aiNum[i]);
	printf(CRLF);

	std_destruct(list);
}

void deque_test(void)
{
	std_deque(int) deque;
	int aiNum[5];
	size_t szNum;

	printf("D0: deque is a %s" CRLF, std_container_name(deque));

	std_construct(deque);

	std_push_back(deque, 2, 1, 4, 3, 5);

	PRINT_ALL(deque, "L1", std_for_each_forward);
	PRINT_ALL(deque, "L2", std_for_each_forward_const);
	PRINT_ALL(deque, "L3", std_for_each_reverse);
	PRINT_ALL(deque, "L4", std_for_each_reverse_const);

	printf("D5:");
	szNum = std_pop_back(deque, aiNum, STD_NUM_ELEMENTS(aiNum));
	for (size_t i = 0; i < szNum; i++)
		printf(" %d", aiNum[i]);
	printf(CRLF);

	std_destruct(deque);
}

void queue_test(void)
{
	std_queue(int) queue;
	int aiNum[5];
	size_t szNum;

	printf("Q0: queue is a %s" CRLF, std_container_name(queue));

	std_construct(queue);
	std_push(queue, 2, 1, 4, 3, 5);

	printf("Q1:");
	szNum = std_pop(queue, aiNum, STD_NUM_ELEMENTS(aiNum));
	for (size_t i = 0; i < szNum; i++)
		printf(" %d", aiNum[i]);
	printf(CRLF);

	std_destruct(queue);
}

void stack_test(void)
{
	std_stack(int) stack;
	int aiNum[5];
	size_t szNum;

	printf("S0: stack is a %s" CRLF, std_container_name(stack));

	std_construct(stack);
	std_push(stack, 2, 1, 4, 3, 5);

	printf("S1:");
	szNum = std_pop(stack, aiNum, STD_NUM_ELEMENTS(aiNum));
	for (size_t i = 0; i < szNum; i++)
		printf(" %d", aiNum[i]);
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

	std_destruct(v);	// FIXME: the contained lists will need an item handler with a destructor!
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
