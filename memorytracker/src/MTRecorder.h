
#ifndef MTRECORDER_H
#define MTRECORDER_H

#include <sys/types.h>

#ifdef _MSC_VER
#   include <hash_map>
#   define HashMap stdext::hash_map
#   define Hash_Space stdext
#elif defined(__GNUC__)
#   include <ext/hash_map>
#   define HashMap __gnu_cxx::hash_map
#   define Hash_Space __gnu_cxx
#endif

#include "MTMemoryConfig.h"
#include "MTThreadLock.h"
#include "MTSTLAllocator.h"
#include <string>

#define MAX_LEN_PER_MODULE_RECORD 200

class MTRecorder
{
public:
    MTRecorder(void);
    virtual ~MTRecorder(void);
    virtual void recordAlloc(void* ptr, unsigned char category, size_t sz, const char* pFileName, const int lineNo);
    virtual void recordDealloc(void* ptr, size_t& sz);
    virtual std::string ReportStatus(unsigned char category = MEMCATEGORY_NUM);
    virtual void BeginAnalyze();
    virtual std::string ReportLeaks();
    virtual void CheckMemoryOverFlow();
    virtual void SetLog2File(bool bLog2File);
    virtual void SetRecordFileNameAndLineNum(bool bRecord, unsigned int category = MEMCATEGORY_NUM, bool bDiff = false);

protected:
    struct Alloc
    {
        size_t bytes;
        size_t alloccount;
        Alloc();
    };

    struct AllocEx
    {
        size_t bytes;
        size_t alloccount;
        size_t unknowBytes;
        size_t unknowAlloccount;
#ifdef HAVE_PTHREAD_GETNAME_NP
        char threadName[16];
#endif
        AllocEx();
        AllocEx(size_t sz, size_t cnt, size_t unknowsz, size_t unknowcnt);
    };

    /**
     * @description of currentData
     *
     * @details definition and inteface for currentData
     */
    class currentData
    {
    public:
        currentData(size_t sz, unsigned char cat, const char* pFileName, const int lineNum, const int tid);
        ~currentData();

        inline const char* getFileName() const
        {
            return mFileName;
        }

        inline size_t getBytes() const
        {
            return mSize;
        }

        inline unsigned char getCategory() const
        {
            return mCategory;
        }

        inline int getLineNum() const
        {
            return mLineNo;
        }

        inline int getTheadId() const
        {
            return mTid;
        }

    private:
        currentData(const currentData&);
        currentData& operator =(const currentData&);

        size_t          mSize;
        unsigned char   mCategory;
        char*           mFileName;
        int             mLineNo;
        int             mTid;
    };

    struct hashFunc
    {
        size_t operator()(void* ptr) const;
    };

    struct threadIdHashFunc
    {
        int operator()(int tid) const;
    };

    // report current diff with last report: new added allocate filename and linenum
    void ReportDiffNewAllocate();

    // report current all allocate filename and linenum
    void ReportAllNewAllocate();

    // report current all threads' memory bytes and operator count
    void ReportThreadAllocate(std::string & strOut);

    int GetThreadNameFromTid(int tid, char* szNameBuf, int bufSize);

private:
#ifdef _MSC_VER
    typedef HashMap<void*, currentData*, Hash_Space::hash_compare<void*, std::less<void*> >, STLAllocator<currentData*> > AllocationMap;
#else
    typedef HashMap<void*, currentData*, hashFunc, std::equal_to<void*>, STLAllocator<currentData*> > AllocationMap;
#endif
#ifdef _MSC_VER
    typedef HashMap<int, AllocEx, Hash_Space::hash_compare<int, std::less<int> >, STLAllocator<AllocEx> > ThreadAllocationMap;
#else
    typedef HashMap<int, AllocEx, threadIdHashFunc, std::equal_to<int>, STLAllocator<AllocEx> > ThreadAllocationMap;
#endif
    MTThreadLock		m_cLock;
    AllocationMap       mAllocations;                   // current allocations
    Alloc               m_vecMemory[MEMCATEGORY_NUM];   // allocation statistics
    ThreadAllocationMap mThreadAllocations;             // current allocations statistics of each thread

    AllocationMap       mLastAllocations;               // last allocations map, for differential report
    Alloc               m_vecMemoryBegin[MEMCATEGORY_NUM];  // last allocation statistics, for differential report

    bool                m_bLog2File;                    // if log report to file
    bool                m_bRecordFileNameAndLineNum;    // if record allocation detail (file, line, etc.)
    bool                m_bReportDiff;                  // if differential report mode is on
    unsigned int        m_recordCategory;               // report on which category
    unsigned int        m_ReportNo;                     // 0-based report index

};

#endif //MTRECORDER_H