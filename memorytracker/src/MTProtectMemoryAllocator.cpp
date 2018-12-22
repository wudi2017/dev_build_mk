#include "MTProtectMemoryAllocator.h"
#include "MTSTLAllocator.h"
#include "MTThreadLock.h"

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

static ThreadLock s_mmapTableLock;
//static std::map<void*, MMAPITEM> s_mmapTable;
static bool s_bInitFlag = false;
static PrivateList<MMAPBLOCKITEM>::type* sp_mmapFreeList = NULL;
static PrivateMap<void*,MMAPBLOCKITEM>::type* sp_mmapUsedList = NULL;
static const int s_pageSize = 4*1024;
static const int s_maxUserSize = 1*s_pageSize;
static const int s_maxProtectBlockCount = 10000;
static const char* s_mmap_fileName_prefix = "/tmp/protectmem_";

char* s_mmap_baseAddr = NULL;
void * monitor_func(void * pv_param)
{
    printf("monitor_func start\n");
    while(true)
    {
        usleep(1000*1000*3);

        s_mmapTableLock.lock();

        if (NULL != sp_mmapFreeList && NULL != sp_mmapUsedList)
        {
            // monitor
            char monitorWriteBuf[512];
            snprintf(monitorWriteBuf,512,"monitor protectmem blockCount used[%d] free[%d]\n", sp_mmapUsedList->size(), sp_mmapFreeList->size());

            printf("monitor_func %s\n",monitorWriteBuf);

            // write to file 
            char monitorFileName[256];
            memset(monitorFileName, 0, 256);
            snprintf(monitorFileName, 256, "%s%d_monitor",s_mmap_fileName_prefix, getpid());
            FILE *  fp;
            fp=fopen(monitorFileName,"w");
            if (NULL != fp)
            {
                fwrite(monitorWriteBuf,strlen(monitorWriteBuf),1,fp);
                fclose(fp);
            }
        }

        s_mmapTableLock.unlock();
    }
}
void protect_tail_mem_init()
{
    static PrivateList<MMAPBLOCKITEM>::type s_mmapFreeList;
    static PrivateMap<void*,MMAPBLOCKITEM>::type s_mmapUsedList;
    sp_mmapFreeList = &s_mmapFreeList;
    sp_mmapUsedList = &s_mmapUsedList;

    printf("protect_tail_mem_init sp_mmapFreeList=%p sp_mmapUsedList=%p\n", sp_mmapFreeList, sp_mmapUsedList);

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
        printf("mmap file:%s\n", mapFileName);
        s_mmap_baseAddr = mem;

        for (int i = 0; i < s_maxProtectBlockCount; ++i)
        {
            char* protectBlockBaseAddr = mem+i*2*s_pageSize;
            char* protectBlockReadOnlyAddr = protectBlockBaseAddr + s_pageSize;

            int magicCode = 0x12457836;
            memcpy(protectBlockReadOnlyAddr, (void*)&magicCode, sizeof(int));

            mprotect(protectBlockBaseAddr, s_pageSize, PROT_READ|PROT_WRITE);
            mprotect(protectBlockReadOnlyAddr, s_pageSize, PROT_READ);

            printf("protectBlock protectBlockBaseAddr:%p protectBlockReadOnlyAddr:%p\n", protectBlockBaseAddr, protectBlockReadOnlyAddr);

            MMAPBLOCKITEM mmapBlockItem;
            mmapBlockItem.baseAddr = protectBlockBaseAddr;
            mmapBlockItem.size = 2*s_pageSize;
            mmapBlockItem.rwsize = s_pageSize;
            s_mmapFreeList.push_back(mmapBlockItem);
        }

        printf("s_mmapFreeList block size=%d\n", s_mmapFreeList.size());

        // create monitor thread
        pthread_t tid;
        int ret = pthread_create(&tid,NULL,&monitor_func,NULL);
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
    s_mmapTableLock.lock();

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

                printf("protect_tail_new userAddr=%p size=%d protectBlockBaseAddr=%p \n", userAddr, size, mmapBlockItem.baseAddr);
            }
        }
    }

    s_mmapTableLock.unlock();

    return userAddr;
}
bool protect_tail_try_delete(void* addr)
{
    bool bDelete = false;

    s_mmapTableLock.lock();

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
                printf("protect_tail_delete addr=%p protectBlockBaseAddr=%p \n", addr, mmitem.baseAddr);
            }
        }
    }

    s_mmapTableLock.unlock();

    return bDelete;
}