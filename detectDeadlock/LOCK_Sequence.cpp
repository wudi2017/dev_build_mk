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
#   include "LOCK_Sequence.h" 
#endif
#if defined(_WIN32) || defined(_WIN32_WCE)
#   include <Tchar.h>
#else    // For Linux
#   include <pthread.h>
#endif
#include <sys/prctl.h>
#include <stdarg.h>
#include "ncore/NCLog.h"
#include "LOCK_StartPerformance.h"
#include <execinfo.h>
#include <dlfcn.h>
#include <stdio.h>
#include <signal.h>
#define _vsnprintf vsnprintf 
#define _snprintf snprintf 

#define LOCK_USE_SYNC 1

LOCK_Sequence*  LOCK_Sequence::s_pcInstance = new LOCK_Sequence();
LOCK_SyncObj* LOCK_Sequence::s_pSyncObj = NULL;
static LOCK_SyncObj* g_pSyncObj = new LOCK_SyncObj();
static int g_iLockProtect = 0;
unsigned int g_uiTickCountLock = 0;
unsigned int g_uiTickCountLockTimes = 0;
unsigned int g_uiTickCountUnLock = 0;
unsigned int g_uiTickCountUnLockTimes = 0;
int g_bLockDoing = false;
int g_bUnLockDoing = false;
std::string g_strThreadName = "";
char* g_chDirBase = "/pdata";

/*---------------------------------------------------------------------------*/
// out put by va_list
VOID 
LEE_UtilityLog_TermOutTermOut_V(const char *prefix, const char *message, va_list args)
{
    // init
    char       str[256];
    std::string   mess;
    memset(str, 0, sizeof(str));

    // output
    _vsnprintf(str, 256 - 1, message, args);
    mess += std::string(str);
    mess += std::string("\n");

    using namespace LOCKSEQUENCE;
    NCSTARTPERF_KSTART;
    NCSTARTPERF_OUT(prefix, mess.c_str());
    NCSTARTPERF_END;  
}

