
#include "MTRecorder.h"
#include <string.h>
#include <new>
#if defined(__linux) || defined(__linux__)
#   include <sys/syscall.h>
#   include <unistd.h>
#elif defined(__APPLE__)
#   include <pthread.h>
#elif defined(_WIN32) || defined(_WIN64)
#   include <windows.h>
#else
#   error "Not supported."
#endif
#include "MTMemoryPrivate.h"

#if defined(_WIN32) || defined(_WIN64)
#   define pid_t DWORD
#   define getpid() (::GetCurrentProcessId())
#endif

const int MAX_THREAD_NAME_LEN = 30;

static pid_t gettid()
{
#if defined(__linux) || defined(__linux__)
    // use TLS to avoid unncessary syscall, saves about 700ms per a million new operation (1024 bytes each) on 17cy MEU.
    static __thread pid_t pid = 0;
    if (0 == pid) {
        pid = syscall(__NR_gettid);
    }
    return pid;
#elif defined(__APPLE__)
    uint64_t tid = 0;
    pthread_t curThread = pthread_self();
    pthread_threadid_np(curThread, &tid);
    return static_cast<int>(tid);
#elif defined(_WIN32) || defined(_WIN64)
    return GetCurrentThreadId();
#endif
}


static const char* MemoryCategoryName[MEMCATEGORY_NUM] = {
    "Unknown",
    "Base",
};

static const char* MemoryReportNewType[] = {
    "ALL",
    "DIFF"
};

MTRecorder::Alloc::Alloc()
    : bytes(0)
    , alloccount(0)
{
}

MTRecorder::AllocEx::AllocEx()
    : bytes(0)
    , alloccount(0)
    , unknowBytes(0)
    , unknowAlloccount(0)
{
#ifdef HAVE_PTHREAD_GETNAME_NP
    threadName[0] = '\0';
#endif
}

MTRecorder::AllocEx::AllocEx(size_t sz, size_t cnt, size_t unknowsz, size_t unknowcnt)
    : bytes(sz)
    , alloccount(cnt)
    , unknowBytes(unknowsz)
    , unknowAlloccount(unknowcnt)
{
#ifdef HAVE_PTHREAD_GETNAME_NP
    pthread_getname_np(pthread_self(), threadName, sizeof(threadName));
#endif
}

MTRecorder::currentData::currentData(size_t sz, unsigned char cat, const char* pFileName, const int lineNo, const int tid)
    : mSize(sz)
    , mCategory(cat)
    , mFileName(NULL)
    , mLineNo(lineNo)
    , mTid(tid)
{
    if (NULL != pFileName) {
        int nFileNameLen = strlen(pFileName);
        mFileName = static_cast<char*>(MEM_malloc_private(nFileNameLen + 1));
        if (NULL == mFileName) {
            MEM_Log_private(false, "MTRecorder::currentData::currentData malloc failed!\n");
            return;
        }
        memcpy(mFileName, pFileName, nFileNameLen);
        mFileName[nFileNameLen] = '\0';
    }
}

MTRecorder::currentData::~currentData()
{
    if (NULL != mFileName) {
        MEM_free_private(mFileName);
        mFileName = NULL;
    }
}

size_t MTRecorder::hashFunc::operator()(void* ptr) const
{
    return ((reinterpret_cast<size_t>(ptr)) >> 2);
}

int MTRecorder::threadIdHashFunc::operator()(int tid) const
{
    return tid;
}

MTRecorder::MTRecorder(void)
    : m_cLock()
    , mAllocations()
    , mThreadAllocations()
    , mLastAllocations()
    , m_bLog2File(false)
    , m_bRecordFileNameAndLineNum(false)
    , m_bReportDiff(false)
    , m_recordCategory(MEMCATEGORY_NUM)
    , m_ReportNo(0)
{
}


MTRecorder::~MTRecorder(void)
{
    CloseLogFile();
}

