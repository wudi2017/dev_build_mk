#include "MTLog.h"
#include "stdarg.h"
#include <time.h>
#include <unistd.h>
#include <sys/syscall.h>

#define PRINTFLOG_ENABLE

pid_t DGettid()
{
    return syscall(__NR_gettid);
}

unsigned long DGetTickCountMS()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec*1000 + time.tv_nsec/1000000;
}
unsigned long DGetThreadTimeUS()
{
    struct timespec time;
    clock_gettime(CLOCK_THREAD_CPUTIME_ID, &time);
    return time.tv_sec*1000*1000 + time.tv_nsec/1000;
}

void MTLOG_I(const char* Fmt, ...)
{
    va_list ap;
    char TempStr[512];
    va_start(ap,Fmt);
    vsprintf(TempStr,Fmt,ap);
    va_end(ap);
    char TempStrOut[512];
    unsigned long TT = DGetThreadTimeUS();
    snprintf(TempStrOut, 512, "[%d %d][%ldms %lu.%03lums] %s", ::getpid(), ::DGettid(), 
        DGetTickCountMS(), TT/1000, TT%1000, TempStr);
    /************************************************************************/

#ifdef FILELOG_ENABLE
    // write to file 
    FILE *  fp;
    fp=fopen("/tmp/testlog.txt","a+");
    if (NULL != fp)
    {
    	char TempStrOutLn[512];
    	strcpy(TempStrOutLn, TempStrOut);
    	strcat(TempStrOutLn, "\n");
    	fwrite(TempStrOutLn,strlen(TempStrOutLn),1,fp);
    	fclose(fp);
    }
#endif
    
#ifdef PRINTFLOG_ENABLE
    // print to consoler
    printf("%s\n", TempStrOut);
#endif

#ifdef ANDROIDLOG_ENABLE
    // android log
    ANDROIDLOG("%s", TempStrOut);
#endif
}
