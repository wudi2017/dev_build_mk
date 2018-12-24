#include "MTMonitor.h"
#include "MTLog.h"
#include "MTRecorder.h"
#include "MTProtectMemoryAllocator.h"

extern MTRecorder s_MTRecorder;

MTMonitor::MTMonitor()
:b_startFlag(false)
{

}

MTMonitor::~MTMonitor()
{

}


void * mt_monitor_thraed(void * pv_param)
{
	MTLOG_I("mt_monitor_thraed begin");

	while(1)
	{
		//MTLOG_I("mt_monitor_thraed task run");

		std::string status_protectmem = dump_status_protectmem();

		std::string status_alloc = s_MTRecorder.ReportStatus();

		std::string status_overflow = s_MTRecorder.dumpMemoryOverFlow();

        // write to file 
        char monitorFileName[256];
        memset(monitorFileName, 0, 256);
        snprintf(monitorFileName, 256, "/tmp/memorytracker_%d_monitor", getpid());
        FILE *  fp;
        fp=fopen(monitorFileName,"w");
        if (NULL != fp)
        {
            fwrite(status_protectmem.c_str(),status_protectmem.length(),1,fp);
            fwrite(status_alloc.c_str(),status_alloc.length(),1,fp);
            fwrite(status_overflow.c_str(),status_overflow.length(),1,fp);
            fclose(fp);
        }

       

		usleep(1000*1000*3);
	}

	return NULL;
}



void MTMonitor::start()
{
	if (!b_startFlag)
	{
		pthread_t tid;
    	int ret = pthread_create(&tid,NULL,&mt_monitor_thraed,NULL);

		b_startFlag = true;
	}
}