void MTRecorder::recordAlloc(void* ptr,  unsigned char category, size_t sz, const char* pFileName, const int lineNum)
{
    if (NULL == ptr) {
        return;
    }
    if (category >= MEMCATEGORY_NUM) {
        category = MEMCATEGORY_UNKNOWN;
    }


    int tid = gettid();
    currentData* data = NULL;
    // m_recordCategory == MEMCATEGORY_NUM record all categories
    if (m_bRecordFileNameAndLineNum && (MEMCATEGORY_NUM == m_recordCategory || category == m_recordCategory)) {
        data = new(MEM_malloc_private(sizeof(*data))) currentData(sz, category, pFileName, lineNum, tid);
    }
    else {
        data = new(MEM_malloc_private(sizeof(*data))) currentData(sz, category, NULL, 0, tid);
    }
    MTThreadLock cMemAutoSync(m_cLock);
    mAllocations[ptr] = data;
    m_vecMemory[category].bytes += sz;
    m_vecMemory[category].alloccount += 1;

    ThreadAllocationMap::iterator tidAllocIter = mThreadAllocations.find(tid);
    if (tidAllocIter != mThreadAllocations.end()) {
        tidAllocIter->second.bytes += sz;
        tidAllocIter->second.alloccount += 1;
        if (MEMCATEGORY_UNKNOWN == category) {
            tidAllocIter->second.unknowBytes += sz;
            tidAllocIter->second.unknowAlloccount += 1;
        }
    }
    else {
        if (MEMCATEGORY_UNKNOWN == category) {
            mThreadAllocations[tid] = AllocEx(sz, 1, sz, 1);
        }
        else {
            mThreadAllocations[tid] = AllocEx(sz, 1, 0, 0);
        }
    }
}

void MTRecorder::recordDealloc(void* ptr, size_t& sz)
{
    if (NULL == ptr) {
       return;
    }
    bool bDeallocError = false;
    {
        MTThreadLock cMemAutoSync(m_cLock);
        AllocationMap::iterator i = mAllocations.find(ptr);
        if (i != mAllocations.end()) {
            currentData* alloc = i->second;
            if (alloc->getCategory() < MEMCATEGORY_NUM) {
                sz = alloc->getBytes();
                m_vecMemory[alloc->getCategory()].bytes -= sz;
                m_vecMemory[alloc->getCategory()].alloccount -= 1;
                // last allocation not free only erase the key and value, or it's double free
                AllocationMap::iterator lastIter = mLastAllocations.find(ptr);
                if (lastIter != mLastAllocations.end()) {
                    mLastAllocations.erase(lastIter);
                }
                int tid = alloc->getTheadId();
                ThreadAllocationMap::iterator tidAllocIter = mThreadAllocations.find(tid);
                if (tidAllocIter != mThreadAllocations.end()) {
                    tidAllocIter->second.bytes -= sz;
                    tidAllocIter->second.alloccount -= 1;
                    if (MEMCATEGORY_UNKNOWN == alloc->getCategory()) {
                        tidAllocIter->second.unknowBytes -= sz;
                        tidAllocIter->second.unknowAlloccount -= 1;
                    }
                }

                mAllocations.erase(i);
                alloc->~currentData();
                MEM_free_private(alloc);
            }
        }
        else {
            bDeallocError = true;
        }
    }

    // if (bDeallocError && MemoryTrackerIF::GetErrorLogFunc()) {
    //     int deTid = gettid();
    //     char szThreadName[MAX_THREAD_NAME_LEN] = { 0 };
    //     int ret = GetThreadNameFromTid(deTid, szThreadName, MAX_THREAD_NAME_LEN);
    //     if (ret < 0) {
    //         MemoryTrackerIF::GetErrorLogFunc()(LogLvl_Error, MEMTRACK_TAG, "Thread:(%d)Unknow, Deallocate Not Found ptr:%p\n", deTid, ptr);
    //     }
    //     else {
    //         MemoryTrackerIF::GetErrorLogFunc()(LogLvl_Error, MEMTRACK_TAG, "Thread:(%d)%s, Deallocate Not Found ptr:%p\n", deTid, szThreadName, ptr);
    //     }
    // }
}

