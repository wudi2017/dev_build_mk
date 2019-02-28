#include "hookapi.h"
#include "stdio.h"
#include <dlfcn.h>

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>



extern "C"
{
	void test()
	{
		// show
		struct dirent * filename;

		int pid = getpid();
		printf("pid:%d\n", pid);
		DIR * dir;                   // return value for opendir()
		char strdir[256];
		snprintf(strdir, 256, "/proc/%d/fd/", pid);
		dir = opendir(strdir);
		if( NULL == dir )
		{
			printf("Can not open dir \n");
			return;
		}
		while( ( filename = readdir(dir) ) != NULL )
		{
			// get rid of "." and ".."
			if( strcmp( filename->d_name , "." ) == 0 || 
				strcmp( filename->d_name , "..") == 0    )
				continue;
			//printf("%s\n", filename ->d_name);
			char filefullname[256];
			snprintf(filefullname, 256, "%s%s",strdir, filename ->d_name);
			char realpath[256];
			readlink(filefullname, realpath, 256);
			printf("%s -> %s\n", filefullname, realpath);
		}

	}
	
	void exit (int __status) __THROW 
	{
		printf("my exit\n");

		void (*pf_exit)(int status);
		pf_exit = reinterpret_cast<void (*)(int)>(dlsym(RTLD_NEXT, "exit"));
		printf("pf_exit:%p\n", pf_exit);

		pf_exit(__status);
	}
	void _exit (int __status) 
	{
		printf("my _exit\n");
		void (*pf__exit)(int status);
		pf__exit = reinterpret_cast<void (*)(int)>(dlsym(RTLD_NEXT, "_exit"));
		printf("pf_exit:%p\n", pf__exit);

		test();

		pf__exit(__status);
	}
	void _Exit (int __status) __THROW 
	{
		printf("my _Exit\n");
		void (*pf__Exit)(int status);
		pf__Exit = reinterpret_cast<void (*)(int)>(dlsym(RTLD_NEXT, "_Exit"));
		printf("pf_Exit:%p\n", pf__Exit);

		pf__Exit(__status);
	}
	void pthread_exit (void *__retval)
	{
		printf("my pthread_exit\n");
		void (*pf_pthread_exit)(void *__retval);
		pf_pthread_exit = reinterpret_cast<void (*)(void *)>(dlsym(RTLD_NEXT, "pthread_exit"));

		pf_pthread_exit(__retval);
	}
	// int strcmp(const char*, const char*) throw ()
	// {
	// 	printf("my strcmp\n");
	// 	return 0;
	// }
}

void fnExit()
{
	printf("fnExit...\n");
	test();
}

class hookapiStatus
{
public:
	hookapiStatus()
	{
		printf("hookapiStatus...\n");
		atexit (fnExit);
	}
	~hookapiStatus()
	{

	}
	
};

static hookapiStatus sHo;