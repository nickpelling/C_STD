/*
 * std/linear_series.h
 *
 *  Created on: 06 Apr 2024
 *      Author: Nick Pelling
 */

#ifndef STD_LINEAR_SERIES_H_
#define STD_LINEAR_SERIES_H_

#include <stddef.h>			// for size_t
#include "std/config.h"		// for STD_LINEAR_ADD

typedef struct
{
	const void * pvData;
	const void * pvEnd;
	size_t szSizeofItem;
	size_t szNumItems;
	int iStep;
} std_linear_series_t;

STD_INLINE void std_linear_series_construct(std_linear_series_t * pstSeries, const void* pvData, size_t szSizeofItem, size_t szNumItems, bool bReverse)
{
	pstSeries->szSizeofItem = szSizeofItem;
	pstSeries->szNumItems = szNumItems;
	if (bReverse == false)
	{
		pstSeries->pvData = pvData;
		pstSeries->pvEnd = STD_LINEAR_ADD(pvData, szSizeofItem * szNumItems);
		pstSeries->iStep = (int)szSizeofItem;
	}
	else
	{
		pstSeries->pvData = STD_LINEAR_ADD(pvData, (szNumItems - 1U) * szSizeofItem);
		pstSeries->pvEnd = STD_LINEAR_SUB(pvData, szSizeofItem);
		pstSeries->iStep = -(int)szSizeofItem;
	}
}

STD_INLINE void std_linear_series_next(std_linear_series_t* pstSeries)
{
	pstSeries->pvData = STD_LINEAR_ADD(pstSeries->pvData, pstSeries->iStep);
}

STD_INLINE bool std_linear_series_done(std_linear_series_t* pstSeries)
{
	return (pstSeries->pvData == pstSeries->pvEnd);
}

#endif /* STD_LINEAR_SERIES_H_ */