
// When not building under Visual C, build separate non-inline versions of all inline functions
#ifndef _MSC_VER
#define STD_INLINE				// for inline functions
#define STD_EXTERN				// for the container jumptable
#include "std/container.h"
#endif