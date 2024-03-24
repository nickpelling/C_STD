#include "std/memory.h"

// Force the C compiler to emit non-inline versions of these inline routines
void* std_memoryhandler_malloc(const std_memoryhandler_t* pstMemoryHandler, std_container_has_t eHas, size_t szSize);
void* std_memoryhandler_realloc(const std_memoryhandler_t* pstMemoryHandler, std_container_has_t eHas, void* pvData, size_t szSize);
void std_memoryhandler_free(const std_memoryhandler_t* pstMemoryHandler, std_container_has_t eHas, void* pvData);