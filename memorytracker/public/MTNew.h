/**
 * Copyright @ 2013 - 2015 Suntec Software(Shanghai) Co., Ltd.
 * All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are NOT permitted except as agreed by
 * Suntec Software(Shanghai) Co., Ltd.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 */
/**
 * @file MEM_NEW.h
 * @brief MEM_NEW
 *
 * including definition and interface for mem tracker
 */
#ifndef MTNEW_H
#define MTNEW_H

#ifndef __cplusplus
    // .c file include , define nothing
#else

#include "MTAPI.h"
#include <new>
#include <stddef.h>
#include "MTConfig.h"

#ifdef new
#  undef new
#endif
#ifdef delete
#  undef delete
#endif

MEMTRACKERLIB_API void* operator new(std::size_t sz);
// MEMTRACKERLIB_API void* operator new(std::size_t sz)  throw(std::bad_alloc);

MEMTRACKERLIB_API void* operator new[](std::size_t sz);
// MEMTRACKERLIB_API void* operator new[](std::size_t sz)  throw(std::bad_alloc);

MEMTRACKERLIB_API void operator delete(void* ptr)  throw();

MEMTRACKERLIB_API void operator delete[] (void* ptr)  throw();

#ifdef HAVE_WRAP_LINK_OPTION
extern "C"{
    MEMTRACKERLIB_API void* __real_malloc (std::size_t sz);
    MEMTRACKERLIB_API void* __wrap_malloc (std::size_t sz);
    // MEMTRACKERLIB_API void __wrap_free(void *ptr);
    // MEMTRACKERLIB_API void __real_free(void *ptr);
    // MEMTRACKERLIB_API void* __wrap_calloc(std::size_t n, std::size_t sz);
    // MEMTRACKERLIB_API void* __real_calloc(std::size_t n, std::size_t sz);
    // MEMTRACKERLIB_API void* __wrap_realloc(void* ptr, std::size_t sz);
    // MEMTRACKERLIB_API void* __real_realloc(void* ptr, std::size_t sz);
    // MEMTRACKERLIB_API void* __wrap_memcpy(void* dst, void* src, std::size_t sz);
    // MEMTRACKERLIB_API void* __real_memcpy(void* dst, void* src, std::size_t sz);
    // MEMTRACKERLIB_API void* __wrap_memmove(void* dst, void* src, std::size_t sz);
    // MEMTRACKERLIB_API void* __real_memmove(void* dst, void* src, std::size_t sz);
    // MEMTRACKERLIB_API void* __wrap_memset(void* ptr, int value, std::size_t sz);
    // MEMTRACKERLIB_API void* __real_memset(void* ptr, int value, std::size_t sz);

}
#endif // HAVE_WRAP_LINK_OPTION

#endif // __cplusplus
#endif // MTNEW_H
/* EOF */
