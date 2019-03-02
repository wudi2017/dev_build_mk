/**
 * Copyright @ 2014 - 2017 Suntec Software(Shanghai) Co., Ltd.
 * All Rights Reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are NOT permitted except as agreed by
 * Suntec Software(Shanghai) Co., Ltd.
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */

#ifndef CXX_LOCK_SEQUENCE_H
#define CXX_LOCK_SEQUENCE_H

#ifndef CXX_LOCK_SYNCOBJ_H
#   include "LOCK_SyncObj.h"
#endif
#ifndef CXX_MAPFILE_H
#   include "MapFile.h"
#endif
#ifndef CXX_MAPTHREAD_H
#include "MapThread.h"
#endif
#include <cstddef>
#include <ext/hash_map>
#include <ext/malloc_allocator.h>
#include <set>
#include <list>

//
struct LOCK_LockObj 
{
    void* pAddress;
    unsigned int uiThreadID;
};
typedef __gnu_cxx::malloc_allocator< std::pair< unsigned int, LOCK_LockObj > > allocator_LOCK_TableLockObjs;
typedef __gnu_cxx::hash_map<unsigned int,LOCK_LockObj, __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>, allocator_LOCK_TableLockObjs > LOCK_TableLockObjs;

//
struct LOCK_ThreadObj 
{
    unsigned int uiThreadID;
    std::string strThreadName;
};
typedef __gnu_cxx::malloc_allocator< std::pair< unsigned int, LOCK_ThreadObj > >  allocator_LOCK_TableThreadObjs;
typedef __gnu_cxx::hash_map<unsigned int,LOCK_ThreadObj, __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>, allocator_LOCK_TableThreadObjs > LOCK_TableThreadObjs;

//
struct LOCK_SequenceObj
{
    void* pAddress;;
    std::string strSequenceName;
};
typedef std::vector<LOCK_SequenceObj, __gnu_cxx::malloc_allocator<LOCK_SequenceObj> > LOCK_AddrSequence;
struct LOCK_LockSequence
{
    LOCK_AddrSequence arrayLocks;
};
struct LockSequence_less
{   
public:   
    bool operator()(const LOCK_LockSequence& _Left, const LOCK_LockSequence& _Right) const   
    {   
        size_t lsize = _Left.arrayLocks.size();
        size_t rsize = _Right.arrayLocks.size();
        if (lsize < rsize) {
            return true;
        }
        else if (lsize > rsize) {
            return false;
        }

        //if (lsize < 0) {
            for (size_t i = 0; i < lsize; ++i) {
                if (_Left.arrayLocks[i].pAddress < _Right.arrayLocks[i].pAddress) {
                    return true;
                }
                else if (_Left.arrayLocks[i].pAddress > _Right.arrayLocks[i].pAddress)  {
                    return false;
                }
            }
        //}
        //else {
        //    if (&_Left < &_Right) {
        //        return true;
        //    }
        //    else if (&_Left > &_Right) {
        //        return false;
        //    }
        //}

        return false;   
    }   
};
typedef std::set<LOCK_LockSequence, LockSequence_less, __gnu_cxx::malloc_allocator<LOCK_LockSequence> > LOCK_VecLockSequences;
//typedef std::list<LOCK_LockSequence> LOCK_VecLockSequences;
//
/*
struct string_less : public stdext::hash_compare<std::string>
{   
public:   
    bool operator()(const std::string& _Left, const std::string& _Right) const
    {   
        return(_Left.compare(_Right) < 0 ? true : false);
    }   
};
*/
struct LOCK_ThreadLock {
    unsigned int uiThreadID;
    std::string strThreadName;
    LOCK_LockSequence cCurrentSequence;
    LOCK_VecLockSequences vecLockSequence;
};
typedef __gnu_cxx::malloc_allocator< std::pair< unsigned int, LOCK_ThreadLock > >  allocator_LOCK_TableThreadLocks;
typedef __gnu_cxx::hash_map<unsigned int,LOCK_ThreadLock, __gnu_cxx::hash<unsigned int>, std::equal_to<unsigned int>, allocator_LOCK_TableThreadLocks > LOCK_TableThreadLocks;

//
class LOCK_Sequence : public MapThread
{
public:
    static LOCK_Sequence* Instance(void);
    static void Destroy(void);
    void Regist(void* pAddress);
    void Lock(void* pAddress);
    void UnLock(void* pAddress);
    void CheckAndOutputToFile(void);
    void OutputToFile(void);
    virtual void Execute();

public:
    void DoRegist(void* pAddress, unsigned int  uiThreadID = 0);
    void DoLock(void* pAddress, unsigned int  uiThreadID = 0);
    void DoUnLock(void* pAddress, unsigned int  uiThreadID = 0);
    void DoCheckAndOutputToFile(void);
    void DoOutputToFile(void);

private:
    void LEE_UtilityLog_TermOutTerm_V(const char *prefix, const char *message, va_list args);
    void LEE_UtilityLog_Term(const char *message, ...);    

private:

    LOCK_TableThreadObjs    m_cTableThreadObjs;
    LOCK_TableLockObjs   m_cTableLockObjs;
    LOCK_TableThreadLocks m_cTableThreadLocks;
    LOCK_SyncObj    m_cSyncObj;
    int             m_iNum;
    MapFile         m_cMapFile;
    bool            m_bDump;
    bool            m_bStop;
    bool            m_bStackOff;
    int             m_iStackStep;

private:
    
    static LOCK_Sequence*       s_pcInstance;
    static LOCK_SyncObj*        s_pSyncObj;
    static LOCK_SyncObj         s_cSyncObj;
    LOCK_Sequence();
    ~LOCK_Sequence();
};

#endif /* >>CXX_LOCK_SEQUENCE_H<< */
/* EOF */

