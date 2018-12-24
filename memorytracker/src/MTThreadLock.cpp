#include "MTThreadLock.h"

MTThreadLock::MTThreadLock()
{
    pthread_mutex_init(&m_mutex_lock, NULL);
}

void MTThreadLock::lock()
{
    pthread_mutex_lock(&m_mutex_lock);
}

void MTThreadLock::unlock()
{
    pthread_mutex_unlock(&m_mutex_lock);
}



MTThreadAutoLock::MTThreadAutoLock(MTThreadLock& cLock)
: m_pcLock(&cLock)
{
	if (m_pcLock)
	{
		m_pcLock->lock();
	}
}

MTThreadAutoLock::~MTThreadAutoLock()
{
	if (m_pcLock)
	{
		m_pcLock->unlock();
	}
}
