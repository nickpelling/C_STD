/*
 * std/config.h
 *
 *  Created on: 22 Oct 2022
 *      Author: Nick Pelling
 */

#ifndef STD_CONFIG_H_
#define STD_CONFIG_H_

#ifdef _MSC_VER
// Microsoft Visual Studio lacks some built-in functions, so emulate them
#include "std/support/ctz_clz.h"
#endif

// These macros should be customised to match the compiler you're using
#ifndef STD_EVAL
#define STD_EVAL(X)			X
#endif

#define _STD_CONCAT(A,B)	A##B
#define STD_CONCAT(A,B)		_STD_CONCAT(A,B)

#ifndef STD_UNUSED
#ifdef _MSC_VER
#define STD_UNUSED			/* [[maybe_unused]] */
#else
#define STD_UNUSED			__attribute__((unused))
#endif
#endif

#ifndef STD_FAKEVAR
#define STD_FAKEVAR()		STD_CONCAT(_fakevar_, __COUNTER__)
#endif

#ifndef STD_OFFSETOF
#define STD_OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0U)->ELEMENT))
#endif

#ifndef STD_TYPEOF
#define STD_TYPEOF(OBJ)		__typeof__(OBJ)
#endif

#ifndef STD_ALIGNOF
#define STD_ALIGNOF(TYPE)	_Alignof(STD_TYPEOF(TYPE))
#endif

// Helper macros (to calculate untyped addresses)
#define STD_LINEAR_ADD(BASEADDR,OFFSET)		((void *)( ((char *)(void *)(BASEADDR)) + (OFFSET) ))
#define STD_LINEAR_SUB(BASEADDR,OFFSET)		((void *)( ((char *)(void *)(BASEADDR)) - (OFFSET) ))

#ifndef STD_CONTAINER_OF
#define STD_CONTAINER_OF(PTR, TYPE, FIELD)	((TYPE *)STD_LINEAR_SUB(PTR, (int)((char *)(void *)&((TYPE *)256U)->FIELD - (char *)256U)))
#endif

#ifndef STD_NUM_ELEMENTS
#define STD_NUM_ELEMENTS(ARRAY)	(sizeof(ARRAY)/sizeof(ARRAY[0]))
#endif

// Command-based static assert - can be used inside or outside functions
#if (__STDC_VERSION__ >= 201112L) && 0 /* test for C11 */
#include <assert.h>
#define STD_STATIC_ASSERT(COND,MSG) _Static_assert(COND,#MSG)
#else
#define STD_STATIC_ASSERT(COND,MSG)	typedef char static_assertion_##MSG[(COND)?1:-1]
#endif

// Expression-based static assert - either halts compilation or returns 0
#if 1
#define STD_EXPR_ASSERT(CONDITION,MSG)	\
    (!sizeof(struct MSG { int MSG : ((CONDITION) ? 1 : -1); }))
#else
#define STD_EXPR_ASSERT(CONDITION,MSG)	\
	(!sizeof(struct MSG { char MSG[1 - 2*!(CONDITION)]; }))
#endif

/* Create a (fake) instance of a pointer to a given type */
#define STD_TYPE_TO_FAKE_INSTANCE(T)    ((STD_TYPEOF(T) *)1024U)

/* Convert a type (or typed variable) to a pointer to that type */
#define STD_VAR_TO_PTR_TO_TYPE(T)       STD_TYPEOF(T) *

#if __STDC_VERSION__ >= 201112L /* test for C11 */
#define STD_TYPES_ARE_SAME(T1,T2)  \
    _Generic(STD_TYPE_TO_FAKE_INSTANCE(T1), \
        STD_VAR_TO_PTR_TO_TYPE(T2): true, default: false)
#else
#define STD_TYPES_ARE_SAME(T1, T2)  1
#endif

#endif /* STD_CONFIG_H_ */
