#include "MTMemoryAllocatePolicy.h"
#include "MTSTLAllocator.h"
#include "MTMemoryPrivate.h"
#include "MTProtectMemoryAllocator.h"

void* MTMemoryAllocatePolicy::allocateBytes(size_t sz, MemoryCategory category, const char* pFileName, const int& lineNum)
{
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

    return ptr;
}
void MTMemoryAllocatePolicy::deallocateBytes(void* ptr)
{
	int sz = 40;
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