VOID 
LEE_UtilityLog_TermOut(const char *message, ...)
{
    va_list args;

    // var list do with start
    va_start(args, message);

    // out put
    LEE_UtilityLog_TermOutTermOut_V("LockSequence", message, args);

    // var list do with over
    va_end(args);
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{   
    unsigned int uiTickCountLast = GetTheTickCountV();
    if (NULL != g_pSyncObj
        && NULL != LOCK_Sequence::Instance()) {
        LOCK_Sequence::Instance()->Lock(static_cast<void*>(mutex));
    }
    
    g_uiTickCountLock += (GetTheTickCountV() - uiTickCountLast);
    
    
    int(*pthread_mutex_lock_ptr)(pthread_mutex_t *);
    pthread_mutex_lock_ptr = reinterpret_cast<int (*)(pthread_mutex_t*)>(dlsym(RTLD_NEXT, "pthread_mutex_lock"));
    int iresult = pthread_mutex_lock_ptr(mutex);
    g_uiTickCountLockTimes++;
    return iresult;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    unsigned int uiTickCountLast = GetTheTickCountV();
    if (NULL != g_pSyncObj
        && NULL != LOCK_Sequence::Instance()) {
        LOCK_Sequence::Instance()->UnLock(static_cast<void*>(mutex));
    }
    
    g_uiTickCountUnLock += (GetTheTickCountV() - uiTickCountLast);
    
    int(*pthread_mutex_unlock_ptr)(pthread_mutex_t *);
    pthread_mutex_unlock_ptr = reinterpret_cast<int (*)(pthread_mutex_t*)>(dlsym(RTLD_NEXT, "pthread_mutex_unlock"));
    int iresult = pthread_mutex_unlock_ptr(mutex);
    g_uiTickCountUnLockTimes++;
    return iresult;
}

LOCK_Sequence::LOCK_Sequence()
:MapThread(5000)
,m_cTableThreadObjs()
,m_cTableLockObjs()
,m_cTableThreadLocks()
,m_cSyncObj()
,m_iNum(0)
,m_cMapFile()
,m_bDump(false)
,m_bStop(false)
,m_bStackOff(false)
,m_iStackStep(2)
{
    if (getpid() < 100) {
        m_bStop = true;
    }

    this->Start();
}

LOCK_Sequence::~LOCK_Sequence()
{
    m_cMapFile.CloseFile();
}

LOCK_Sequence* LOCK_Sequence::Instance(void)
{

    if (NULL == s_pSyncObj) {
        s_pSyncObj = g_pSyncObj;
    }

    if (NULL == s_pSyncObj) {
        return NULL;
    }

    //s_pSyncObj->SyncStart();
    //g_iLockProtect++;
    //if (NULL == s_pcInstance) {
        //s_pcInstance = new LOCK_Sequence();  
        //s_pcInstance->Start();  
        //LEE_UtilityLog_TermOut("my pid is start new  %d", getpid());    
    //}
    //g_iLockProtect--;
    //s_pSyncObj->SyncEnd();
    
    return s_pcInstance;
}

VOID LOCK_Sequence::Destroy(void)
{
    if (NULL == s_pSyncObj) {
        return;
    }
    
    s_pSyncObj->SyncStart();  
    g_iLockProtect++; 
    if (s_pcInstance != NULL) {

        delete s_pcInstance;
        s_pcInstance = NULL;
    }
    g_iLockProtect--;
    s_pSyncObj->SyncEnd();
}

VOID LOCK_Sequence::Execute()
{
    // init
    char       str[256];
    std::string   mess;

    // record flag
    bool bStop = false;
    MapFile cRecordFile;
    memset(str, 0, sizeof(str));
    _snprintf(str, 256 - 1, "%s/LockSequence/RecordConfig.txt", g_chDirBase);
    std::string strPath(str);
    if (cRecordFile.OpenFile(strPath.c_str(), "r")) {
        cRecordFile.CloseFile();
        
        // check only pid
        memset(str, 0, sizeof(str));
        _snprintf(str, 256 - 1, "%s/LockSequence/RecordConfig_%d.txt", g_chDirBase, getpid());
        std::string strPath(str);
        if (!cRecordFile.OpenFile(strPath.c_str(), "r")) {
            bStop = true;
        }
        else {
            cRecordFile.CloseFile();
        }

        // check call stack
        if (!cRecordFile.OpenFile((std::string(g_chDirBase) + "/LockSequence/RecordConfig_NoStack.txt").c_str(), "r")) {
            m_bStackOff = false;

            if (cRecordFile.OpenFile((std::string(g_chDirBase) + "/LockSequence/RecordConfig_Stack3.txt").c_str(), "r")) {
                m_iStackStep = 3;
                cRecordFile.CloseFile();
            }
            else if (cRecordFile.OpenFile((std::string(g_chDirBase) + "/LockSequence/RecordConfig_Stack4.txt").c_str(), "r")) {
                m_iStackStep = 4;
                cRecordFile.CloseFile();
            }
            else {
                m_iStackStep = 2;
            }
        }
        else {
            m_bStackOff = true;
            cRecordFile.CloseFile();
        }
    }
    else {
        m_bStackOff = true;
    }

    // debug
    if (getpid() < 100) {
        bStop = true;
    }

    // 
    bool  bChange = false;
    if (m_bStop != bStop) {
        bChange = true;
        //LEE_UtilityLog_TermOut("my pid is %d,now runstate... %d", getpid(), !bStop);
    }
    m_bStop = bStop;

    if (m_bStop) {
        return;
    }

    //LEE_UtilityLog_TermOut("all com time is %d,%lf,%lf,%d,%d,%d,%d,%s", getpid(),g_uiTickCountLock*1.0/g_uiTickCountLockTimes,g_uiTickCountUnLock*1.0/g_uiTickCountUnLockTimes,g_uiTickCountLockTimes,g_uiTickCountUnLockTimes,g_bLockDoing,g_bUnLockDoing,g_strThreadName.c_str());

    // dump progress
    if (!m_bDump) {
        MapFile cConfigFile;
        std::string strPath(std::string(g_chDirBase) + "/LockSequence/DumpConfig.txt");
        if (cConfigFile.OpenFile(strPath.c_str(),"r")) {

            cConfigFile.CloseFile();
            
            LEE_UtilityLog_TermOut("my pid is %d,start dump ...", getpid());
            OutputToFile();
            CheckAndOutputToFile();
            m_cMapFile.Flush();
            m_cMapFile.CloseFile();

            //
            memset(str, 0, sizeof(str));
            _snprintf(str, 256 - 1, "%s/LockSequence/DumpOk_%d.txt", g_chDirBase, getpid());
            std::string strPath(str);
            if (cConfigFile.OpenFile(strPath.c_str(), "a"))
            {
                LEE_UtilityLog_TermOut("my pid is %d,dump OK...", getpid());
                cConfigFile.CloseFile();
                m_bDump = true;
            }
            else {
                LEE_UtilityLog_TermOut("dump wrong,please remonut the udisk for enough right...");
            }  
        }
    }
    else {
            MapFile cDumpFile;

            //
            memset(str, 0, sizeof(str));
            _snprintf(str, 256 - 1, "%s/LockSequence/DumpOk_%d.txt", g_chDirBase, getpid());
            std::string strPath(str);
            if (!cDumpFile.OpenFile(strPath.c_str(), "r"))
            {
                m_bDump = false;
            }
            else {
                cDumpFile.CloseFile();
            }
    }    
}

void
LOCK_Sequence::Regist(void* pAddress)
{
#if !LOCK_USE_SYNC
    LOCK_TaskRegistLock* pTask = new LOCK_TaskRegistLock(pAddress, pthread_self(),*this);
    m_cTaskManager.PostTask(pTask);
#else
    if (NULL == s_pSyncObj) {
        return;
    }
    
    s_pSyncObj->SyncStart();  
    if (g_iLockProtect > 0) {
        s_pSyncObj->SyncEnd();
        return;
    }

    g_iLockProtect++;
    DoRegist(pAddress);
    g_iLockProtect--;
    s_pSyncObj->SyncEnd();
#endif
}

void
LOCK_Sequence::Lock(void* pAddress)
{
#if !LOCK_USE_SYNC
    LOCK_TaskLock* pTask = new LOCK_TaskLock(pAddress, pthread_self(), *this);
    m_cTaskManager.PostTask(pTask);
#else
    if (NULL == s_pSyncObj) {
        return;
    }

    if (m_bStop) {
        return;
    }
    
    s_pSyncObj->SyncStart();  
    if (g_iLockProtect > 0) {
        s_pSyncObj->SyncEnd();
        return;
    }

    g_iLockProtect++;
    DoLock(pAddress);
    g_iLockProtect--;
    s_pSyncObj->SyncEnd();
#endif
}

void
LOCK_Sequence::UnLock(void* pAddress)
{
#if !LOCK_USE_SYNC
    LOCK_TaskUnLock* pTask = new LOCK_TaskUnLock(pAddress, pthread_self(), *this);
    m_cTaskManager.PostTask(pTask);
#else
    if (NULL == s_pSyncObj) {
        return;
    }

    if (m_bStop) {
        return;
    }
    
    s_pSyncObj->SyncStart(); 
    if (g_iLockProtect > 0) {
        s_pSyncObj->SyncEnd();
        return;
    }

    g_iLockProtect++; 
    DoUnLock(pAddress);
    g_iLockProtect--;
    s_pSyncObj->SyncEnd();
#endif
}

void
LOCK_Sequence::CheckAndOutputToFile(void)
{
#if !LOCK_USE_SYNC
    LOCK_CheckOutToFile* pTask = new LOCK_CheckOutToFile(*this);
    m_cTaskManager.PostTask(pTask);
#else
    if (NULL == s_pSyncObj) {
        return;
    }

    s_pSyncObj->SyncStart();  
    if (g_iLockProtect > 0) {
        s_pSyncObj->SyncEnd();
        return;
    }

    g_iLockProtect++; 
    DoCheckAndOutputToFile();
    g_iLockProtect--;
    s_pSyncObj->SyncEnd();
#endif
}

void
LOCK_Sequence::OutputToFile(void){
#if !LOCK_USE_SYNC
    LOCK_OutToFile* pTask = new LOCK_OutToFile(*this);
    m_cTaskManager.PostTask(pTask);
#else
    if (NULL == s_pSyncObj) {
        return;
    }
    
    s_pSyncObj->SyncStart(); 
    if (g_iLockProtect > 0) {
        s_pSyncObj->SyncEnd();
        return;
    }

    g_iLockProtect++;  
    DoOutputToFile();
    g_iLockProtect--;
    s_pSyncObj->SyncEnd();
#endif
}

void
LOCK_Sequence::DoRegist(void* pAddress, unsigned int  uiThreadID)
{
    int iThreadID = uiThreadID;
    if (iThreadID == 0) {
        iThreadID = pthread_self();
    }

    LOCK_LockObj cLockObj;
    cLockObj.pAddress = pAddress;
    cLockObj.uiThreadID = iThreadID;
    m_cTableLockObjs[reinterpret_cast<int>(cLockObj.pAddress)] = cLockObj;

}

void
LOCK_Sequence::DoLock(void* pAddress, unsigned int  uiThreadID)
{
    g_bLockDoing = 1;
    int iThreadID = uiThreadID;
    if (iThreadID == 0) {
        iThreadID = pthread_self();
    }

    char tname[128];  
    prctl(PR_GET_NAME, tname); 
    g_strThreadName = tname;

    g_bLockDoing = 2;
    LOCK_TableThreadLocks::iterator it = m_cTableThreadLocks.find(iThreadID);
    if (it == m_cTableThreadLocks.end()) {
        char tname[128];  
        prctl(PR_GET_NAME, tname); 

        LOCK_ThreadLock  cThreadLock;
        cThreadLock.uiThreadID = iThreadID;
        cThreadLock.strThreadName = "";
        m_cTableThreadLocks[iThreadID] = cThreadLock;
        it = m_cTableThreadLocks.find(iThreadID);
        
        
        LOCK_ThreadObj cThreadObj;
        cThreadObj.uiThreadID = iThreadID;
        cThreadObj.strThreadName = "";
        m_cTableThreadObjs[iThreadID] = cThreadObj;
    }

    g_bLockDoing = 3;
    if (it != m_cTableThreadLocks.end()) {
        LOCK_ThreadLock* pThreadLock = &(it->second);

        std::string strResult("");

        // every time get the thread name
        char tname[32];  
        prctl(PR_GET_NAME, tname); 
        pThreadLock->strThreadName = tname;

        if (!m_bStackOff) {
            // get call stack
			void *array[5];
			size_t size = backtrace (array, 3+m_iStackStep);

#if 0 	
			/*
			char **strings;
			size_t i;
			strings = backtrace_symbols (array, size);
			for (size_t il = 3;il<size;il++) {
				 strResult += std::string(strings[il]);
				 strResult += "\n";
            } 
            */
#else
		    for (size_t il = 3;il<size;il++) {
			    _snprintf(tname, 32-1, "%x", (unsigned int)(array[il]));    
			    strResult += tname;
			    strResult += "\n";
            }                                    
#endif    

        }

        // save
        g_bLockDoing = 4;
        LOCK_SequenceObj cSequenceObj;
        cSequenceObj.pAddress = pAddress;
        cSequenceObj.strSequenceName = strResult;
        pThreadLock->cCurrentSequence.arrayLocks.push_back(cSequenceObj);
        if ( pThreadLock->cCurrentSequence.arrayLocks.size() > 1) {
            pThreadLock->vecLockSequence.insert(pThreadLock->cCurrentSequence);
        }
        g_bLockDoing = 5;
    }

    g_bLockDoing = 0;
}

void
LOCK_Sequence::DoUnLock(void* pAddress, unsigned int  uiThreadID)
{
    char tname[128];  
    prctl(PR_GET_NAME, tname); 
    g_strThreadName = tname;

    g_bUnLockDoing = 1;
    int iThreadID = uiThreadID;
    if (iThreadID == 0) {
        iThreadID = pthread_self();
    }

    g_bUnLockDoing = 2;
    LOCK_TableThreadLocks::iterator it = m_cTableThreadLocks.find(iThreadID);
    if (it != m_cTableThreadLocks.end()) {
        LOCK_ThreadLock* pThreadLock = &(it->second);

        LOCK_AddrSequence::iterator itSequence = pThreadLock->cCurrentSequence.arrayLocks.begin();
        while (itSequence != pThreadLock->cCurrentSequence.arrayLocks.end()) {
            if (pAddress == itSequence->pAddress) {
                itSequence = pThreadLock->cCurrentSequence.arrayLocks.erase(itSequence);
                continue;
            }

            ++itSequence;
        }
    }

    g_bUnLockDoing = 0;
}

void
LOCK_Sequence::DoCheckAndOutputToFile(void)
{
    LEE_UtilityLog_Term("*********************************deadlock sequence will from here****************************** \n");
    LEE_UtilityLog_Term("*********************************deadlock sequence will from here****************************** \n");
    LEE_UtilityLog_Term("*********************************deadlock sequence will from here****************************** \n");
    LEE_UtilityLog_Term("*********************************deadlock sequence will from here****************************** \n");
    LEE_UtilityLog_Term("*********************************deadlock sequence will from here****************************** \n");
    LEE_UtilityLog_Term("*********************************deadlock sequence will from here****************************** \n");
    char       str[256];
    std::string   mess;
    memset(str, 0, sizeof(str));

    int iSum = 0;
    LOCK_VecLockSequences vecLockSequenceTemp;

    LOCK_TableThreadLocks::iterator itThread;
    for (itThread = m_cTableThreadLocks.begin(); itThread != m_cTableThreadLocks.end(); ++itThread) {
        LOCK_ThreadLock* pThreadLock = &(itThread->second);
        int indexLock = 0;
        
        LOCK_VecLockSequences::iterator itSequence;
        for (itSequence = pThreadLock->vecLockSequence.begin(); itSequence != pThreadLock->vecLockSequence.end(); ++itSequence) {
            for (size_t i = 0; i < itSequence->arrayLocks.size(); ++i) {
                for (size_t j = (i+1); j < itSequence->arrayLocks.size(); ++j) {
                    
                    // check have checked
                    bool bChecked = false;
                    for (LOCK_VecLockSequences::iterator itSequenceCheck = vecLockSequenceTemp.begin(); itSequenceCheck != vecLockSequenceTemp.end(); ++itSequenceCheck) {
                        if (itSequenceCheck->arrayLocks.size() == 2) {
                            if (itSequence->arrayLocks[i].pAddress == itSequenceCheck->arrayLocks[0].pAddress
                                && itSequence->arrayLocks[j].pAddress == itSequenceCheck->arrayLocks[1].pAddress
                                && itSequence->arrayLocks[i].strSequenceName == itSequenceCheck->arrayLocks[0].strSequenceName
                                && itSequence->arrayLocks[j].strSequenceName == itSequenceCheck->arrayLocks[1].strSequenceName) {

                                bChecked = true;
                                break;
                            }
                            else if (itSequence->arrayLocks[i].pAddress == itSequenceCheck->arrayLocks[1].pAddress
                                && itSequence->arrayLocks[j].pAddress == itSequenceCheck->arrayLocks[0].pAddress){
                                
                                bChecked = true;
                                break;
                            }
                        }
                    }
                    if (bChecked) {

                        break;
                    }

                    // action
                    LOCK_TableThreadLocks::iterator itThreadTemp;
                    for (itThreadTemp = m_cTableThreadLocks.begin(); itThreadTemp != m_cTableThreadLocks.end(); ++itThreadTemp) {
                        bool bFound = false;
                        int indexLockCheck = 0;

                        if (itThreadTemp != itThread) {
                            LOCK_ThreadLock* pThreadLockTemp = &(itThreadTemp->second);

                            LOCK_VecLockSequences::iterator itSequenceTemp;
                            for (itSequenceTemp = pThreadLockTemp->vecLockSequence.begin(); itSequenceTemp != pThreadLockTemp->vecLockSequence.end(); ++itSequenceTemp) {
                                for (size_t x = 0; x < itSequenceTemp->arrayLocks.size(); ++x) {
                                    if (itSequenceTemp->arrayLocks[x].pAddress == itSequence->arrayLocks[j].pAddress) {
                                        for (size_t y = (x+1); y < itSequenceTemp->arrayLocks.size(); ++y) {
                                            if (itSequenceTemp->arrayLocks[y].pAddress == itSequence->arrayLocks[i].pAddress
                                                && itSequence->arrayLocks[i].pAddress != itSequence->arrayLocks[j].pAddress) {

                                                LOCK_LockSequence sLOCK_LockSequence;
                                                sLOCK_LockSequence.arrayLocks.push_back(itSequence->arrayLocks[i]);
                                                sLOCK_LockSequence.arrayLocks.push_back(itSequence->arrayLocks[j]);
                                                size_t isizebefore = vecLockSequenceTemp.size();
                                                vecLockSequenceTemp.insert(sLOCK_LockSequence);
                                                size_t isizeafter = vecLockSequenceTemp.size();

                                                if (isizeafter != isizebefore) {
                                                    LEE_UtilityLog_Term("now the flag index is LLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLLL %d", iSum);
                                                    _snprintf(str, 256 - 1, "\n A->Lock->B: Thread %u %s; Sequence %u,%u;index %u", pThreadLock->uiThreadID, pThreadLock->strThreadName.c_str(), reinterpret_cast<int>(itSequence->arrayLocks[i].pAddress), reinterpret_cast<int>(itSequence->arrayLocks[j].pAddress), indexLock);
                                                    mess = std::string(str);
                                                    LEE_UtilityLog_Term(mess.c_str());
                                                    iSum++;

                                                    LEE_UtilityLog_Term("call stack \n %s", itSequence->arrayLocks[i].strSequenceName.c_str());
                                                    LEE_UtilityLog_Term("call stack \n %s", itSequence->arrayLocks[j].strSequenceName.c_str());

                                                    _snprintf(str, 256 - 1, "\n B->Lock->A: ThreadOp %u %s; Sequence %u,%u;index %u", pThreadLockTemp->uiThreadID, pThreadLockTemp->strThreadName.c_str(), reinterpret_cast<int>(itSequenceTemp->arrayLocks[x].pAddress), reinterpret_cast<int>(itSequenceTemp->arrayLocks[y].pAddress), indexLockCheck);
                                                    mess = std::string(str);
                                                    LEE_UtilityLog_Term(mess.c_str());

                                                    LEE_UtilityLog_Term("call stack \n %s", itSequenceTemp->arrayLocks[x].strSequenceName.c_str());
                                                    LEE_UtilityLog_Term("call stack \n %s", itSequenceTemp->arrayLocks[y].strSequenceName.c_str());

                                                }

                                                bFound = true;
                                                break;
                                            }
                                        }
                                    }

                                    if (bFound) {
                                        break;
                                    }
                                }

                                indexLockCheck++;

                                if (bFound) {
                                    break;
                                }
                            }
                        }
                    }    
                }
            }
            indexLock++;
        }
    }

    LEE_UtilityLog_Term("all the thread lock may be %d", iSum);
}

void
LOCK_Sequence::DoOutputToFile(void)
{
    //
    char       str[256];
    std::string   mess;
    memset(str, 0, sizeof(str));
    int iSum = 0;

    LOCK_TableThreadLocks::iterator itThread;
    for (itThread = m_cTableThreadLocks.begin(); itThread != m_cTableThreadLocks.end(); ++itThread) {
        LEE_UtilityLog_Term("Now the summary thread index --------------------------------------------------------- %d ", iSum);
        iSum++;
        LOCK_ThreadLock* pThreadLock = &(itThread->second);
        _snprintf(str, 256 - 1, "Thread ID %u Name %s\n", pThreadLock->uiThreadID, pThreadLock->strThreadName.c_str());
        mess = std::string(str);
        LEE_UtilityLog_Term(mess.c_str());
        
        int iSumSe = 0;
        LOCK_VecLockSequences::iterator itSequence;
        for (itSequence = pThreadLock->vecLockSequence.begin(); itSequence != pThreadLock->vecLockSequence.end(); ++itSequence) {
            for (size_t i = 0; i < itSequence->arrayLocks.size(); ++i) {
                _snprintf(str, 256 - 1, "%u \n %s ", reinterpret_cast<int>(itSequence->arrayLocks[i].pAddress), itSequence->arrayLocks[i].strSequenceName.c_str());
                mess = std::string(str);
                LEE_UtilityLog_Term(mess.c_str());
            }
            LEE_UtilityLog_Term("this sequence index is ---------------------------- %d", iSumSe);
            iSumSe++;
        }
        LEE_UtilityLog_Term("now the thread have sequence %d\n", iSumSe);
    }
}

/*---------------------------------------------------------------------------*/
// out put by va_list
VOID 
LOCK_Sequence::LEE_UtilityLog_TermOutTerm_V(const char *prefix, const char *message, va_list args)
{
    // init
    char       str[256];
    std::string   mess;

    // Initialize
    memset(str, 0, sizeof(str));

    // output
    _vsnprintf(str, 256 - 1, message, args);
    mess += std::string(str);
    mess += std::string("\n");

#if 0
    using namespace nutshell;
    NCSTARTPERF_KSTART;
    NCSTARTPERF_OUT(prefix, mess.GetString());
    NCSTARTPERF_END;
#else
    if (!m_cMapFile.IsOpen()) {
        //
        memset(str, 0, sizeof(str));
        _snprintf(str, 256 - 1, "%s/LockSequence/pidresult_%d.txt", g_chDirBase, getpid());
        std::string strPath(str);
        if (!m_cMapFile.OpenFile(strPath.c_str(), "a+"))
        {
            return;
        }
    }
        
    m_cMapFile.Write(mess.c_str(), mess.size(), 1);
#endif    
}

VOID 
LOCK_Sequence::LEE_UtilityLog_Term(const char *message, ...)
{
    va_list args;

    // var list do with start
    va_start(args, message);

    // out put
    LEE_UtilityLog_TermOutTerm_V("taohui", message, args);

    // var list do with over
    va_end(args);
}

/* EOF */
