#ifndef CXX_MAPTHREAD_H
#define CXX_MAPTHREAD_H

#ifdef WIN32
#include <process.h>
#else
#include <pthread.h>
#include <unistd.h>
#endif

class MapThread
{
public:
    MapThread(unsigned int   iThreadTimer = 0);
    virtual ~MapThread();
    void    Start();
    void    Stop();
    virtual void Execute() = 0;

private:

    void* RunSelf();
    static void* Run(void* pVoid);
    unsigned int        m_iThreadTimer;

#ifdef WIN32
    static  UINT WINAPI RunWin(LPVOID lpParam);
    HANDLE              m_hThread;
#endif
};

unsigned int    GetTheTickCount();
unsigned int    GetTheTickCountV();
#ifdef WIN32
unsigned int    GetThreadTime();
#endif

void            ThreadSleep(unsigned int uiCount);

#endif
