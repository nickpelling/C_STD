/*
 * std/lock.h
 *
 *  Created on: 09 Mar 2024
 *      Author: Nick Pelling
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

inline bool std_lock_construct(const std_lock_handler_t* pstLockHandler, std_lock_handle_t* ppstvLock)
{
	if (pstLockHandler)
	{
		return (*pstLockHandler->pfnLock_Construct)(ppstvLock);
	}
	*ppstvLock = NULL;
	return false;
}

inline bool std_lock_destruct(const std_lock_handler_t* pstLockHandler, std_lock_handle_t* ppstvLock)
{
	if (pstLockHandler)
	{
		return (*pstLockHandler->pfnLock_Destruct)(ppstvLock);
	}
	*ppstvLock = NULL;
	return false;
}

inline bool std_lock_update(const std_lock_handler_t* pstLockHandler, std_lock_handle_t pstvLock, std_lock_state_t eNewState, std_lock_state_t* pstOldState, uint32_t u32Timeout_msec)
{
	if (pstLockHandler)
	{
		std_lock_state_t eState = (*pstLockHandler->pfnLock_Update)(pstvLock, eNewState, u32Timeout_msec);	// FIXME
		*pstOldState = eState;
		return true;
	}
	return false;
}

inline bool std_lock_for_reading(const std_lock_handler_t* pstLockHandler, std_lock_handle_t pstvLock, std_lock_state_t * pstLockState, uint32_t u32Timeout_msec)
{
	return std_lock_update(pstLockHandler, pstvLock, e_std_lock_ReadLocked, pstLockState, u32Timeout_msec);
}

inline bool std_lock_for_writing(const std_lock_handler_t* pstLockHandler, std_lock_handle_t pstvLock, std_lock_state_t* pstLockState, uint32_t u32Timeout_msec)
{
	return std_lock_update(pstLockHandler, pstvLock, e_std_lock_WriteLocked, pstLockState, u32Timeout_msec);
}

#endif /* _STD_LOCK_H */