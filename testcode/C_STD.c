/*
 * C_STD.c

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

#include <stdio.h>
#include <stdlib.h>

#include "std/container.h"
#include "std/stack.h"
#include "std/queue.h"

#define CRLF	"\r\n"

static int int_compare(const int* a, const int* b)
{
    return (*b < *a);
}

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
	size_t szNum;
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
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	// Push 1/2/3/4/5 onto the front of the container, and make sure size == 5
	std_push_front(v, 1, 2, 3, 4, 5);
	TEST_SIZE(v, 5);

	// Pop 5/4/3/2/1 from the front of the container
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	std_push_back(v, 5, 4, 3, 2, 1);
	TEST_SIZE(v, 5);
	std_sort(v, &int_compare);
	TEST_SIZE(v, 5);
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	szNum = 0;
	for (i = 0; i < 5; i++)
	{
		szNum += std_push_back(v, ai12345[i]);
	}
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 5);

	memset(aiPopped, 0, sizeof(aiPopped));
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	szNum = 0;
	for (i = 0; i < 5; i++)
	{
		szNum += std_push_front(v, ai12345[i]);
	}
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 5);

	memset(aiPopped, 0, sizeof(aiPopped));
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	szNum = 0;
	for (i = 0; i < 1000000; i++)
	{
		szNum += std_push_back(v, (int) i);
	}
	TEST_SIZE(v, 1000000);
	TEST_SAME(v, szNum, 1000000);

	for (i = 0; i < 1000000; i++)
	{
		if (std_pop_back(v, aiPopped, 1U ) != 1U)
		{
			printf("Failed to pop element #%d from array (line = %d)" CRLF, (int) i, __LINE__);
			return false;
		}
		if (aiPopped[0] != (int)(1000000U - 1U - i))
		{
			printf("Element #%d in vector failed to match (= %d) (line = %d)" CRLF, (int) i, aiPopped[0], __LINE__);
			return false;
		}
	}
	TEST_SIZE(v, 0);

	std_push_back(v, 1, 2);
	TEST_SIZE(v, 2);
	std_append_reversed(v, 5, 4, 3);
	TEST_SIZE(v, 5);
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 4, 5);
	TEST_SIZE(v, 2);
	std_prepend(v, 1, 2, 3);
	TEST_SIZE(v, 5);
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_destruct(v);

	std_construct(v);
	TEST_SIZE(v, 0);
	std_push_back(v, 100, 100, 1, 2, 3, 4, 5, 100, 100);
	i = 0;
	for (std_for_range(v, it, 2, 6), i++)
		aiPopped[i] = std_iterator_at(it)[0];
	TEST_SAME(v, i, 5);
	TEST_ARRAY(aiPopped, ai12345);
	std_pop_back(v, NULL, 100);
	std_destruct(v);

	std_construct(v);
	TEST_SIZE(v, 0);
	std_push_back(v, 1, 2, 3, 4, 5);
	std_destruct(v);

	return true;
}

static bool list_test(void)
{
	std_list(int) v;
	int aiPopped[10];
	size_t szNum;
	size_t i;

	TEST_CONTAINER_NAME(v, "list");

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
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	// Push 1/2/3/4/5 onto the front of the container, and make sure size == 5
	std_push_front(v, 1, 2, 3, 4, 5);
	TEST_SIZE(v, 5);

	// Pop 5/4/3/2/1 from the front of the container
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	szNum = 0;
	for (i = 0; i < 5; i++)
	{
		szNum += std_push_back(v, ai12345[i]);
	}
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 5);

	memset(aiPopped, 0, sizeof(aiPopped));
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	szNum = 0;
	for (i = 0; i < 5; i++)
	{
		szNum += std_push_front(v, ai12345[i]);
	}
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 5);

	memset(aiPopped, 0, sizeof(aiPopped));
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	szNum = 0;
	for (i = 0; i < 1000000; i++)
	{
		szNum += std_push_back(v, (int)i);
	}
	TEST_SIZE(v, 1000000);
	TEST_SAME(v, szNum, 1000000);

	for (i = 0; i < 1000000; i++)
	{
		if (std_pop_back(v, aiPopped, 1U) != 1U)
		{
			printf("Failed to pop element #%d from array (line = %d)" CRLF, (int)i, __LINE__);
			return false;
		}
		if (aiPopped[0] != (int)(1000000U - 1U - i))
		{
			printf("Element #%d in vector failed to match (= %d) (line = %d)" CRLF, (int)i, aiPopped[0], __LINE__);
			return false;
		}
	}
	TEST_SIZE(v, 0);

	std_push_back(v, 1, 2);
	TEST_SIZE(v, 2);
	std_append_reversed(v, 5, 4, 3);
	TEST_SIZE(v, 5);
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 4, 5);
	TEST_SIZE(v, 2);
	std_prepend(v, 1, 2, 3);
	TEST_SIZE(v, 5);
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_destruct(v);

	std_construct(v);

	std_push_back(v, 1, 5);
	TEST_SIZE(v, 2);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 0)
		{
			szNum = std_push_after(it, 4, 3, 2);
			TEST_SIZE(v, 5);
			TEST_SAME(v, szNum, 3);
			break;
		}
	}
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	std_push_back(v, 1, 5);
	TEST_SIZE(v, 2);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 1)
		{
			szNum = std_push_before(it, 2, 3, 4);
			TEST_SIZE(v, 5);
			TEST_SAME(v, szNum, 3);
			break;
		}
	}
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	std_push_back(v, 1, 5);
	TEST_SIZE(v, 2);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 0)
		{
			szNum = std_push_after_reversed(it, 2, 3, 4);
			TEST_SIZE(v, 5);
			TEST_SAME(v, szNum, 3);
			break;
		}
	}
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	std_push_back(v, 1, 5);
	TEST_SIZE(v, 2);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 1)
		{
			szNum = std_prepend_before(it, 4, 3, 2);
			TEST_SIZE(v, 5);
			TEST_SAME(v, szNum, 3);
			break;
		}
	}
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	std_push_back(v, 1, 100, 2, 3, 4, 5);
	TEST_SIZE(v, 6);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 1)
		{
			std_erase(it);
			TEST_SIZE(v, 5);
			break;
		}
	}
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 100, 1, 2, 3, 4, 5);
	TEST_SIZE(v, 6);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 0)
		{
			std_erase(it);
			TEST_SIZE(v, 5);
			break;
		}
	}
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 1, 2, 3, 4, 5, 100);
	TEST_SIZE(v, 6);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 5)
		{
			std_erase(it);
			TEST_SIZE(v, 5);
			break;
		}
	}
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 100, 1, 100, 2, 100, 3, 100, 4, 100, 5, 100);
	TEST_SIZE(v, 11);
	for (std_each_forward(v, it))
	{
		if (std_iterator_at(it)[0] == 100)
		{
			std_erase(it);
		}
	}
	TEST_SIZE(v, 5);
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_destruct(v);

	std_construct(v);
	TEST_SIZE(v, 0);
	std_push_back(v, 1, 2, 3, 4, 5);
	std_destruct(v);


	return true;
}

static bool forward_list_test(void)
{
	std_forward_list(int) v;
	int aiPopped[10];
	size_t szNum;
	size_t i;

	TEST_CONTAINER_NAME(v, "forward list");

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

	// Pop 5/4/3/2/1 from the back of the container
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	// Push 1/2/3/4/5 onto the front of the container, and make sure size == 5
	std_push_front(v, 1, 2, 3, 4, 5);
	TEST_SIZE(v, 5);

	// Pop 5/4/3/2/1 from the back of the container
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	std_destruct(v);

	std_construct(v);

	std_push_back(v, 1, 5);
	TEST_SIZE(v, 2);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 0)
		{
			szNum = std_push_after(it, 4, 3, 2);
			TEST_SIZE(v, 5);
			TEST_SAME(v, szNum, 3);
			break;
		}
	}
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 1, 5);
	TEST_SIZE(v, 2);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 1)
		{
			szNum = std_push_before(it, 2, 3, 4);
			TEST_SIZE(v, 5);
			TEST_SAME(v, szNum, 3);
			break;
		}
	}
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 1, 5);
	TEST_SIZE(v, 2);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 0)
		{
			szNum = std_push_after_reversed(it, 2, 3, 4);
			TEST_SIZE(v, 5);
			TEST_SAME(v, szNum, 3);
			break;
		}
	}
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 1, 5);
	TEST_SIZE(v, 2);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 1)
		{
			szNum = std_prepend_before(it, 4, 3, 2);
			TEST_SIZE(v, 5);
			TEST_SAME(v, szNum, 3);
			break;
		}
	}
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 1, 100, 2, 3, 4, 5);
	TEST_SIZE(v, 6);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 1)
		{
			std_erase(it);
			TEST_SIZE(v, 5);
			break;
		}
	}
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 100, 1, 2, 3, 4, 5);
	TEST_SIZE(v, 6);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 0)
		{
			std_erase(it);
			TEST_SIZE(v, 5);
			break;
		}
	}
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 1, 2, 3, 4, 5, 100);
	TEST_SIZE(v, 6);
	i = 0;
	for (std_each_forward(v, it), i++)
	{
		if (i == 5)
		{
			std_erase(it);
			TEST_SIZE(v, 5);
			break;
		}
	}
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);


	std_destruct(v);

	return true;
}

static bool deque_test(void)
{
	std_deque(int) v;
	int aiPopped[10];
	size_t szNum;
	size_t i;

	TEST_CONTAINER_NAME(v, "deque");

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
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	// Push 1/2/3/4/5 onto the front of the container, and make sure size == 5
	std_push_front(v, 1, 2, 3, 4, 5);
	TEST_SIZE(v, 5);

	// Pop 5/4/3/2/1 from the front of the container
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	szNum = 0;
	for (i = 0; i < 5; i++)
	{
		szNum += std_push_back(v, ai12345[i]);
	}
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 5);

	memset(aiPopped, 0, sizeof(aiPopped));
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	szNum = 0;
	for (i = 0; i < 5; i++)
	{
		szNum += std_push_front(v, ai12345[i]);
	}
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 5);

	memset(aiPopped, 0, sizeof(aiPopped));
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	szNum = 0;
	for (i = 0; i < 1000000; i++)
	{
		szNum += std_push_back(v, (int)i);
	}
	TEST_SIZE(v, 1000000);
	TEST_SAME(v, szNum, 1000000);

	for (i = 0; i < 1000000; i++)
	{
		if (std_pop_back(v, aiPopped, 1U) != 1U)
		{
			printf("Failed to pop element #%d from array (line = %d)" CRLF, (int)i, __LINE__);
			return false;
		}
		if (aiPopped[0] != (int)(1000000U - 1U - i))
		{
			printf("Element #%d in vector failed to match (= %d) (line = %d)" CRLF, (int)i, aiPopped[0], __LINE__);
			return false;
		}
	}
	TEST_SIZE(v, 0);

	std_push_back(v, 1, 2);
	TEST_SIZE(v, 2);
	std_append_reversed(v, 5, 4, 3);
	TEST_SIZE(v, 5);
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 4, 5);
	TEST_SIZE(v, 2);
	std_prepend(v, 1, 2, 3);
	TEST_SIZE(v, 5);
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 100, 100, 1, 2, 3, 4, 5, 100, 100);
	i = 0;
	for (std_for_range(v, it, 2, 6), i++)
		aiPopped[i] = std_iterator_at(it)[0];
	TEST_SAME(v, i, 5);
	TEST_ARRAY(aiPopped, ai12345);
	std_pop_back(v, NULL, 100);

	std_destruct(v);

	std_construct(v);
	TEST_SIZE(v, 0);
	std_push_back(v, 1, 2, 3, 4, 5);
	std_destruct(v);

	return true;
}

static bool ring_test(void)
{
	std_ring(int) v;
	int aiPopped[10];
	size_t szNum;
	size_t i;

	TEST_CONTAINER_NAME(v, "ring");

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
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	// Push 1/2/3/4/5 onto the front of the container, and make sure size == 5
	std_push_front(v, 1, 2, 3, 4, 5);
	TEST_SIZE(v, 5);

	// Pop 5/4/3/2/1 from the front of the container
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	szNum = 0;
	for (i = 0; i < 5; i++)
	{
		szNum += std_push_back(v, ai12345[i]);
	}
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 5);

	memset(aiPopped, 0, sizeof(aiPopped));
	szNum = std_pop_back(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	szNum = 0;
	for (i = 0; i < 5; i++)
	{
		szNum += std_push_front(v, ai12345[i]);
	}
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 5);

	memset(aiPopped, 0, sizeof(aiPopped));
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	std_destruct(v);

	std_construct(v);

	szNum = std_push_back(v, 6, 7, 8, 9, 1);
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 5);

	szNum = std_pop_front(v, (int *)NULL, 4);
	TEST_SIZE(v, 1);
	TEST_SAME(v, szNum, 4);

	szNum = std_push_back(v, 2, 3, 4, 5);
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 4);

	memset(aiPopped, 0, sizeof(aiPopped));
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	szNum = 0;
	for (i = 0; i < 1000000; i++)
	{
		szNum += std_push_back(v, (int)i);
	}
	TEST_SIZE(v, 1000000);
	TEST_SAME(v, szNum, 1000000);

	for (i = 0; i < 1000000; i++)
	{
		if (std_pop_back(v, aiPopped, 1U) != 1U)
		{
			printf("Failed to pop element #%d from ring (line = %d)" CRLF, (int)i, __LINE__);
			return false;
		}
		if (aiPopped[0] != (int)(1000000U - 1U - i))
		{
			printf("Element #%d in ring failed to match (= %d) (line = %d)" CRLF, (int)i, aiPopped[0], __LINE__);
			return false;
		}
	}
	TEST_SIZE(v, 0);

	std_push_back(v, 1, 2);
	TEST_SIZE(v, 2);
	std_append_reversed(v, 5, 4, 3);
	TEST_SIZE(v, 5);
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 4, 5);
	TEST_SIZE(v, 2);
	std_prepend(v, 1, 2, 3);
	TEST_SIZE(v, 5);
	szNum = std_pop_front(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_push_back(v, 100, 100, 1, 2, 3, 4, 5, 100, 100);
	i = 0;
	for (std_for_range(v, it, 2, 6), i++)
		aiPopped[i] = std_iterator_at(it)[0];
	TEST_SAME(v, i, 5);
	TEST_ARRAY(aiPopped, ai12345);
	std_pop_back(v, NULL, 100);

	std_destruct(v);

	std_construct(v);
	TEST_SIZE(v, 0);
	std_push_back(v, 1, 2, 3, 4, 5);
	std_destruct(v);

	return true;
}

static bool queue_test(void)
{
	std_queue(std_vector,int) v;
	int aiPopped[10];
	size_t szNum;
	size_t i;

	TEST_CONTAINER_NAME(v, "vector");
	std_construct(v);
	TEST_SIZE(v, 0);

	TEST_SIZE(v, 0);

	// Push 1/2/3/4/5 onto the container, and make sure size == 5
	std_enqueue(v, 1, 2, 3, 4, 5);
	TEST_SIZE(v, 5);

	// Pop 5/4/3/2/1 from the container
	szNum = std_dequeue(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	szNum = 0;
	for (i = 0; i < 5; i++)
	{
		szNum += std_enqueue(v, ai12345[i]);
	}
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 5);

	memset(aiPopped, 0, sizeof(aiPopped));
	szNum = std_dequeue(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai12345);

	std_destruct(v);

	return true;
}

static bool stack_test(void)
{
	std_stack(std_vector,int) v;
	int aiPopped[10];
	size_t szNum;
	size_t i;

	TEST_CONTAINER_NAME(v, "vector");
	std_construct(v);
	TEST_SIZE(v, 0);

	// Push 1/2/3/4/5 onto the container, and make sure size == 5
	std_push(v, 1, 2, 3, 4, 5);
	TEST_SIZE(v, 5);

	// Pop 5/4/3/2/1 from the container
	szNum = std_pop(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	szNum = 0;
	for (i = 0; i < 5; i++)
	{
		szNum += std_push(v, ai12345[i]);
	}
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 5);

	memset(aiPopped, 0, sizeof(aiPopped));
	szNum = std_pop(v, aiPopped, STD_NUM_ELEMENTS(aiPopped));
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	std_destruct(v);

	return true;
}

static bool priorityqueue_test(void)
{
#if 0
	std_priorityqueue(int) v;
	int aiPopped[10];
	size_t szNum;

	TEST_CONTAINER_NAME(v, "priority queue");
	std_construct(v);
	std_priorityqueue_compare_set(v, &int_compare);
	TEST_SIZE(v, 0);

	szNum = std_push(v, 3, 1, 5, 2, 4);
	TEST_SIZE(v, 5);
	TEST_SAME(v, szNum, 5);

	szNum = std_pop(v, aiPopped, 10);
	TEST_SIZE(v, 0);
	TEST_SAME(v, szNum, 5);
	TEST_ARRAY(aiPopped, ai54321);

	std_destruct(v);
#endif
	return true;
}

static bool prioritydeque_test(void)
{
#if 0
	std_prioritydeque(int) v;

	TEST_CONTAINER_NAME(v, "priority deque");
	std_construct(v);
	std_destruct(v);
#endif
	return true;
}

static bool vector_of_lists_test(void)
{
	std_vector_itemhandler(std_list(int)) v;
	int i;

	std_construct_itemhandler(v, std_container_default_itemhandler(STD_ITEM(v)));

	STD_ITEM_TYPEOF(v) list1;
	std_construct(list1);
	std_push_back(list1, 1, 2, 3);
	std_push_back(v, list1);

	STD_ITEM_TYPEOF(v) list2;
	std_construct(list2);
	std_push_back(list2, 4, 3, 2, 1);
	std_push_back(v, list2);

	i = 0;
	for (std_each_const(v, it),i++)
	{
		const STD_ITEM_TYPEOF(v) * list3 = std_iterator_at(it);
		size_t szListSize = std_size(list3[0]);
		if ((i == 0) && (szListSize != 3))
		{
			printf("Vectored list has wrong size (%d found, %d expected)" CRLF,
				(int) szListSize, 3);
		}
		if ((i == 1) && (szListSize != 4))
		{
			printf("Vectored list has wrong size (%d found, %d expected)" CRLF,
				(int) szListSize, 4);
		}
	}

	std_destruct(v);

	return true;
}

// -----------------------------------------------------------------

int main(int argc, char * argv[])
{
	const char * pachArg;
	bool bRunAll;
	bool bStatus;

	if (argc < 2)
	{
		bRunAll = true;
		pachArg = "";
	}
	else
	{
		bRunAll = false;
		pachArg = argv[1];
	}

	bStatus = true;
	if (bRunAll || strcmp(pachArg, "vector") == 0)			{	bStatus &= vector_test();			}
	if (bRunAll || strcmp(pachArg, "forwardlist") == 0)		{	bStatus &= forward_list_test();		}
	if (bRunAll || strcmp(pachArg, "list") == 0)			{	bStatus &= list_test();				}
	if (bRunAll || strcmp(pachArg, "deque") == 0)			{	bStatus &= deque_test();			}
	if (bRunAll || strcmp(pachArg, "ring") == 0)			{	bStatus &= ring_test();				}
	if (bRunAll || strcmp(pachArg, "queue") == 0)			{	bStatus &= queue_test();			}
	if (bRunAll || strcmp(pachArg, "stack") == 0)			{	bStatus &= stack_test();			}
	if (bRunAll || strcmp(pachArg, "priorityqueue") == 0)	{	bStatus &= priorityqueue_test();	}
	if (bRunAll || strcmp(pachArg, "prioritydeque") == 0)	{	bStatus &= prioritydeque_test();	}
	if (bRunAll || strcmp(pachArg, "nested") == 0)			{	bStatus &= vector_of_lists_test();	}

	return bStatus ? EXIT_SUCCESS : EXIT_FAILURE;
}
