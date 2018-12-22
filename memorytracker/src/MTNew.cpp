#include <MTNew.h>
#include "MTMemoryAllocatePolicy.h"

MEMTRACKERLIB_API void* operator new(std::size_t sz) 
{
    if (0 == sz) {
        sz = 1;
    }
    void* p = MTMemoryAllocatePolicy::allocateBytes(sz);
    return p;
}

void* operator new[](std::size_t sz)
{
    if (0 == sz) {
        sz = 1;
    }

    void* p = MTMemoryAllocatePolicy::allocateBytes(sz);
    return p;
}

void operator delete(void* ptr)  throw()
{
    if (NULL == ptr) {
        return;
    }
    MTMemoryAllocatePolicy::deallocateBytes(ptr);
}

void operator delete[] (void* ptr)  throw()
{
    if (NULL == ptr) {
        return;
    }
    MTMemoryAllocatePolicy::deallocateBytes(ptr);
}