#ifdef _MSC_VER
#include <intrin.h>

static inline int __builtin_ctz(unsigned x)
{
    return (int)_tzcnt_u32(x);
}

static inline int __builtin_ctzll(unsigned long long x)
{
#ifdef _WIN64
    return (int)_tzcnt_u64(x);
#else
    return !!unsigned(x) ? __builtin_ctz((unsigned)x) : 32 + __builtin_ctz((unsigned)(x >> 32));
#endif
}

static inline int __builtin_ctzl(unsigned long x)
{
    return sizeof(x) == 8 ? __builtin_ctzll(x) : __builtin_ctz((unsigned)x);
}

static inline int __builtin_clz(unsigned x)
{
    return (int)_lzcnt_u32(x);
}

static inline int __builtin_clzll(unsigned long long x)
{
#ifdef _WIN64
    return (int)_lzcnt_u64(x);
#else
    return !!unsigned(x >> 32) ? __builtin_clz((unsigned)(x >> 32)) : 32 + __builtin_clz((unsigned)x);
#endif
}

static inline int __builtin_clzl(unsigned long x)
{
    return sizeof(x) == 8 ? __builtin_clzll(x) : __builtin_clz((unsigned)x);
}
#endif