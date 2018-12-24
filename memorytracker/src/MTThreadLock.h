
#ifndef MTTHREADLOCK_H
#define MTTHREADLOCK_H

#include <string.h>
#include <memory.h>
#include <list>
#include <ctime>
#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>

class MTThreadLock
{
public:
    MTThreadLock();
    void lock();
    void unlock();

    pthread_mutex_t m_mutex_lock;
};


class MTThreadAutoLock
{

public:
    MTThreadAutoLock(MTThreadLock& cLock);
    ~MTThreadAutoLock();
    
private:
    MTThreadAutoLock(const MTThreadLock& cLock);
    MTThreadAutoLock& operator=(const MTThreadAutoLock& cSync);
    MTThreadLock* m_pcLock;
};


#endif //MTTHREADLOCK_H