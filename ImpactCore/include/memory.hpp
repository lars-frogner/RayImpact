#pragma once
#include "RegionAllocator.hpp"
#include <stdlib.h>
#include <malloc.h>

namespace Impact {

// Set default value for L1 cache line size
#ifndef IMP_L1_CACHE_LINE_SIZE
#define IMP_L1_CACHE_LINE_SIZE 64
#endif

// Inline memory function definitions

inline void* allocateAligned(size_t size)
{
    #ifdef IMP_IS_WINDOWS
    return _aligned_malloc(size, IMP_L1_CACHE_LINE_SIZE);
    #else // Linux
    void* pointer;
    if (posix_memalign(&pointer, IMP_L1_CACHE_LINE_SIZE, size) != 0)
        pointer = nullptr;
    return pointer;
    #endif
}

inline void freeAligned(void* pointer)
{
    #ifdef IMP_IS_WINDOWS
    _aligned_free(pointer);
    #else // Linux
    free(pointer);
    #endif
}

template <typename T>
inline T* allocateAligned(size_t count)
{
    return (T*)(allocateAligned(count*sizeof(T)));
}

// Memory macros

#define allocated_on_stack(type, count) (type*)alloca((count)*sizeof(type))

#define allocated_in_region(allocator, type) new (allocator.allocate(sizeof(type))) type

} // Impact
