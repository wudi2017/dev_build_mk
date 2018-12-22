#include "MTThreadLock.h"

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
