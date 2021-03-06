#include "baselib.h"
#include <iostream>
using namespace std;


//////////////////////////////////////////////////////////////////////////////////////////
// PrintMemBuf

#define TESTLOG printf

void PrintMemBuf(const void * buf, const int size)
{
    const char * cbuf = (const char*)buf;
    int eventLineOutBytes = 32;

	const int LINE_SIZE = 512;
    char sLine[LINE_SIZE];
    memset(sLine, 0, LINE_SIZE);
    char tmpbuf[16];

    int i = 0;
    for (; i < size; ++i)
    {
        unsigned char c = cbuf[i];
        sprintf(tmpbuf, "%02x ", c);
        strcat(sLine, tmpbuf);

        if ((i+1)%eventLineOutBytes == 0)
        {
            int ad_i = i/eventLineOutBytes;
            const void* addr = cbuf + ad_i*eventLineOutBytes;
            printf("[%p] %s\n", addr, sLine);
            memset(sLine, 0, LINE_SIZE);
        }
    }

    if (strlen(sLine)>0)
    {
        int ad_i = i/eventLineOutBytes;
        const void* addr = cbuf + ad_i*eventLineOutBytes;
        printf("[%p] %s\n", addr, sLine);
        memset(sLine, 0, LINE_SIZE);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// CheckSum

unsigned int CheckSum(const void * buf, const int size)
{
	const char * cbuf = (const char*)buf;
	unsigned int checkSum = 0;
	int iCntDw = size/4;
	int iCntByte = size%4;

	// printf("checkSum iCntDw %d\n", iCntDw);
	// printf("checkSum iCntByte %d\n", iCntByte);

	unsigned int * pDw = (unsigned int *)cbuf;
	for (int i = 0; i < iCntDw; ++i)
	{
		unsigned int ui = *pDw;

		//printf("checkSum pDw ui %d\n", ui);

		checkSum = checkSum + ui;

		pDw++;
	}

	const char * bufLast = cbuf + 4*iCntDw;
	for (int i = 0; i < iCntByte; ++i)
	{
		char ui = *bufLast;

		//printf("checkSum bufLast ui %d\n", ui);

		checkSum = checkSum + ui;

		bufLast++;
	}

	return checkSum;
}

//////////////////////////////////////////////////////////////////////////////////////////
// std string StringSplit
std::vector<std::string> StringSplit(std::string str, std::string pat)
{
	std::vector<std::string> bufStr;
 
	while (true)
	{
		std::size_t index = str.find(pat);
 
		std::string subStr = str.substr(0, index);
		if (!subStr.empty())
			bufStr.push_back(subStr);
 
		str.erase(0, index + pat.size());

		if (index == std::string::npos)
			break;
	}
	return bufStr;
}

//////////////////////////////////////////////////////////////////////////////////////////
// log

extern unsigned long DGetTickCountMS();
extern unsigned long DGetThreadTimeUS();
extern void testlog(const char* Fmt, ...);

#include "stdarg.h"
#include <time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>

//#define ANDROIDLOG_ENABLE
#define FILELOG_ENABLE
//#define PRINTFLOG_ENABLE

#ifdef ANDROIDLOG_ENABLE
#include <android/log.h>
#define ANDROIDLOG(...) __android_log_print(ANDROID_LOG_DEBUG, "testlog", __VA_ARGS__)
#endif

pid_t DGettid()
{
    return syscall(__NR_gettid);
}

unsigned long DGetTickCountMS()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec*1000 + time.tv_nsec/1000000;
}
unsigned long DGetThreadTimeUS()
{
    struct timespec time;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
    return time.tv_sec*1000*1000 + time.tv_nsec/1000;
}

void testlog(const char* Fmt, ...)
{
    va_list ap;
    char TempStr[512];
    va_start(ap,Fmt);
    vsprintf(TempStr,Fmt,ap);
    va_end(ap);
    char TempStrOut[512];
    unsigned long TT = DGetThreadTimeUS();
    snprintf(TempStrOut, 512, "[%d %d][%ld %lu.%03lums] %s", ::getpid(), ::DGettid(), 
        DGetTickCountMS(), TT/1000, TT%1000, TempStr);
    /************************************************************************/

#ifdef FILELOG_ENABLE
    // write to file 
    FILE *  fp;
    fp=fopen("/tmp/testlog.txt","a+");
    if (NULL != fp)
    {
    	char TempStrOutLn[512];
    	strcpy(TempStrOutLn, TempStrOut);
    	strcat(TempStrOutLn, "\n");
    	fwrite(TempStrOutLn,strlen(TempStrOutLn),1,fp);
    	fclose(fp);
    }
#endif
    
#ifdef PRINTFLOG_ENABLE
    // print to consoler
    printf("%s\n", TempStrOut);
#endif

#ifdef ANDROIDLOG_ENABLE
    // android log
    ANDROIDLOG("%s", TempStrOut);
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////
// proc sync 

extern void proc_lock_init();
extern void proc_lock();
extern void proc_unlock();

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>

union semun {
	int     val;            /* value for SETVAL */
	struct  semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
	u_short *array;         /* array for GETALL & SETALL */
};

static int m_semid = 0;

void proc_lock_init()
{
	int key = ftok("/tmp/key_testproc", 0x66);
	int semid = semget(key, 1, IPC_CREAT|0600);
	if (-1 == semid)
	{
		printf("ERROR1");
	}
	union semun options;
	options.val = 1;
	int ret = semctl(semid, 0, SETVAL, options);
	if (ret < 0)
	{
		printf("ERROR2");
	}
	m_semid = semid;
}

void proc_lock()
{
	struct  sembuf  buf  = { 0, -1, IPC_NOWAIT|SEM_UNDO};
	if ( semop ( m_semid, &buf, 1) == -1)
	{
		printf("ERROR3");
		return;
	}

	//printf("proc_lock OK\n");

	return;
}

void proc_unlock()
{
	struct  sembuf  buf  = { 0, 1, IPC_NOWAIT|SEM_UNDO};
    if ( semop ( m_semid, &buf, 1) == -1)
    {
    	printf("ERROR4");
    	return;
    }

    //printf("proc_unlock OK\n");

    return;
}

//////////////////////////////////////////////////////////////////////////////////////////
// thread sync 


ThreadLock::ThreadLock()
{
	pthread_mutex_init(&m_mutex_lock, NULL);
}

void ThreadLock::lock()
{
	pthread_mutex_lock(&m_mutex_lock);
}

void ThreadLock::unlock()
{
	pthread_mutex_unlock(&m_mutex_lock);
}

//////////////////////////////////////////////////////////////////////////////////////////
// proc share mem

#include <map>

typedef std::map<std::string, void *> PSNAMEADDRMAP;
typedef std::pair<std::string, void *> PSNAMEADDRPAIR;

#define PSLOG printf
static PSNAMEADDRMAP s_psNameAddrMap;

void proc_mem_init(const char* name, unsigned long size)
{
    std::string psname = std::string("");
    psname += std::string("/tmp/pmem_");
    psname += std::string(name);

    // update map
    {
	    PSNAMEADDRMAP::iterator it = s_psNameAddrMap.find(psname);
	    if(it != s_psNameAddrMap.end()) {
	    	PSLOG("proc_mem_init ps already init! s_pShareData = %p\n", it->second);
			return;
	    }
    }

    int iSize = size;
    int fd = ::open(psname.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
    if (fd < 0)
    {
        PSLOG("share memory open fd ERROR, err(%d)\n", errno);
        return;
    }

    int offset = lseek(fd, 0, SEEK_END);
    if (offset < 0)
    {
        PSLOG("share memory lseek end ERROR, err(%d)\n", errno);
        return;
    }
    if (offset < iSize)
    {
        PSLOG("share memory fd init\n");
        lseek(fd, iSize, SEEK_SET);
        write(fd, "\0", 1);
        lseek(fd, 0, SEEK_END);
    }

    void * addr = mmap(0, iSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (MAP_FAILED == addr)
    {
        PSLOG("share memory mmap ERROR, err(%d)\n", errno);
        return;
    }

    // update map
    {
	    PSNAMEADDRMAP::iterator it = s_psNameAddrMap.find(psname);
	    if(it == s_psNameAddrMap.end()) {
	    	PSNAMEADDRPAIR cpair(psname, addr);
	    	s_psNameAddrMap.insert(cpair);
	    }
	    else
	    {
	    	it->second = addr;
	    }
    }

    intptr_t base = (intptr_t)addr;
    PSLOG("share memory create ok: name=%s size=%d addr=%p~%p\n", psname.c_str(), iSize, base, base+iSize);
}

void * proc_mem_get(const char* name)
{
    std::string psname = std::string("");
    psname += std::string("/tmp/pmem_");
    psname += std::string(name);

 	PSNAMEADDRMAP::iterator it = s_psNameAddrMap.find(psname);
    if(it != s_psNameAddrMap.end()) {
		return it->second;
    }
    else
    {
    	return NULL;
    }
}

//////////////////////////////////////////////////////////////////////////////////////////
// proc share map

const int psmap_size = 64;
struct PSMapPair
{
	char key[256];
	char value[256];
};
struct PSKVMap
{
	PSMapPair kvdata[psmap_size];
};
static PSKVMap * s_psmap = NULL;

int psmap_find(const char * key)
{
	printf("::psmap_find key=%s\n", key);

	if (NULL == s_psmap)
	{
		proc_mem_init("map_1", sizeof(PSKVMap));
		s_psmap = (PSKVMap *)proc_mem_get("map_1");
	}

	int ifind = -1;
	if (NULL != s_psmap)
	{
		for (int i = 0; i < psmap_size; ++i)
		{
			if ( 0 == strncmp(s_psmap->kvdata[i].key,key,strlen(key)) )
			{
				ifind = i;
				break;
			}
		}
	}
	return ifind;
}
void psmap_set(const char * key, const char* value)
{
	printf("::psmap_set key=%s value=%s\n", key, value);

	if (NULL == s_psmap)
	{
		proc_mem_init("map_1", sizeof(PSKVMap));
		s_psmap = (PSKVMap *)proc_mem_get("map_1");
	}

	if (NULL != s_psmap)
	{
		int ifind = -1;
		for (int i = 0; i < psmap_size; ++i)
		{
			if ( 0 == strncmp(s_psmap->kvdata[i].key,key,strlen(key)) )
			{
				ifind = i;
				break;
			}
		}

		if (ifind>=0)
		{
			printf("    psmap_set [%d] %s=%s success(update)!\n", ifind, key, value);
			memset(s_psmap->kvdata[ifind].value, 0, 256);
			memcpy(s_psmap->kvdata[ifind].value, value, strlen(value));
		}
		else
		{
			for (int i = 0; i < psmap_size; ++i)
			{
				if ( 0 == strcmp(s_psmap->kvdata[i].key,"") )
				{
					memset(s_psmap->kvdata[i].key, 0, 256);
					memset(s_psmap->kvdata[i].value, 0, 256);

					memcpy(s_psmap->kvdata[i].key, key, strlen(key));
					memcpy(s_psmap->kvdata[i].value, value, strlen(value));
					printf("    psmap_set [%d] %s=%s success(create)!\n", i, s_psmap->kvdata[i].key, s_psmap->kvdata[i].value);
					break;
				}
			}
		}
	}
}
const char* psmap_get(const char * key)
{
	printf("::psmap_get key=%s\n", key);

	if (NULL == s_psmap)
	{
		proc_mem_init("map_1", sizeof(PSKVMap));
		s_psmap = (PSKVMap *)proc_mem_get("map_1");
	}

	if (NULL != s_psmap)
	{
		int ifind = -1;
		for (int i = 0; i < psmap_size; ++i)
		{
			if ( 0 == strncmp(s_psmap->kvdata[i].key,key,strlen(key)) )
			{
				ifind = i;
				break;
			}
		}
		if (ifind>=0)
		{
			printf("    psmap_get %s=%s success\n", key, s_psmap->kvdata[ifind].value);
			return s_psmap->kvdata[ifind].value;
		}
		else
		{
			printf("    psmap_get %s=null\n", key);
			return NULL;
		}
	}
	else
    {
        return NULL;
    }
}


//////////////////////////////////////////////////////////////////////////////////////////
// protect tail mem new
#include <list>
#include <ctime>
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
static ThreadLock s_mmapTableLock;
//static std::map<void*, MMAPITEM> s_mmapTable;
static bool s_bInitFlag = false;
static std::list<MMAPBLOCKITEM> s_mmapFreeList;
static std::map<void*,MMAPBLOCKITEM> s_mmapUsedList;
static const int s_pageSize = 4*1024;
static const int s_maxUserSize = 1*s_pageSize;
static const int s_maxProtectBlockCount = 10;
static const char* s_mmap_fileName = "/tmp/protectmem";
char* s_mmap_baseAddr = NULL;
void* protect_tail_mem_init()
{
	int mmapMemSize = s_maxProtectBlockCount * 2 * s_pageSize;
	int fd = open(s_mmap_fileName, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	lseek(fd, mmapMemSize, SEEK_SET);
	write(fd, "\0", 1);
	char * mem = (char*)mmap(NULL, mmapMemSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	close(fd);

	if (mem)
	{
		printf("mmap file:%s\n", s_mmap_fileName);
		s_mmap_baseAddr = mem;

		for (int i = 0; i < s_maxProtectBlockCount; ++i)
		{
			char* protectBlockBaseAddr = mem+i*2*s_pageSize;
			char* protectBlockReadOnlyAddr = protectBlockBaseAddr + s_pageSize;

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
	}
}
void* protect_tail_mem_deinit()
{
	if (s_mmap_baseAddr)
	{
		int mmapMemSize = s_maxProtectBlockCount * 2 * s_pageSize;
		munmap(s_mmap_baseAddr, mmapMemSize);

		remove(s_mmap_fileName);
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

	if (s_mmapFreeList.size() > 0)
	{
		MMAPBLOCKITEM mmapBlockItem = s_mmapFreeList.front();
		if (size < mmapBlockItem.rwsize)
		{
			// rm freebock in free list
			s_mmapFreeList.pop_front();

			userAddr = mmapBlockItem.baseAddr + mmapBlockItem.rwsize - size;

			// add used map
			s_mmapUsedList[userAddr] = mmapBlockItem;

			printf("protect_tail_new userAddr=%p protectBlockBaseAddr=%p \n", userAddr, mmapBlockItem.baseAddr);
		}
	}

	s_mmapTableLock.unlock();

	return userAddr;
}
void protect_tail_delete(void* addr)
{
	s_mmapTableLock.lock();

	std::map<void*, MMAPBLOCKITEM>::iterator it = s_mmapUsedList.find(addr);
	if(it != s_mmapUsedList.end()) {
		MMAPBLOCKITEM mmitem = it->second;
		if (NULL != mmitem.baseAddr)
		{
			s_mmapFreeList.push_back(mmitem);
			
			s_mmapUsedList.erase(it);

			printf("protect_tail_delete addr=%p protectBlockBaseAddr=%p \n", addr, mmitem.baseAddr);
		}
	}

	s_mmapTableLock.unlock();
}