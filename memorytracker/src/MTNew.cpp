#include <MTNew.h>
#include "MTMemoryAllocatePolicy.h"
#include "MTRecorder.h"

extern MTRecorder s_MTRecorder;

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

#ifdef HAVE_WRAP_LINK_OPTION

void* __wrap_malloc (std::size_t sz)
{
    void* ptr = __real_malloc(sz);
    //s_MTRecorder.recordAlloc(ptr, MEMCATEGORY_UNKNOWN, sz, NULL, 0);
    return ptr;
}

// void* __wrap_calloc(std::size_t n, std::size_t sz)
// {

//     void *ptr = __real_calloc(n, sz);
//     s_MTRecorder.recordAlloc(ptr, MEMCATEGORY_UNKNOWN, sz*n, NULL, 0);
//     return ptr;
// }

// void* __wrap_realloc(void* ptr, std::size_t sz)
// {
//     size_t s = 0;
//     s_MTRecorder.recordDealloc(ptr, s);
//     ptr = __real_realloc(ptr, sz);
//     s_MTRecorder.recordAlloc(ptr, MEMCATEGORY_UNKNOWN, sz, NULL, 0);
//     return ptr;
// }

// void __wrap_free(void *ptr)
// {
//     size_t sz = 0;
//     s_MTRecorder.recordDealloc(ptr, sz);
//     __real_free(ptr);
// }

// void* __wrap_memcpy(void* dst, void* src, std::size_t sz)
// {
//     return __real_memcpy(dst, src, sz);
// }

// void* __wrap_memmove(void* dst, void* src, std::size_t sz)
// {
//     return __real_memmove(dst, src, sz);
// }

// void* __wrap_memset(void* ptr, int value, std::size_t sz)
// {
//     return __real_memset(ptr, value, sz);
// }

#endif