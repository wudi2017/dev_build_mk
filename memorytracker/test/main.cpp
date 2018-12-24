#include "stdio.h"
#include "string.h"
#include "pthread.h"
#include "unistd.h"

void * test_thread1(void * pv_param)
{
	printf("test_thread1 begin\n");

	for (int i = 0; i < 1000; ++i)
	{
		char* buf = new char[40];
		memset(buf, 0, 40);
		usleep(1000*1000*0.5);
	}


	return NULL;
}

void * test_thread2(void * pv_param)
{
	printf("test_thread2 begin\n");

	for (int i = 0; i < 1000; ++i)
	{
		char* buf = new char[100];
		memset(buf, 0, 101);
		usleep(1000*1000*1);
	}


	return NULL;
}

int main(int argc, char** argv)
{
	printf("testmain begin\n");


	// create monitor thread
	{
		pthread_t tid;
    	int ret = pthread_create(&tid,NULL,&test_thread1,NULL);
	}

	{
		pthread_t tid;
    	int ret = pthread_create(&tid,NULL,&test_thread2,NULL);
	}

	usleep(1000*1000*100);

	return 0;
}