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
#include "std/typelist.h"
#include "std/vector.h"

#define CRLF	"\r\n"

static int int_compare(const int* a, const int* b)
{
    return (*b < *a);
}

static bool int_equal(const int* a, const int* b)
{
    return (*b == *a);
}

#if 0
void typeof_test(void)
{
	uint32_t u32;
	 int32_t i32;
	uint16_t u16;
	 int16_t i16;

#define P(RESULT)	printf("res = %d" CRLF, (int)(RESULT));
#define TEST1(X)	P(STD_TYPEOF(X + 0U) == STD_TYPEOF(X))
#define TEST2(X)	P(STD_TYPEOF(X) == STD_TYPEOF(X + 0U))
#define TEST3(X)	P(STD_TYPEOF(X) == STD_TYPEOF(0U + X))
#define TEST4(X)	P(STD_TYPEOF(X + 0) == STD_TYPEOF(X + 0U))
#define TEST5(X)	P(STD_TYPEOF(0 + X) == STD_TYPEOF(0U + X))

	TEST1(u16);
	TEST2(u16);
	TEST3(u16);
	TEST4(u16);
	TEST5(u16);
}
#endif

#if 0
typedef TYPELIST(int, char, float) myTypeList;
typedef TYPELIST_APPEND(myTypeList, unsigned int) myTypeList2;
typedef TYPELIST_REMOVELAST(myTypeList2) myTypeList3;

void typelist_test(void)
{
	TYPELIST_TYPE1(myTypeList) a;
	TYPELIST_TYPE2(myTypeList) b;
	TYPELIST_TYPE3(myTypeList) c;
	TYPELIST_TYPE4(myTypeList2) d;

	a = 0;
	b = '0';
	c = 0.0;
	d = 0U;
	if (a || b || c || d) { /* lint unused */ }
}
#endif

void vector_test(void)
{
	std_vector(int) v;
	bool bSame;

	printf("V0: v is a %s" CRLF, std_container_name(v));

	std_construct(v);
	std_reserve(v, 5);
	for (int i = 0; i < 5; i++)
		std_push_back(v)[0] = (i + 2) ^ 1;
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

	std_vector(int) v2;
	std_construct(v2);
	std_push_back(v)[0] = 25;
}

void list_test(void)
{
	std_list(int) list;

	printf("L0: list is a %s" CRLF, std_container_name(list));

	std_construct(list);
	for (int i = 0; i < 5; i++)
		std_push_back(list)[0] = (i + 2) ^ 1;

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
}

void deque_test(void)
{
	std_deque(int) deque;

	printf("D0: deque is a %s" CRLF, std_container_name(deque));

	std_construct(deque);

	printf("D1: ");
	for (int i = 0; i < 5; i++)
		std_push_back(deque)[0] = (i + 2) ^ 1;
	while (!std_empty(deque))
	{
		int iTemp;
		std_pop_front(deque, &iTemp);
		printf("%d ", iTemp);
	}
	printf(CRLF);

	printf("D2: ");
	for (int i = 0; i < 5; i++)
		std_push_front(deque)[0] = (i + 2) ^ 1;
	while (!std_empty(deque))
	{
		int iTemp;
		std_pop_back(deque, &iTemp);
		printf("%d ", iTemp);
	}
	printf(CRLF);

	printf("D3: ");
	for (int i = 0; i < 5; i++)
		std_push_back(deque)[0] = (i + 2) ^ 1;
	for_each_const(deque, it)
		printf("%d ", std_iterator_at(it));
	printf(CRLF);

	printf("D4: ");
	for_each_const_reverse(deque, it)
		printf("%d ", std_iterator_at(it));
	printf(CRLF);

}

void queue_test(void)
{
	std_queue(int) queue;
	int num;

	printf("Q0: queue is a %s" CRLF, std_container_name(queue));

	std_construct(queue);
	for (int i = 0; i < 5; i++)
		std_push(queue)[0] = (i + 2) ^ 1;

	printf("Q1: ");
	while (!std_empty(queue))
	{
		printf("%d ", std_pop(queue, &num)[0]);
	}
	printf(CRLF);
}

void stack_test(void)
{
	std_stack(int) stack;
	int num;

	printf("S0: stack is a %s" CRLF, std_container_name(stack));

	std_construct(stack);
	for (int i = 0; i < 5; i++)
		std_push(stack)[0] = (i + 2) ^ 1;

	printf("S1: ");
	while (!std_empty(stack))
	{
		printf("%d ", std_pop(stack, &num)[0]);
	}
	printf(CRLF);
}

int main(void)
{
	if (true)
	{
		vector_test();
	}
	list_test();
	deque_test();
	queue_test();
	stack_test();

	return EXIT_SUCCESS;
}
