/*
 * std/linear_series.h

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

#ifndef STD_LINEAR_SERIES_H_
#define STD_LINEAR_SERIES_H_

#include <stddef.h>			// for size_t
#include "std/config.h"		// for STD_LINEAR_ADD

typedef struct
{
	const void * pvStart;
	const void * pvEnd;
	size_t szSizeofItem;
	size_t szNumItems;
	int iStep;
} std_linear_series_t;

typedef struct
{
	const std_linear_series_t* pstSeries;
	const void* pvData;
} std_linear_series_iterator_t;

STD_INLINE void std_linear_series_construct(std_linear_series_t * pstSeries, const void* pvData, size_t szSizeofItem, size_t szNumItems, bool bReverse)
{
	pstSeries->szSizeofItem = szSizeofItem;
	pstSeries->szNumItems = szNumItems;
	if (bReverse == false)
	{
		pstSeries->pvStart = pvData;
		pstSeries->pvEnd = STD_LINEAR_ADD(pvData, szSizeofItem * szNumItems);
		pstSeries->iStep = (int)szSizeofItem;
	}
	else
	{
		pstSeries->pvStart = STD_LINEAR_ADD(pvData, (szNumItems - 1U) * szSizeofItem);
		pstSeries->pvEnd = STD_LINEAR_SUB(pvData, szSizeofItem);
		pstSeries->iStep = -(int)szSizeofItem;
	}
}

STD_INLINE void std_linear_series_iterator_construct(std_linear_series_iterator_t* pstIterator, const std_linear_series_t* pstSeries)
{
	pstIterator->pstSeries = pstSeries;
	pstIterator->pvData = pstSeries->pvStart;
}

STD_INLINE void std_linear_series_iterator_next(std_linear_series_iterator_t* pstIterator)
{
	pstIterator->pvData = STD_LINEAR_ADD(pstIterator->pvData, pstIterator->pstSeries->iStep);
}

STD_INLINE bool std_linear_series_iterator_done(std_linear_series_iterator_t* pstIterator)
{
	return (pstIterator->pvData == pstIterator->pstSeries->pvEnd);
}

#endif /* STD_LINEAR_SERIES_H_ */