std::string MTRecorder::ReportStatus(unsigned char category)
{
    std::string strOut;
    if (category > MEMCATEGORY_NUM) {
        MEM_Log_private(m_bLog2File, "MTRecorder::ReportStatus Invalid Param:category:%d\n", category);
    }
    // < MEMCATEGORY_NUM
    else if (category < MEMCATEGORY_NUM) {
        char szTempOut[MAX_LEN_PER_MODULE_RECORD] = { 0 };
        snprintf(szTempOut, MAX_LEN_PER_MODULE_RECORD, "ModuleName %10s  size:: [%12d]bytes, [%12.6f]M Operate Count: %12d\n",
            MemoryCategoryName[category], static_cast<unsigned int>(m_vecMemory[category].bytes),
            (static_cast<float>(m_vecMemory[category].bytes))/1024/1024, static_cast<unsigned int>(m_vecMemory[category].alloccount));
        strOut += szTempOut;
        MEM_Log_private(m_bLog2File, "%s", szTempOut);
    }
    else {
        // == MEMCATEGORY_NUM, output all
        for (int i = 0; i < MEMCATEGORY_NUM; ++i) {
            // output log
            char szTempOut[MAX_LEN_PER_MODULE_RECORD] = { 0 };
            snprintf(szTempOut, MAX_LEN_PER_MODULE_RECORD, "ModuleName %10s  size:: [%12u]bytes, [%12.6f]M Operate Count: %12u\n",
                MemoryCategoryName[i], static_cast<unsigned int>(m_vecMemory[i].bytes),
                (static_cast<float>(m_vecMemory[i].bytes))/1024/1024, static_cast<unsigned int>(m_vecMemory[i].alloccount));
            strOut += szTempOut;
            MEM_Log_private(m_bLog2File, "%s", szTempOut);
        }
        {
            MTThreadLock cMemAutoSync(m_cLock);
            // output the thread alloc infomation
            ReportThreadAllocate(strOut);
            // output nondeleted new
            MEM_Log_private(m_bLog2File, "=============================list %s nondeleted new No:[%12d] BEGIN=============================\n", MemoryReportNewType[m_bReportDiff], m_ReportNo);
            if (m_bReportDiff) {
                ReportDiffNewAllocate();
                mLastAllocations = mAllocations;
            }
            else {
                ReportAllNewAllocate();
            }

            MEM_Log_private(m_bLog2File, "=============================list %s nondeleted new No:[%12d] END=============================\n", MemoryReportNewType[m_bReportDiff], m_ReportNo);
            ++m_ReportNo;
        }
    }
    FlushLogFile();
    CloseLogFile();
    return strOut;
}

void MTRecorder::CheckMemoryOverFlow()
{
    // check all allocation, if it's overflow, write error log.
#ifdef MEM_COMP_OPT_USE_MAGIC_CODE
    MTThreadLock cMemAutoSync(m_cLock);
    AllocationMap::iterator iter = mAllocations.begin();
    for (; iter != mAllocations.end(); ++iter) {
        void* ptr = iter->first;
        size_t sz = iter->second->getBytes();
        if (sz > EXTRA_MAGIC_LEN
            && 0 != memcmp(reinterpret_cast<char*>(ptr) + sz - EXTRA_MAGIC_LEN, &EXTRA_MAGIC_CODE, EXTRA_MAGIC_LEN)) {
            // overflow, write to error log file
            char szThreadName[MAX_THREAD_NAME_LEN] = { 0 };
            int ret = GetThreadNameFromTid(iter->second->getTheadId(), szThreadName, MAX_THREAD_NAME_LEN);
            if (ret < 0) {
                memcpy(szThreadName, "Unknow", sizeof("Unknow"));
            }
            if (NULL != iter->second->getFileName()) {
                MEM_Log2Serial_private("Module:%d thread:%s ptr:%p, overflow file:%s line:%d, size:%" MEM_PRIusize_t "\n", iter->second->getCategory(),
                    szThreadName, ptr, iter->second->getFileName(), iter->second->getLineNum(), sz);
            }
            else {
                MEM_Log2Serial_private("Module:%d thread:%s ptr:%p, overflow, size:%" MEM_PRIusize_t "\n", iter->second->getCategory(), szThreadName, ptr, sz);
            }
        }

    }
#endif
}

