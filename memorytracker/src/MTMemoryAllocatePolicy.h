
#include "MTMemoryConfig.h"

class MTMemoryAllocatePolicy
{
public:
    static void* allocateBytes(size_t sz, MemoryCategory category = MEMCATEGORY_UNKNOWN, const char* pFileName = NULL, const int& lineNum = 0);

    static void deallocateBytes(void* ptr);
};