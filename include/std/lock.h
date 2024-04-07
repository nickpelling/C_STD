/*
 * std/lock.h

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

#ifndef _STD_LOCK_H
#define _STD_LOCK_H

#include <stddef.h>		// for size_t
#include <stdbool.h>	// for bool
#include <stdint.h>		// for uint32_t
#include <string.h>		// for memcpy

#include "std/memory.h"

typedef struct
{
	std_memoryhandler_t * pstMemoryHandler;
} std_lock_platform_t;

typedef enum
{
	e_std_lock_Unlocked,
	e_std_lock_ReadLocked,
	e_std_lock_WriteLocked,

	// Enumeration value used to signal that no lock action was required, so should not be persisted!
	e_std_lock_NoRestoreNeeded,

	// Enumeration value used to signal that lock state was invalid, so should not be persisted!
	// This is used to support iterator loops
	e_std_lock_Invalid,

	e_std_lock_MAX
} std_lock_state_t;

// Handle to a lock instance of an unspecified type
typedef void * std_lock_handle_t;

typedef struct
{
	bool (*pfnLock_Construct)(std_lock_handle_t * ppstvLock);
	bool (*pfnLock_Destruct)(std_lock_handle_t * ppstvLock);

	std_lock_state_t (*pfnLock_Update)(std_lock_handle_t pstvLock, std_lock_state_t eNewState, uint32_t u32Timeout_msec);
} std_lock_handler_t;

STD_INLINE bool std_lock_construct(const std_lock_handler_t* pstLockHandler, std_lock_handle_t* ppstvLock)
{
	if (pstLockHandler)
	{
		return (*pstLockHandler->pfnLock_Construct)(ppstvLock);
	}
	*ppstvLock = NULL;
	return false;
}

STD_INLINE bool std_lock_destruct(const std_lock_handler_t* pstLockHandler, std_lock_handle_t* ppstvLock)
{
	if (pstLockHandler)
	{
		return (*pstLockHandler->pfnLock_Destruct)(ppstvLock);
	}
	*ppstvLock = NULL;
	return false;
}

STD_INLINE std_lock_state_t std_lock_update(const std_lock_handler_t* pstLockHandler, std_lock_handle_t pstvLock, std_lock_state_t eNewState, uint32_t u32Timeout_msec)
{
	if (pstLockHandler)
	{
		return (*pstLockHandler->pfnLock_Update)(pstvLock, eNewState, u32Timeout_msec);	// FIXME
	}
	return e_std_lock_NoRestoreNeeded;
}

STD_INLINE std_lock_state_t std_lock_for_reading(const std_lock_handler_t* pstLockHandler, std_lock_handle_t pstvLock, uint32_t u32Timeout_msec)
{
	return std_lock_update(pstLockHandler, pstvLock, e_std_lock_ReadLocked, u32Timeout_msec);
}

STD_INLINE std_lock_state_t std_lock_for_writing(const std_lock_handler_t* pstLockHandler, std_lock_handle_t pstvLock, uint32_t u32Timeout_msec)
{
	return std_lock_update(pstLockHandler, pstvLock, e_std_lock_WriteLocked, u32Timeout_msec);
}

#endif /* _STD_LOCK_H */