void MTRecorder::ReportDiffNewAllocate()
{
    // report added between last and current not deleted new file and position
    AllocationMap::iterator iter = mAllocations.begin();
    for (; iter != mAllocations.end(); ++iter) {
        if (m_bRecordFileNameAndLineNum
            && (MEMCATEGORY_NUM == m_recordCategory || iter->second->getCategory() == m_recordCategory)
            && NULL != iter->second->getFileName()) {
            // find in last alloc and compare name , line and size, same continue
            AllocationMap::iterator lastAllocIter = mLastAllocations.find(iter->first);
            if (lastAllocIter != mLastAllocations.end()) {
                if (0 == strcmp(iter->second->getFileName(), lastAllocIter->second->getFileName())
                    && iter->second->getLineNum() == lastAllocIter->second->getLineNum()
                    && iter->second->getBytes() == lastAllocIter->second->getBytes()) {
                    continue;
                }
            }

            const char* pFileName = iter->second->getFileName();
            MEM_Log_private(m_bLog2File, "Module:[%10s] ThreadId:[%d] FileName:[%s] Line:[%d] ptr:%p, size:[%d]\n", MemoryCategoryName[iter->second->getCategory()],
                iter->second->getTheadId(), pFileName, iter->second->getLineNum(), iter->first, iter->second->getBytes());
            pFileName = strrchr(pFileName, '/');
            if (NULL != pFileName && 0 == strcmp(pFileName, "/CL_String.cpp")) {
                MEM_Log_private(m_bLog2File, "CL_String content:%s\n", (char*)iter->first);
            }
        }
    }
}

void MTRecorder::ReportAllNewAllocate()
{
    // report current all not deleted new file and position
    AllocationMap::iterator iter = mAllocations.begin();
    for (; iter != mAllocations.end(); ++iter) {
        if (m_bRecordFileNameAndLineNum
            && (MEMCATEGORY_NUM == m_recordCategory || iter->second->getCategory() == m_recordCategory)
            && NULL != iter->second->getFileName()) {
            const char* pFileName = iter->second->getFileName();
            MEM_Log_private(m_bLog2File, "Module:[%10s] ThreadId:[%d] FileName:[%s] Line:[%d] ptr:%p, size:[%d]\n", MemoryCategoryName[iter->second->getCategory()],
                iter->second->getTheadId(), pFileName, iter->second->getLineNum(), iter->first, iter->second->getBytes());
            pFileName = strrchr(pFileName, '/');
            if (NULL != pFileName && 0 == strcmp(pFileName, "/CL_String.cpp")) {
                MEM_Log_private(m_bLog2File, "CL_String content:%s\n", (char*)iter->first);
            }
        }
    }
}

void MTRecorder::ReportThreadAllocate(std::string & strOut)
{
    ThreadAllocationMap::iterator threadIter = mThreadAllocations.begin();
    while (threadIter != mThreadAllocations.end()) {
        char szTempOut[MAX_LEN_PER_MODULE_RECORD] = { 0 };
        char szThreadName[MAX_THREAD_NAME_LEN] = { 0 };
        int ret = GetThreadNameFromTid(threadIter->first, szThreadName, MAX_THREAD_NAME_LEN);
        if (ret < 0) {
            szThreadName[0] = '\0';
            if (threadIter->second.bytes == 0) {
                mThreadAllocations.erase(threadIter++);
                continue;
            }
        }
        snprintf(szTempOut, MAX_LEN_PER_MODULE_RECORD,
            "Thread(%d) %16s  size/Unknown [%10u/%10u]bytes, [%10.6f/%10.6f]M Operate Count: %12u/%12u\n",
            threadIter->first, szThreadName,
            static_cast<unsigned int>(threadIter->second.bytes), static_cast<unsigned int>(threadIter->second.unknowBytes),
            (static_cast<float>(threadIter->second.bytes))/1024/1024, (static_cast<float>(threadIter->second.unknowBytes))/1024/1024,
            static_cast<unsigned int>(threadIter->second.alloccount),
            static_cast<unsigned int>(threadIter->second.unknowAlloccount));
        strOut += szTempOut;
        MEM_Log_private(m_bLog2File, "%s", szTempOut);
        ++threadIter;
    }
}

