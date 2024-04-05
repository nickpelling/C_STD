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
	for (size_t j = 0; j < szNum; j++)				\
		printf(" %d", aiNum[j]);					\
	printf(CRLF)

#define TEST_CONTAINER_NAME(CONTAINER,NAME)			\
	do {											\
		const char * pachContainerName = std_container_name(CONTAINER);	\
		if (strcmp(pachContainerName, NAME) != 0)	\
		{											\
			printf("Error: container name was \"%s\" (expected \"%s\") (line #%d)" CRLF, \
						pachContainerName, NAME, __LINE__);	\
			return false;							\
		}											\
	} while (0)

#define TEST_SAME(CONTAINER, NUM1, NUM2)			\
	do {											\
		if (NUM1 != NUM2)							\
		{											\
			printf("Error: %s: %d items encountered (%d expected) (line #%d)" CRLF,	\
				std_container_name(CONTAINER), (int) NUM1, (int) NUM2, __LINE__);	\
			return false;							\
		}											\
	} while (0)


#define TEST_SIZE(CONTAINER, NUM)					\
	do {											\
		if (std_size(CONTAINER) != NUM)				\
		{											\
			printf("Error: %s contained %d items (%d expected) (line #%d)" CRLF,		\
				std_container_name(CONTAINER), (int) std_size(CONTAINER), (int) NUM, __LINE__);	\
			return false;							\
		}											\
	} while (0)

#define TEST_ARRAY(POPARRAY,CMPARRAY)	\
	do {	\
		if (memcmp(POPARRAY, CMPARRAY, sizeof(CMPARRAY)) != 0)	\
		{	\
			printf("Popped array didn't contain the expected values (line #%d)" CRLF, __LINE__);	\
			return false;	\
		}	\
	} while (0)
	
#define READ_CONTAINER(CONTAINER, ITERATOR)			\
	do {											\
		i = 0;										\
		memset(aiPopped, 0, sizeof(aiPopped));		\
		for (ITERATOR(CONTAINER, it))				\
		{											\
			aiPopped[i++] = std_iterator_at(it)[0];	\
			if (i >= STD_NUM_ELEMENTS(aiPopped))	\
			{										\
				break;								\
			}										\
		}											\
	} while (0)

static const int ai12345[5] = { 1, 2, 3, 4, 5 };
static const int ai54321[5] = { 5, 4, 3, 2, 1 };

static bool vector_test(void)
{
	std_vector(int) v;
	int aiPopped[10];
	size_t szNum1;
	size_t i;

	// Construct a container, test the container name & that it starts empty
	std_construct(v);
	TEST_SIZE(v, 0);

	// Push 1/2/3/4/5 onto the back of the container, and make sure size == 5
	std_push_back(v, 1, 2, 3, 4, 5);
	TEST_SIZE(v, 5);

	// Iterate forwards
	READ_CONTAINER(v, std_each_forward);
	TEST_SAME(v, i, 5);
	TEST_ARRAY(aiPopped, ai12345);

	// Iterate forwards const
	READ_CONTAINER(v, std_each_forward_const);
	TEST_SAME(v, i, 5);
	TEST_ARRAY(aiPopped, ai12345);

	// Iterate backwards
	READ_CONTAINER(v, std_each_reverse);
	TEST_SAME(v, i, 5);
	TEST_ARRAY(aiPopped, ai54321);

	// Iterate backwards const
	READ_CONTAINER(v, std_each_reverse_const);
	TEST_SAME(v, i, 5);
	TEST_ARRAY(aiPopped, ai54321);

	// Pop 5/4/3/2/1 from the back of the container
	szNum1 = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum1, 5);
	TEST_ARRAY(aiPopped, ai54321);

	// Push 1/2/3/4/5 onto the front of the container, and make sure size == 5
	std_push_front(v, 1, 2, 3, 4, 5);
	TEST_SIZE(v, 5);

	// Pop 5/4/3/2/1 from the front of the container
	szNum1 = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum1, 5);
	TEST_ARRAY(aiPopped, ai54321);

	std_push_back(v, 5, 4, 3, 2, 1);
	TEST_SIZE(v, 5);
	std_sort(v, &int_compare);
	TEST_SIZE(v, 5);
	szNum1 = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum1, 5);
	TEST_ARRAY(aiPopped, ai54321);

	std_destruct(v);

	return true;
}

static bool list_test(void)
{
	std_list(int) list;

	TEST_CONTAINER_NAME(list, "list");
	std_construct(list);
	TEST_SIZE(list, 0);

	std_push_back(list, 2, 1, 4, 3, 5);
	TEST_SIZE(list, 5);

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

	return true;
}

static bool deque_test(void)
{
	std_deque(int) deque;

	TEST_CONTAINER_NAME(deque, "deque");
	std_construct(deque);
	TEST_SIZE(deque, 0);

	std_push_back(deque, 2, 1, 4, 3, 5);
	TEST_SIZE(deque, 5);

	PRINT_ALL(deque, "D1", std_each_forward);
	PRINT_ALL(deque, "D2", std_each_forward_const);
	PRINT_ALL(deque, "D3", std_each_reverse);
	PRINT_ALL(deque, "D4", std_each_reverse_const);

	POP_ALL(deque, "D5", std_pop_back, 5);

	std_destruct(deque);

	return true;
}

static bool queue_test(void)
{
	std_queue(int) queue;

	TEST_CONTAINER_NAME(queue, "queue");
	std_construct(queue);
	TEST_SIZE(queue, 0);

	std_push(queue, 2, 1, 4, 3, 5);
	TEST_SIZE(queue, 5);

	POP_ALL(queue, "Q1", std_pop, 5);

	std_destruct(queue);

	return true;
}

static bool stack_test(void)
{
	std_stack(int) stack;

	TEST_CONTAINER_NAME(stack, "stack");
	std_construct(stack);
	TEST_SIZE(stack, 0);

	std_push(stack, 2, 1, 4, 3, 5);
	TEST_SIZE(stack, 5);

	POP_ALL(stack, "S1", std_pop, 5);

	std_destruct(stack);

	return true;
}

typedef std_list(int) list_int_t;

static bool vector_of_lists_test(void)
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

	return true;
}

// -----------------------------------------------------------------

int main(int argc, char * argv[])
{
	const char * pachArg;
	bool bStatus;

	if (argc < 2)
	{
#if 0
		fprintf(stderr, "Too few arguments on command line" CRLF);
		return EXIT_FAILURE;
#else
		pachArg = "vector";
#endif
	}
	else
	{
		pachArg = argv[1];
	}

	if (strcmp(pachArg, "vector") == 0)
	{
		bStatus = vector_test();
	}
	else if (strcmp(pachArg, "list") == 0)
	{
		bStatus = list_test();
	}
	else if (strcmp(pachArg, "deque") == 0)
	{
		bStatus = deque_test();
	}
	else if (strcmp(pachArg, "queue") == 0)
	{
		bStatus = queue_test();
	}
	else if (strcmp(pachArg, "stack") == 0)
	{
		bStatus = stack_test();
	}
	else if (strcmp(pachArg, "nested") == 0)
	{
		bStatus = vector_of_lists_test();
	}
	else
	{
		bStatus = false;
	}

	return bStatus ? EXIT_SUCCESS : EXIT_FAILURE;
}
