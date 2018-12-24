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
 * @file MEM_MemoryAllocateConfig.h 
 * @brief MemoryAllocateConfig
 *
 * including definition and interface for mem tracker
 */
#ifndef MEMORYALLOCATECONFIG_H
#define MEMORYALLOCATECONFIG_H
#ifndef __cplusplus
#    error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <cstddef>

enum MemoryCategory
{
    MEMCATEGORY_UNKNOWN     = 0,
    MEMCATEGORY_BASE        = 1,
    MEMCATEGORY_NUM,
};

#endif  // MEMORYALLOCATECONFIG_H
/* EOF */
