/*
 * std/config.h

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

#ifndef STD_CONFIG_H_
#define STD_CONFIG_H_

#ifdef _MSC_VER
// Microsoft Visual Studio lacks some built-in functions, so emulate them
#include "std/support/ctz_clz.h"
#endif

#ifndef STD_INLINE
#define STD_INLINE  static inline
#endif

#ifndef STD_STATIC
#define STD_STATIC  static
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

// MSVC complains about static asserts that use an anonymous struct
// (whereas gcc doesn't mind). So, we use this macro solely for MSVC
// to give static asserts a unique name
#ifndef STD_USE_ASSERTNAME
#ifdef _MSC_VER
#define STD_USE_ASSERTNAME  STD_FAKEVAR()
#else
#define STD_USE_ASSERTNAME
#endif
#endif

// Command-based static assert - can be used inside or outside functions
// Note: we don't use _Static_assert() for metaprogramming because of compiler
// difficulties to do with using static asserts within expressions.
#define STD_STATIC_ASSERT(COND,MSG)	(void) sizeof(struct STD_USE_ASSERTNAME { int MSG : ((COND) ? 1 : -1); })

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

#define STD_ARG1(A,...)	        A
#define STD_ARG2(A,B,...)	    B
#define STD_ARG3(A,B,C,...)	    C
#define STD_ARG4(A,B,C,D,...)	D
#define STD_ARG5(A,B,C,D,E,...)	E

#define STD_JOIN(A,B)		    A B

#define STD_TRIGGER_COMMA(...)	,
#define STD_DEFAULT_PARAMETER2(DEFAULT,OPTIONAL)	\
	STD_JOIN(STD_ARG5,(STD_TRIGGER_COMMA OPTIONAL (~) STD_TRIGGER_COMMA OPTIONAL, DEFAULT, OPTIONAL, DEFAULT, OPTIONAL))
#define STD_DEFAULT_PARAMETER(DEFAULT,...)	STD_DEFAULT_PARAMETER2(DEFAULT,STD_ARG1(__VA_ARGS__,))

#endif /* STD_CONFIG_H_ */
