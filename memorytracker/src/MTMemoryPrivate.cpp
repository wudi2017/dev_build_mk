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
#include "MTMemoryPrivate.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define MAX_STRING_LENGTH 1024
static FILE* s_LogFile = NULL;
#define LOG_FILE_PATH "/tmp/memtracker.log"
#define MAX_FILE_SIZE (1024 * 1024 * 100)                 // file max length:100 MB
static long write_offset = 0;

void* MEM_malloc_private(unsigned int sz)
{
    if (0 == sz) {
        return NULL;
    }
    void* p = malloc(sz);
    return p;
}

void MEM_free_private(void* mem)
{
    if (mem) {
        free(mem);
    }
}

bool MEM_Log_private(bool bLog2File, const char* pszString, ...)
{
    char  szOutputString[(MAX_STRING_LENGTH+1)] = { 0 };
    int  iOutputLength = 0;
    va_list args;
    va_start(args, pszString);

    iOutputLength = vsnprintf(szOutputString, MAX_STRING_LENGTH, pszString, args);
    va_end(args);
    if (iOutputLength < 0) {
        return false;
    }
    if (bLog2File) {
        Log2File(szOutputString, iOutputLength);
        return true;
    }
    printf("%s", szOutputString);

    return true;
}

bool MEM_Log2Serial_private(const char* pszString, ...)
{
    char  szOutputString[(MAX_STRING_LENGTH+1)] = { 0 };
    int  iOutputLength = 0;
    va_list args;
    va_start(args, pszString);

    iOutputLength = vsnprintf(szOutputString, MAX_STRING_LENGTH, pszString, args);
    va_end(args);
    if (iOutputLength < 0) {
        return false;
    }
    int logFd = open("/dev/kmsg", O_WRONLY);
    if (-1 == logFd) {
        printf("MemoryTracker MEM_Log2Serial_private open /dev/kmsg failed\n");
        return false;
    }
    write(logFd, szOutputString, strlen(szOutputString));
    ::close(logFd);
    return true;
}

void Log2File(const char* pszString, const int& nLogLen)
{
    if (nLogLen <= 0) {
        printf("MemoryTracker Log2File invalid param\n");
        return;
    }
    if (NULL == s_LogFile) {
        s_LogFile = fopen(LOG_FILE_PATH, "w+b");
        write_offset = 0;
    }
    if (NULL == s_LogFile) {
        printf("MemoryTracker Log2File File:%s failed, fopen failed!\n", LOG_FILE_PATH);
        return;
    }
    if (write_offset + nLogLen > MAX_FILE_SIZE) {
        printf("MemoryTracker Log2File File:%s failed, file size overflow!\n", LOG_FILE_PATH);
        return;
    }
    fwrite(pszString, nLogLen, 1, s_LogFile);
    write_offset += nLogLen;

}

void FlushLogFile()
{
    if (NULL != s_LogFile) {
        fflush(s_LogFile);
    }
}

void CloseLogFile()
{
   if (NULL != s_LogFile) {
        fclose(s_LogFile);
        s_LogFile = NULL;
    }
}

/* EOF */
