#include "testcase.h"
#include "baselib.h"
#include "string"
#include "vector"
using namespace std;

extern void demo_test();


#include <execinfo.h>
#define BACKTRACE_SIZE   16

unsigned long DGetTickCountMSX()
{
    struct timespec time;
    clock_gettime(CLOCK_MONOTONIC, &time);
    return time.tv_sec*1000 + time.tv_nsec/1000000;
}

void dump(void)
{
	printf("%p\n", dump);
	int j, nptrs;
	void *buffer[BACKTRACE_SIZE];
	char **strings;
	
	nptrs = backtrace(buffer, BACKTRACE_SIZE);
	
	printf("backtrace() returned %d addresses\n", nptrs);
  
	strings = backtrace_symbols(buffer, nptrs);
	if (strings == NULL) {
		perror("backtrace_symbols");
		exit(EXIT_FAILURE);
	}
 
	for (j = 0; j < nptrs; j++)
	{
		printf("  [%02d] %s\n", j, strings[j]);
	}
 
	free(strings);
}


void fff()
{
	unsigned long b = DGetTickCountMSX();
	for (int i = 0; i < 1; ++i)
	{
		dump();
	}
	unsigned long e = DGetTickCountMSX();
	printf("%d %d\n", b, e);
	
}
int main() {

		//fff();

	// demo_test();
	// return 0;

	// unsigned long dwSerial = 0x5f5e100;

	// std::string id;

 // 	char buf[64];
 //    memset(buf, 0, 64);
 //    snprintf(buf, 64, "281B%08d", dwSerial);
 //    id.assign(buf);


 //    printf("ID::: [%s]\n", id.c_str());

 //    return 0;

	// snprintf(buf, 64, "281B%08X", dwSerial);

	demo_test();
	return 0;

	{
		printf("-----------------------------------------------------------------------------\n");
		printf("X strlen %lu\n", strlen(__FILE__));
		printf("C %d\n", length(__FILE__));
	}
	


	{
		printf("-----------------------------------------------------------------------------\n");
		char buf[10] = {5,0,0,0,8,0,0,0,1,10};
		printf("CheckSum %d\n", CheckSum(buf,10));
	}

	{
		printf("-----------------------------------------------------------------------------\n");
		std::vector<std::string> plits = StringSplit("abcdefghij\nklmn876opq123456687409835abc\n\ndefghijklmnopq187623456687409835", "\n");
		for (int i = 0; i < plits.size(); i++)
		{
			printf("%s\n", plits.at(i).c_str());
		}
	}

	{
		printf("-----------------------------------------------------------------------------\n");
	 	const char * buf = "12345678901234561234567890123456123456789012345612345678901234567";
	 	PrintMemBuf(buf, strlen(buf));
	 }
	
	{
		printf("-----------------------------------------------------------------------------\n");
		for (int i = 0; i < 10; ++i)
		{
			testlog("dump log test string... i=%d", i);
		}
	}

	{
		printf("-----------------------------------------------------------------------------\n");
		printf("ps lock\n");
		int x = 0;
		proc_lock_init();
		proc_lock();
		x=10;
		proc_unlock();

	}
	 
	{
		printf("-----------------------------------------------------------------------------\n");
		proc_mem_init("test", 4);
		int * pSPInt = (int *) proc_mem_get("test");
		*pSPInt = 10;
		printf("proc_mem_get *pSPInt = %d\n", *pSPInt);
	}

	{
		printf("-----------------------------------------------------------------------------\n");
		psmap_set("key1","value1");
		psmap_set("key1","value11");
		psmap_set("key2","value2");

		
		printf("key1 v= %s\n", psmap_get("key1"));
		printf("key2 v= %s\n", psmap_get("key2"));
		printf("key3 v= %s\n", psmap_get("key3"));

		printf("key3 find= %d\n", psmap_find("key3"));
		printf("key1 find= %d\n", psmap_find("key1"));
	}


	return 0;
}