int MTRecorder::GetThreadNameFromTid(int tid, char* szNameBuf, int bufSize)
{
    int retval = -1;
    if (NULL == szNameBuf || bufSize < 1) {
        return retval;
    }

#ifdef HAVE_PTHREAD_GETNAME_NP
    ThreadAllocationMap::iterator tidAllocIter = mThreadAllocations.find(tid);
    if (tidAllocIter != mThreadAllocations.end()) {
        retval = strlen(tidAllocIter->second.threadName);
        strncpy(szNameBuf, tidAllocIter->second.threadName, bufSize);
        if (retval >= bufSize) {
            szNameBuf[bufSize - 1]  = '\0';
        }
    }
#elif defined(__linux) || defined(__linux__)
    pid_t pid = getpid();
    size_t field_len = strlen("Name:");
    FILE * status_file = NULL;
    char buf[100] = { 0 };
    memset(buf, 0, sizeof(buf));
    if (tid > 0) {
        snprintf(buf, sizeof(buf) - 1, "/proc/%d/task/%d/status", pid, tid);
    }
    else {
        snprintf(buf, sizeof(buf) - 1, "/proc/%d/status", pid);
    }

    status_file = fopen(buf, "r");
    if (status_file == NULL) {
        return -1;
    }

    while (fgets(buf, sizeof(buf), status_file)) {
        if (0 == strncmp(buf, "Name:", field_len)) {
            size_t pos = field_len;
            while ((pos < strlen(buf)) && isspace(buf[pos])) {
                pos++;
            }

            strncpy(szNameBuf, &buf[pos], bufSize);
            szNameBuf[bufSize - 1] = '\0';
            retval = strlen(szNameBuf);
            if (retval > 0 && szNameBuf[retval - 1] == '\n') {
                --retval;
                szNameBuf[retval] = '\0';
            }
            break;
        }
    }

    fclose(status_file);
#endif
    return retval;
}

void MTRecorder::BeginAnalyze()
{
    MTThreadLock cMemAutoSync(m_cLock);
    for (int i = 0; i < MEMCATEGORY_NUM; ++i) {
        m_vecMemoryBegin[i].bytes = m_vecMemory[i].bytes;
        m_vecMemoryBegin[i].alloccount = m_vecMemory[i].alloccount;
    }
    mLastAllocations = mAllocations;
}

std::string MTRecorder::ReportLeaks()
{
    std::string strOut;
    for (int i = 0; i < MEMCATEGORY_NUM; ++i) {
        // output log
        char szTempOut[MAX_LEN_PER_MODULE_RECORD] = { 0 };
        snprintf(szTempOut, MAX_LEN_PER_MODULE_RECORD, "ModuleName %10s  size:: [%lu]bytes, [%12.6f]M Operate Count: %lu\n",
            MemoryCategoryName[i], m_vecMemory[i].bytes - m_vecMemoryBegin[i].bytes,
            (static_cast<float>(m_vecMemory[i].bytes - m_vecMemoryBegin[i].bytes))/1024/1024, m_vecMemory[i].alloccount - m_vecMemoryBegin[i].alloccount);
        MEM_Log_private(m_bLog2File, "%s", szTempOut);
        strOut += szTempOut;
    }
    return strOut;
}


void MTRecorder::SetLog2File(bool bLog2File)
{
    if (m_bLog2File != bLog2File) {
        m_bLog2File = bLog2File;
        if (!m_bLog2File) {
            MTThreadLock cMemAutoSync(m_cLock);
            FlushLogFile();
            CloseLogFile();
        }
    }
}

void MTRecorder::SetRecordFileNameAndLineNum(bool bRecord, unsigned int category, bool bDiff)
{
    if (!m_bReportDiff && bDiff) {
        BeginAnalyze();
    }
    MTThreadLock cMemAutoSync(m_cLock);
    m_bRecordFileNameAndLineNum = bRecord;
    m_recordCategory = category;
    m_bReportDiff = bDiff;
}

/* EOF */
