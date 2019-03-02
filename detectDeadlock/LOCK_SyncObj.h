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
#define CXX_LOCK_SYNCOBJ_H
#ifndef __cplusplus
#   error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#if !defined(_WIN32) && !defined(_WIN32_WCE)
#   include <pthread.h>
#endif //_LINUX

/**
 *  class LOCK_SyncObj
 *
 * The class of Synchronizing object
 *
 */
class LOCK_SyncObj
{
public:
#if defined(_WIN32) || defined(_WIN32_WCE)
    CRITICAL_SECTION cs;
#else
    pthread_mutex_t mutex;
    pthread_mutexattr_t attr;
#endif //_LINUX

private:
    LOCK_SyncObj(const LOCK_SyncObj& src){}
    LOCK_SyncObj& operator = (const LOCK_SyncObj& src){return *this;}

public:
    /**
    * Construction.
    */
    LOCK_SyncObj();

    /**
    * Destruction.
    */
    virtual ~LOCK_SyncObj();

    /**
    * Synchronize start.
    */
    void SyncStart();

    /**
    * Try synchronize start
    *
    * @return CL_BOOL : CL_TRUE means synchronize succeed, and CL_FALSE failed.
    */
    bool TrySyncStart();

    /**
    * Synchronize end.
    */
    void SyncEnd();
};

#endif // CXX_LOCK_SYNCOBJ_H
/* EOF */
