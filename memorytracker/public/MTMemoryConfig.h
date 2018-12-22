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
    MEMCATEGORY_MAP         = 2,
    MEMCATEGORY_SEARCH      = 3,
    MEMCATEGORY_Location    = 4,
    MEMCATEGORY_PATH        = 5,
    MEMCATEGORY_Traffic     = 6,
    MEMCATEGORY_Guide       = 7,
    MEMCATEGORY_NaviMain    = 8,
    MEMCATEGORY_Voice       = 9,
    MEMCATEGORY_RegPoint    = 10,
    MEMCATEGORY_PhoneLink   = 11,
    MEMCATEGORY_DATAENGINE  = 12,
    MEMCATEGORY_NETPROXY    = 13,
    MEMCATEGORY_Highway     = 14,
    MEMCATEGORY_Effect      = 15,
    MEMCATEGORY_AvcLan      = 16,
    MEMCATEGORY_Provider    = 17,
    MEMCATEGORY_UIFWBASE    = 18,
    MEMCATEGORY_UIFWCEGUI   = 19,
    MEMCATEGORY_UIFWCC      = 20,
    MEMCATEGORY_UIFWMC      = 21,
    MEMCATEGORY_UIFWMM      = 22,
    MEMCATEGORY_UIFWPFW     = 23,
    MEMCATEGORY_UIFWRTL     = 24,
    MEMCATEGORY_NAVIUI      = 25,
    MEMCATEGORY_Luya        = 26,
    MEMCATEGORY_Phoenix     = 27,
    MEMCATEGORY_SIGNAL      = 28,
    MEMCATEGORY_Model       = 29,
    MEMCATEGORY_UIFWBC      = 30,
    MEMCATEGORY_ANIMATION   = 31,
    MEMCATEGORY_RESOURCE    = 32,
    MEMCATEGORY_UIFWONS     = 33,
    MEMCATEGORY_MODELNAVI   = 34,
    MEMCATEGORY_UIAAP       = 35,
    MEMCATEGORY_UIAV        = 36,
    MEMCATEGORY_UIBTPHONE   = 37,
    MEMCATEGORY_UICALLING   = 38,
    MEMCATEGORY_UICAMERA    = 39,
    MEMCATEGORY_UICARPLAY   = 40,
    MEMCATEGORY_UICOMPASS   = 41,
    MEMCATEGORY_UIDIAG      = 42,
    MEMCATEGORY_UIHOME      = 43,
    MEMCATEGORY_UIINCOMING  = 44,
    MEMCATEGORY_UIMIRRORLINK    = 45,
    MEMCATEGORY_UISETTING       = 46,
    MEMCATEGORY_UISYSTEMSERVICE = 47,
    MEMCATEGORY_UISYSTEMUI      = 48,
    MEMCATEGORY_UISYSTEMUPDATE  = 49,
    MEMCATEGORY_UIVR            = 50,
    MEMCATEGORY_BASEFC          = 51,
    MEMCATEGORY_MAPFC           = 52,
    MEMCATEGORY_SEARCHFC        = 53,
    MEMCATEGORY_LocationFC      = 54,
    MEMCATEGORY_PATHFC          = 55,
    MEMCATEGORY_TrafficFC       = 56,
    MEMCATEGORY_GuideFC         = 57,
    MEMCATEGORY_VoiceFC         = 58,
    MEMCATEGORY_RegPointFC      = 59,
    MEMCATEGORY_EffectFC        = 60,
    MEMCATEGORY_NUM,
};

#endif  // MEMORYALLOCATECONFIG_H
/* EOF */
