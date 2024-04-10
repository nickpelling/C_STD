/*
 * std_priority_queue.c

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

#include "std/priority_queue.h"

#define CONTAINER_TO_PRIORITYQUEUE(CONTAINER)		STD_CONTAINER_OF(CONTAINER, std_priorityqueue_t, stContainer)
#define PRIORITYQUEUE_TO_CONTAINER(PRIORITYQUEUE)	&PRIORITYQUEUE->stContainer

#define ITERATOR_TO_VECTORIT(IT)					STD_CONTAINER_OF(IT, std_vector_iterator_t, stIterator)
#define VECTORIT_TO_ITERATOR(PRIORITYQUEUEIT)		&PRIORITYQUEUEIT->stIterator

void stdlib_priorityqueue_compare_set(std_priorityqueue_t* pstPriorityQueue, pfn_std_compare_t pfnCompare)
{
	pstPriorityQueue->pfnCompare = pfnCompare;
}

/**
 * Push a series of items onto a priority queue container
 *
 * @param[in]	pstContainer	Priority queue container to push the series of items onto
 * @param[in]	pstSeries		Linear series of items to push onto the container
 *
 * @return Number of items inserted into the priority queue container
 */
size_t stdlib_priorityqueue_push(std_container_t* pstContainer, std_linear_series_t* pstSeries)
{
	std_priorityqueue_t* pstPriorityQueue = CONTAINER_TO_PRIORITYQUEUE(pstContainer);
	return stdlib_vector_heap_insert(pstContainer, pstSeries, pstPriorityQueue->pfnCompare);
}
