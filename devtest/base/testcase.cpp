#include "testcase.h"
#include "baselib.h"
#include "stdio.h"
#include <pthread.h> // thread
#include <unistd.h>  // sleep(s) usleep(us)

BnlQList<int> cList(0);
int addcnt = 0 ;
int getcnt = 0 ;
int getfailedcnt = 0 ;

void * thread_func_1(void * pv_param)
{
	for (int i = 0; i < 1000; ++i)
	{
		cList.AddToTail(i);
		//printf("AddToTail data=%d\n",i);
		usleep(10);
		addcnt ++;
	}
	printf("thread_func_1 exit\n");
}
void * thread_func_2(void * pv_param)
{
	while(true)
	{
		int data = -1;
		bool bGet = cList.GetFromHead(data);
		if (bGet)
		{
			//printf("GetFromHead ______ data=%d\n",data);
			usleep(8);
			getcnt++;
		}
		else
		{
			getfailedcnt++;
		}
	}
}
void testcase_multithread()
{
	pthread_t tid;
	int i,ret;
	for(int tcnt=0; tcnt<10; tcnt++)
	{
		ret = pthread_create(&tid,NULL,&thread_func_1,(void*)tcnt);
		if(ret!=0){
			printf ("Create pthread error!\n");
			break;
		}
		//pthread_join(tid,NULL); // wait thread exit
	}
	for(int tcnt=0; tcnt<1; tcnt++)
	{
		ret = pthread_create(&tid,NULL,&thread_func_2,(void*)tcnt);
		if(ret!=0){
			printf ("Create pthread error!\n");
			break;
		}
		//pthread_join(tid,NULL); // wait thread exit
	}

	sleep(10);

	printf("This is the main thread process exit begin.\n");

	printf("all add cnt = %d\n", addcnt);
	printf("all get cnt = %d\n", getcnt);
	printf("all getfailedcnt = %d\n", getfailedcnt);
	int data=-1;
	while(cList.GetFromHead(data))
	{
		printf("Last check: GetFromHead data=%d\n",data);
		usleep(20*1000);
	}

	printf("This is the main thread process exit end.\n");
}



class CA
{
public:
	CA()
	{
		m_data=0;
	}
	int increase(int x)
	{
		m_data += x;
		return m_data;
	}

	int m_data;
};

typedef int (CA::*fp_increase)(int x);

typedef int (*fp_increase_c)(CA* o, int x);

void testcase_cpp_obj_func_call()
{
	CA a;

	printf("call origin = %d\n", a.increase(1));
	printf("call origin = %d\n", a.increase(2));


	CA * pa = &a;
	fp_increase pfaddr = &CA::increase;
	int itest2 = (pa->*pfaddr)(2);
	printf("call C++ class::func_addr = %d\n", itest2);
	//  ((m_cReplyInnerAddr.m_pThis)->*(m_cReplyInnerAddr.m_pfc))(evcontext, pMsg);


	long* funcAddr = (long*)(&pfaddr);
	fp_increase_c pfaddrcc = (fp_increase_c)(*funcAddr);
	int itest3 = pfaddrcc(pa, 10);
	printf("call C style func_addr = %d\n", itest3);
}

class CB1
{
public:
	CB1()
	{
		b1=1;
	}
	void printCB1()
	{
		printf("CB1::printCB1 %p [%d]\n", this, b1);
	}
	int b1;
};

class CB2
{
public:
	CB2()
	{
		b2=2;
	}
	void printCB2()
	{
		printf("CB2::printCB2 %p [%d]\n", this, b2);
	}
	int b2;
};

class D: public CB1, public CB2
{
public:
	D()
	{
		d=3;
	}
	void printD()
	{
		printf("D::printD %p [%d]\n", this, d);
	}
	int d;
};

typedef void (*fp_printCB1)(CB1* o);
typedef void (*fp_printCB2)(CB2* o);

void testcase_cpp_obj_func_call2()
{
	D d;

	d.printCB1();
	d.printCB2();

	{
		void* pf = (void*)&CB1::printCB1;
		printf("pfaddrpcb1=%p\n", pf);
		fp_printCB1 pfaddrpcb1 = (fp_printCB1)pf;
		pfaddrpcb1(&d);
	}
	{
		void* pf = (void*)&CB2::printCB2;
		printf("pfaddrpcb2=%p\n", pf);
		fp_printCB2 pfaddrpcb2 = (fp_printCB2)pf;
		pfaddrpcb2(&d);
	}
}