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


#ifndef CXX_LOCK_SYNCOBJ_H
#   include "LOCK_SyncObj.h"
#endif
#include <dlfcn.h>
#include <stdio.h>
#include <signal.h>

static int(*pthread_mutex_lock_ptr)(pthread_mutex_t *) = NULL;
static int(*pthread_mutex_unlock_ptr)(pthread_mutex_t *) = NULL;

LOCK_SyncObj::LOCK_SyncObj()
{
#if defined(_WIN32) || defined(_WIN32_WCE)
    ::InitializeCriticalSection(&cs);
#else
    pthread_mutex_lock_ptr = reinterpret_cast<int (*)(pthread_mutex_t*)>(dlsym(RTLD_NEXT, "pthread_mutex_lock"));
    pthread_mutex_unlock_ptr = reinterpret_cast<int (*)(pthread_mutex_t*)>(dlsym(RTLD_NEXT, "pthread_mutex_unlock"));
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&mutex, &attr);
#endif
}

LOCK_SyncObj::~LOCK_SyncObj()
{
#if defined(_WIN32) || defined(_WIN32_WCE)
    ::DeleteCriticalSection(&cs);
#else
    pthread_mutexattr_destroy(&attr);
    pthread_mutex_destroy(&mutex);
#endif
}

void
LOCK_SyncObj::SyncStart()
{
#if defined(_WIN32) || defined(_WIN32_WCE)
    ::EnterCriticalSection(&cs);
#else
    pthread_mutex_lock_ptr(&mutex);
#endif
}

bool
LOCK_SyncObj::TrySyncStart()
{
#if defined(_WIN32) || defined(_WIN32_WCE)
    BOOL blRet = ::TryEnterCriticalSection(&cs);
    return blRet?CL_TRUE:CL_FALSE;
#else
    int ret = pthread_mutex_trylock(&mutex);
    return (ret==0)?true:false;
#endif
}

void
LOCK_SyncObj::SyncEnd()
{
#if defined(_WIN32) || defined(_WIN32_WCE)
    ::LeaveCriticalSection(&cs);
#else
    pthread_mutex_unlock_ptr(&mutex);
#endif
}

/* EOF */
