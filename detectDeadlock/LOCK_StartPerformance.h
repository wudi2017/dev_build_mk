/**
 * Copyright @ 2013 - 2014 Suntec Software(Shanghai) Co., Ltd.
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
 * @file NCStartPerformance.h
 * @brief Declaration file of class NCStartPerformance.
 * @attention used for C++ only.
 */

#ifndef NCSTARTPERFORMANCE_H
#define NCSTARTPERFORMANCE_H
#ifndef __cplusplus
#   error ERROR: This file requires C++ compilation (use a .cpp suffix)
#endif

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <cutils/log.h>
#include <ncore/NCTimeDefine.h>
#include <ncore/NCLog.h>



namespace LOCKSEQUENCE
{

unsigned int GetTickCount()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec*1000 + time.tv_nsec/1000000;
}

unsigned int GetThreadTime()
{
    struct timespec time;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
    return time.tv_sec*1000 + time.tv_nsec/1000000;

}
    /**
     * NCStartPerformance
     *
     * I/F for recored start performance log (radio), with kmsg output function.
     */
    class NCCORE_API NCStartPerformance
    {
    public:
        static const uint32_t LOG_BUFF_LEN = 128;
        NCStartPerformance(bool kmg = false)
        : m_fd(0)
        , m_kmg(kmg)
        {
            if (m_kmg) {
                m_fd = ::open("/dev/kmsg", O_WRONLY);
            }
            memset(m_msg, 0, LOG_BUFF_LEN);
        }

        ~NCStartPerformance()
        {
            if (m_fd) {
                ::close(m_fd);
            }
            m_fd = 0;
        }

        void write(const char* module, const char* szlog)
        {
            if (module && szlog) {
                if (m_fd) {
                    ::snprintf(m_msg, LOG_BUFF_LEN, "<5>[%u]%s %s\n", GetThreadTime(), module, szlog);
                    ::write(m_fd, m_msg, strlen(m_msg)+1);
                }
                else {
                    ::snprintf(m_msg, LOG_BUFF_LEN, "[TC:%u][TT:%u][%s]%s", GetTickCount(), GetThreadTime(), module, szlog);
                    __android_log_buf_write(LOG_ID_RADIO, ANDROID_LOG_INFO, "start", m_msg);
                }
            }
        }

        void writex(const char* module, const char* szlog, int type)
        {
            if (module && szlog) {
                if (m_fd) {
                    ::snprintf(m_msg, LOG_BUFF_LEN, "<%d>[%u]%s %s\n", type, GetThreadTime(), module, szlog);
                    ::write(m_fd, m_msg, strlen(m_msg)+1);
                }
                else {
                    ::snprintf(m_msg, LOG_BUFF_LEN, "[TC:%u][TT:%u][%s]%s", GetTickCount(), GetThreadTime(), module, szlog);
                    __android_log_buf_write(LOG_ID_RADIO, ANDROID_LOG_INFO, "start", m_msg);
                }
            }
        }

        void write(const char* module, const char* szlog, int param, int type = 5)
        {
            if (module && szlog) {
                if (m_fd) {
                    ::snprintf(m_msg, LOG_BUFF_LEN, "<%d>[%u]%s %s[%d]\n", type, GetThreadTime(), module, szlog, param);
                    ::write(m_fd, m_msg, strlen(m_msg)+1);
                }
                else {
                    ::snprintf(m_msg, LOG_BUFF_LEN, "[TC:%u][TT:%u][%s]%s[%d]", GetTickCount(), GetThreadTime(), module, szlog, param);
                    __android_log_buf_write(LOG_ID_RADIO, ANDROID_LOG_INFO, "start", m_msg);
                }
            }
        }

        void write(const char* module, const char* szlog, const char* param, int type = 5)
        {
            if (module && szlog) {
                if (m_fd) {
                    ::snprintf(m_msg, LOG_BUFF_LEN, "<%d>[%u]%s %s[%s]\n", type, GetThreadTime(), module, szlog, param);
                    ::write(m_fd, m_msg, strlen(m_msg)+1);
                }
                else {
                    ::snprintf(m_msg, LOG_BUFF_LEN, "[TC:%u][TT:%u][%s]%s[%s]", GetTickCount(), GetThreadTime(), module, szlog, param);
                    __android_log_buf_write(LOG_ID_RADIO, ANDROID_LOG_INFO, "start", m_msg);
                }
            }
        }

        void close()
        {
            if (m_fd) {
                ::close(m_fd);
            }
            m_fd = 0;
        }
    private:
        int m_fd;
        bool m_kmg;
        char m_msg[LOG_BUFF_LEN];
    };

#define NCSTARTPERF_START NCStartPerformance __perfStart
#define NCSTARTPERF_KSTART NCStartPerformance __perfStart(true)
#define NCSTARTPERF_OUT(module, log) __perfStart.writex(module, log, 5)
#define NCSTARTWARN_OUT(module, log) __perfStart.writex(module, log, 4)
#define NCSTARTERR_OUT(module, log) __perfStart.writex(module, log, 3)
#define NCSTARTPERF_OUT_1N(module, log, param) __perfStart.write(module, log, param, 5)
#define NCSTARTPERF_OUT_1S(module, log, param) __perfStart.write(module, log, param, 5)
#define NCSTARTWARN_OUT_1N(module, log, param) __perfStart.write(module, log, param, 4)
#define NCSTARTWARN_OUT_1S(module, log, param) __perfStart.write(module, log, param, 4)
#define NCSTARTERR_OUT_1N(module, log, param) __perfStart.write(module, log, param, 3)
#define NCSTARTERR_OUT_1S(module, log, param) __perfStart.write(module, log, param, 3)
#define NCSTARTPERF_END __perfStart.close()

}

#endif //NCSTARTPERFORMANCE_H
/*EOF*/
