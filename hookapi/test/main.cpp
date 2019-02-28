#include "stdio.h"
#include "stdlib.h"
#include <unistd.h>
#include <dlfcn.h>
#include "string.h"

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>


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
			printf("%s\n", filename ->d_name);
			char filefullname[256];
			snprintf(filefullname, 256, "%s%s",strdir, filename ->d_name);
			char realpath[256];
			readlink(filefullname, realpath, 256);
			printf("%s\n", realpath);
		}

	}

int main(int argc, char** argv)
{
	printf("test hook api begin\n");


	//test();

	// void (*pf_exit)(int status);
	// pf_exit = reinterpret_cast<void (*)(int)>(dlsym(RTLD_NEXT, "exit"));
	// printf("pf_exit:%p\n", pf_exit);

	// printf("call strcmp\n");
	// strcmp(NULL,NULL);

	//printf("call exit\n");
	//exit(0);

	//printf("call _exit\n");
	//_exit(0);

	printf("test hook api end\n");
	return 0;
}