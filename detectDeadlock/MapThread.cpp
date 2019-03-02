
#   include <time.h>
#   include <stdarg.h>
#ifndef CXX_MAPTHREAD_H
#include "MapThread.h"
#endif
#include <stdio.h>
#include <sys/time.h>

MapThread::MapThread(unsigned int   iThreadTimer)
:m_iThreadTimer(iThreadTimer)
#ifdef WIN32
,m_hThread(NULL)
#endif
{
    //
}

MapThread::~MapThread()
{
    //
    Stop();
}

void
MapThread::Start()
{
#ifdef WIN32
    m_hThread = (HANDLE)_beginthreadex(NULL, 0, RunWin, this, 0, NULL);
#else
    int res = 0;
     pthread_t  thread;
     res = pthread_create(&thread, NULL, Run, this);
#endif
}

void
MapThread::Stop()
{
#ifdef WIN32
    if (m_hThread != NULL){
		TerminateThread(m_hThread, 0);
		CloseHandle(m_hThread);
	}
#else
    pthread_exit(NULL);
#endif
}

void*
MapThread::RunSelf()
{ 
    //
    if(m_iThreadTimer == 0){
        Execute();
    }
    else{
        unsigned int uiTickCountLast = GetTheTickCount();
        unsigned int uiTickCountNow;
        unsigned int uiCountDis = 0;
        while(1){
            uiTickCountNow = GetTheTickCount();
            uiCountDis = uiTickCountNow - uiTickCountLast;
            if(uiCountDis>=m_iThreadTimer){
                Execute();
                uiTickCountLast = uiTickCountNow;
            }
            else{
                ThreadSleep((m_iThreadTimer - uiCountDis));
            }
        }
    }

    return this;
}

void*
MapThread::Run(void* pVoid)
{
    MapThread* pThread = (MapThread*)pVoid;
    pThread->RunSelf();
    return pThread;
}

#ifdef WIN32
UINT WINAPI
MapThread::RunWin(LPVOID lpParam){
	MapThread* pThread = static_cast<MapThread*>(lpParam);
    try{
        pThread->RunSelf();
    }
    catch(...){
    }

    return 0;
}
#endif

unsigned int    GetTheTickCountV()
{
    unsigned int uiGetTickCount;

#ifdef WIN32
    uiGetTickCount = GetTickCount();
#else
     timeval ts;
     gettimeofday(&ts,0);
     uiGetTickCount = (unsigned int)(ts.tv_sec * 10000 + (ts.tv_usec / 100));
#endif

    return uiGetTickCount;
}

unsigned int    GetTheTickCount()
{
    unsigned int uiGetTickCount;

#ifdef WIN32
    uiGetTickCount = GetTickCount();
#else
     timeval ts;
     gettimeofday(&ts,0);
     uiGetTickCount = (unsigned int)(ts.tv_sec * 1000 + (ts.tv_usec / 1000));
#endif

    return uiGetTickCount;
}

#ifdef WIN32
unsigned int
GetThreadTime()
{
	FILETIME ftCreationTime, ftExitTime, ftKernelTime, ftUserTime;
	GetThreadTimes(
		GetCurrentThread(),
		&ftCreationTime,
		&ftExitTime,
		&ftKernelTime,
		&ftUserTime);

	ULARGE_INTEGER uKernalTime;
	ULARGE_INTEGER uUserTime;
	memcpy(&uKernalTime, &ftKernelTime, sizeof(FILETIME));
	memcpy(&uUserTime, &ftUserTime, sizeof(FILETIME));

	ULONGLONG prefKernalTime = uKernalTime.QuadPart / 10000;
	ULONGLONG prefUserTime = uUserTime.QuadPart / 10000;

	return unsigned int (prefKernalTime + prefUserTime);
}
#endif

void
ThreadSleep(unsigned int uiCount)
{
#ifdef WIN32
    Sleep(uiCount);
#else
    usleep(uiCount);
#endif
}



