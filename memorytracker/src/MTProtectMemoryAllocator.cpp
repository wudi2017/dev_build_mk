#include "MTProtectMemoryAllocator.h"
#include "MTSTLAllocator.h"
#include "MTThreadLock.h"
#include "MTLog.h"
#include "MTMemoryPrivate.h"

bool s_bEnableMemoryTracker = false;

static bool s_bRandFlag = false;
struct MMAPBLOCKITEM
{
    MMAPBLOCKITEM()
    {
        baseAddr = NULL;
        size = 0;
        rwsize = 0;
    }
    void* baseAddr;
    int size;
    int rwsize;
};


template<typename T>
struct PrivateList
{
    typedef std::list<T, STLAllocator<T> > type;
};

template <typename K, typename V, typename P = std::less<K> >
struct PrivateMap
{
    typedef std::map<K, V, P, STLAllocator<std::pair<const K, V> > >    type;
};

static MTThreadLock s_mmapTableLock;
//static std::map<void*, MMAPITEM> s_mmapTable;
static bool s_bInitFlag = false;
static PrivateList<MMAPBLOCKITEM>::type* sp_mmapFreeList = NULL;
static PrivateMap<void*,MMAPBLOCKITEM>::type* sp_mmapUsedList = NULL;
static const int s_pageSize = 4*1024;
static const int s_maxUserSize = 1*s_pageSize;
static const int s_maxProtectBlockCount = 10000;
static const char* s_mmap_fileName_prefix = "/tmp/protectmem_";

char* s_mmap_baseAddr = NULL;

std::string dump_status_protectmem()
{
    MTThreadAutoLock autoLock(s_mmapTableLock);

    std::string dumpStatus;

    dumpStatus.assign("protectmem blockCount used[0] free[0]\n");

    if (NULL != sp_mmapFreeList && NULL != sp_mmapUsedList)
    {
        // monitor
        char monitorWriteBuf[512];
        snprintf(monitorWriteBuf,512,"protectmem blockCount used[%d] free[%d]\n", sp_mmapUsedList->size(), sp_mmapFreeList->size());

        dumpStatus.assign(monitorWriteBuf);
    }

    MEM_Log_private(false, "%s", dumpStatus.c_str());

    return dumpStatus;
}

void protect_tail_mem_init()
{
    static PrivateList<MMAPBLOCKITEM>::type s_mmapFreeList;
    static PrivateMap<void*,MMAPBLOCKITEM>::type s_mmapUsedList;
    sp_mmapFreeList = &s_mmapFreeList;
    sp_mmapUsedList = &s_mmapUsedList;

    MTLOG_I("protect_tail_mem_init sp_mmapFreeList=%p sp_mmapUsedList=%p", sp_mmapFreeList, sp_mmapUsedList);

    char mapFileName[256];
    memset(mapFileName, 0, 256);
    snprintf(mapFileName, 256, "%s%d",s_mmap_fileName_prefix, getpid());

    int mmapMemSize = s_maxProtectBlockCount * 2 * s_pageSize;
    
    int fd = open(mapFileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    lseek(fd, mmapMemSize, SEEK_SET);
    write(fd, "\0", 1);
    //char * mem = (char*)mmap(NULL, mmapMemSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    char * mem = (char*)mmap(NULL, mmapMemSize, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0);
    close(fd);

    if (mem)
    {
        MTLOG_I("mmap file:%s", mapFileName);
        s_mmap_baseAddr = mem;

        for (int i = 0; i < s_maxProtectBlockCount; ++i)
        {
            char* protectBlockBaseAddr = mem+i*2*s_pageSize;
            char* protectBlockReadOnlyAddr = protectBlockBaseAddr + s_pageSize;

            int magicCode = EXTRA_MAGIC_CODE;
            memcpy(protectBlockReadOnlyAddr, (void*)&magicCode, sizeof(int));

            mprotect(protectBlockBaseAddr, s_pageSize, PROT_READ|PROT_WRITE);
            mprotect(protectBlockReadOnlyAddr, s_pageSize, PROT_READ);

            MTLOG_I("protectBlock protectBlockBaseAddr:%p protectBlockReadOnlyAddr:%p", protectBlockBaseAddr, protectBlockReadOnlyAddr);

            MMAPBLOCKITEM mmapBlockItem;
            mmapBlockItem.baseAddr = protectBlockBaseAddr;
            mmapBlockItem.size = 2*s_pageSize;
            mmapBlockItem.rwsize = s_pageSize;
            s_mmapFreeList.push_back(mmapBlockItem);
        }

        MTLOG_I("s_mmapFreeList block size=%d", s_mmapFreeList.size());

    }
}
void protect_tail_mem_deinit()
{
    char mapFileName[256];
    memset(mapFileName, 0, 256);
    snprintf(mapFileName, 256, "%s%d",s_mmap_fileName_prefix, getpid());

    if (s_mmap_baseAddr)
    {
        int mmapMemSize = s_maxProtectBlockCount * 2 * s_pageSize;
        munmap(s_mmap_baseAddr, mmapMemSize);

        remove(mapFileName);
    }
}
void* protect_tail_new(const int size)
{
    MTThreadAutoLock autoLock(s_mmapTableLock);

    if (!s_bInitFlag)
    {
        protect_tail_mem_init();
        s_bInitFlag = true;
    }

    void* userAddr = NULL;

    if (NULL != sp_mmapFreeList && NULL != sp_mmapUsedList)
    {
        if (sp_mmapFreeList->size() > 0)
        {
            MMAPBLOCKITEM mmapBlockItem = sp_mmapFreeList->front();
            if (size < mmapBlockItem.rwsize)
            {
                // rm freebock in free list
                sp_mmapFreeList->pop_front();

                memset(mmapBlockItem.baseAddr, 0, mmapBlockItem.rwsize);

                userAddr = mmapBlockItem.baseAddr + mmapBlockItem.rwsize - size;

                // add used map
                sp_mmapUsedList->insert(std::pair<void*,MMAPBLOCKITEM>(userAddr,mmapBlockItem));

                MTLOG_I("protect_tail_new userAddr=%p size=%d protectBlockBaseAddr=%p", userAddr, size, mmapBlockItem.baseAddr);
            }
        }
    }

    return userAddr;
}
bool protect_tail_try_delete(void* addr)
{
    bool bDelete = false;

    MTThreadAutoLock autoLock(s_mmapTableLock);

    if (NULL != sp_mmapFreeList && NULL != sp_mmapUsedList)
    {
        std::map<void*, MMAPBLOCKITEM>::iterator it = sp_mmapUsedList->find(addr);
        if(it != sp_mmapUsedList->end()) {
            MMAPBLOCKITEM mmitem = it->second;
            if (NULL != mmitem.baseAddr)
            {
                sp_mmapFreeList->push_back(mmitem);
                
                sp_mmapUsedList->erase(it);

                bDelete = true;
                MTLOG_I("protect_tail_delete addr=%p protectBlockBaseAddr=%p", addr, mmitem.baseAddr);
            }
        }
    }

    return bDelete;
}