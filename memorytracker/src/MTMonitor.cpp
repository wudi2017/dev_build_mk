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
		MTLOG_I("mt_monitor_thraed task run");

		run_task_save_protectblock_status();

		std::string status = s_MTRecorder.ReportStatus();
		//printf("%s\n", status.c_str());

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