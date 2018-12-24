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

#ifdef MEM_COMP_OPT_USE_MAGIC_CODE
    // sz + EXTRA_MAGIC_LEN overflow the size_t, should not modify it.
    if (sz < static_cast<size_t>(sz + EXTRA_MAGIC_LEN)) {
        sz += EXTRA_MAGIC_LEN;
    }
    else {
        // if (MemoryTrackerIF::GetErrorLogFunc()) {
        //     MemoryTrackerIF::GetErrorLogFunc()(LogLvl_Error, MEMTRACK_TAG, "Module:%d allocate too big size, size:%" MEM_PRIusize_t, category, sz);
        // }
        return NULL;
    }
#endif


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

#ifdef MEM_COMP_OPT_USE_MAGIC_CODE
    if (ptr) {
        if (0 != memcmp(reinterpret_cast<char*>(ptr) + sz - EXTRA_MAGIC_LEN, &EXTRA_MAGIC_CODE, EXTRA_MAGIC_LEN))
        {
            memcpy(reinterpret_cast<char*>(ptr) + sz - EXTRA_MAGIC_LEN, &EXTRA_MAGIC_CODE, EXTRA_MAGIC_LEN);
        }
    }
#endif

    return ptr;
}
void MTMemoryAllocatePolicy::deallocateBytes(void* ptr)
{
    if (!ptr) {
        return;
    }
    size_t sz = 0;
    s_MTRecorder.recordDealloc(ptr, sz);

#ifdef MEM_COMP_OPT_USE_MAGIC_CODE
    // empty instance sz will be 0
    if (sz > EXTRA_MAGIC_LEN
        && 0 != memcmp(reinterpret_cast<char*>(ptr) + sz - EXTRA_MAGIC_LEN, &EXTRA_MAGIC_CODE, EXTRA_MAGIC_LEN)) {
        // memroy corrupt, overflow
        char* p = NULL;
        *p = 'a';
    }
#endif

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