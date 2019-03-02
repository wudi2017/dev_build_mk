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

int test() __attribute__ ((thread("threadA", "threadB", "threadC")));
int test()
{
	return 0;
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