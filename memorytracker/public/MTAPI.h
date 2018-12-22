/**
 * Copyright @ 2014 - 2017 Suntec Software(Shanghai) Co., Ltd.
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
 * @file MEM_MemTrackerLibApi.h
 * @brief dll export defines for memorytracker.
 *
 * This file defines the dll export macros for memorytracker.
 *
 * @attention None
 */
#ifndef CXX_MTAPI_H
#define CXX_MTAPI_H

#if defined(WIN32) || defined(WIN64)
#   if defined(libmemtracker_navi_EXPORTS) || defined(libmemtracker_core_navi_EXPORTS)
#       define MEMTRACKERLIB_API __declspec(dllexport)
#       define MEMTRACKERLIB_DATA __declspec(dllexport)
#   else
#       define MEMTRACKERLIB_API
#       define MEMTRACKERLIB_DATA
#   endif
#else // else of WIN32 || WIN64
#    if defined(__GNUC__) && __GNUC__ >= 4
#        define MEMTRACKERLIB_API  __attribute__ ((visibility("default")))
#        define MEMTRACKERLIB_DATA __attribute__ ((visibility("default")))
#    else
#        define MEMTRACKERLIB_API
#        define MEMTRACKERLIB_DATA
#    endif
#endif  // end of WIN32 || WIN64

#endif // CXX_MTAPI_H
/* EOF */
