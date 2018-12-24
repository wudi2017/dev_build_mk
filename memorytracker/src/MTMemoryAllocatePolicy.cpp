#include "MTMemoryAllocatePolicy.h"
#include "MTSTLAllocator.h"
#include "MTMemoryPrivate.h"
#include "MTProtectMemoryAllocator.h"
#include "MTRecorder.h"
#include "MTMonitor.h"

MTRecorder s_MTRecorder;
static MTMonitor s_MTMonitor;

void* MTMemoryAllocatePolicy::allocateBytes(size_t sz, MemoryCategory category, const char* pFileName, const int& lineNum)
{
    s_MTMonitor.start();

	void* ptr = NULL;

    if (40 == sz)
    {
        ptr = protect_tail_new(sz);
        if (NULL == ptr)
        {
            ptr = MEM_malloc_private(sz);
        }
    }
    else
    {
        ptr = MEM_malloc_private(sz);
    }

    s_MTRecorder.recordAlloc(ptr, category, sz, pFileName, lineNum);

    return ptr;
}
void MTMemoryAllocatePolicy::deallocateBytes(void* ptr)
{
    if (!ptr) {
        return;
    }
    size_t sz = 0;
    s_MTRecorder.recordDealloc(ptr, sz);

    if (40 == sz)
    {
        if (!protect_tail_try_delete(ptr))
        {
            MEM_free_private(ptr);
        }
    }
    else
    {
        MEM_free_private(ptr);
    }
}