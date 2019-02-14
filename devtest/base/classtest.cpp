#include "stdio.h"
#include "stdint.h"
extern void PrintMemBuf(const void * buf, const int size);

class B
{
public:
	B(){b=1;}
	virtual void f(){}
	int b;
};

class X : public B
{
public:
	X(){x=2;}
	virtual void f(){}
	int x;
};

class Y : public B
{
public:
	Y(){y=3;}
	virtual void f(){}
	int y;
};

class M : public X, public Y
{
public:
	M(){m=4;}
	virtual void f(){}
	int m;
};

class VX : virtual public B
{
public:
	VX(){x=2;}
	virtual void f(){}
	int x;
};

class VY : virtual public B
{
public:
	VY(){y=3;}
	virtual void f(){}
	int y;
};

class VM : virtual public X, virtual public Y
{
public:
	VM(){m=4;}
	virtual void f(){}
	int m;
};

void classtest_inherit()
{
	printf("classtest_inherit ####################################\n");

	B b;
	B b1;

	X x;

	Y y;

	M m;

	{
		printf("object b\n");
		printf("addr=%p size=%d\n", &b, sizeof(b)); // 8 4 4
		PrintMemBuf(&b, 16);
		printf("vtable:\n");
		intptr_t p = *(intptr_t*)&b;
		PrintMemBuf((void*)p, 8);
		printf("\n");
	}
	{
		printf("object b1\n");
		printf("addr=%p size=%d\n", &b1, sizeof(b1)); // 8 4 4
		PrintMemBuf(&b1, 16);
		printf("vtable:\n");
		intptr_t p = *(intptr_t*)&b1;
		PrintMemBuf((void*)p, 8);
		printf("\n");
	}
	
	{
		printf("object x\n");
		printf("addr=%p size=%d \n", &x, sizeof(x)); // 8 4 4
		PrintMemBuf(&x, 16);
		printf("vtable:\n");
		intptr_t p = *(intptr_t*)&x;
		PrintMemBuf((void*)p, 8);
		printf("\n");
	}
	
	{
		printf("object y \n");
		printf("addr=%p size=%d \n", &y, sizeof(y)); // 8 4 4
		PrintMemBuf(&y, 16);
		printf("vtable:\n");
		intptr_t p = *(intptr_t*)&y;
		PrintMemBuf((void*)p, 8);
		printf("\n");
	}

	{
		printf("object m \n");
		printf("addr=%p size=%d \n", &m, sizeof(m)); // 8 4 4 | 8 4 4 | 4 4
		PrintMemBuf(&m, 40);
		printf("vtable:\n");
		intptr_t p = *(intptr_t*)&m;
		PrintMemBuf((void*)p, 8);
		intptr_t p2 = *(intptr_t*)((char*)&m + 16);
		PrintMemBuf((void*)p2, 8);
		printf("\n");
	}

}

void classtest_virtual_inherit()
{
	printf("classtest_virtual_inherit ####################################\\n");
	B b;
	B b1;
	VX vx;
	VY vy;
	VM vm;

	{
		printf("object b\n");
		printf("addr=%p size=%d\n", &b, sizeof(b)); // 8 4 4
		PrintMemBuf(&b, 16);
		printf("vtable:\n");
		intptr_t p = *(intptr_t*)&b;
		PrintMemBuf((void*)p, 8);
		printf("\n");
	}
	{
		printf("object b1\n");
		printf("addr=%p size=%d\n", &b1, sizeof(b1)); // 8 4 4
		PrintMemBuf(&b1, 16);
		printf("vtable:\n");
		intptr_t p = *(intptr_t*)&b1;
		PrintMemBuf((void*)p, 8);
		printf("\n");
	}
	
	{
		printf("object vx\n");
		printf("addr=%p size=%d \n", &vx, sizeof(vx)); // 8 4 4 8 4 4
		PrintMemBuf(&vx, 32);
		printf("vtable:\n");
		intptr_t p = *(intptr_t*)&vx;
		PrintMemBuf((void*)p, 8);
		intptr_t p2 = *(intptr_t*)((char*)&vx + 16);
		PrintMemBuf((void*)p2, 8);
		printf("\n");
	}
	
	{
		printf("object vy \n");
		printf("addr=%p size=%d \n", &vy, sizeof(vy)); // 8 4 4 8 4 4
		PrintMemBuf(&vy, 32);
		printf("vtable:\n");
		intptr_t p = *(intptr_t*)&vy;
		PrintMemBuf((void*)p, 8);
		intptr_t p2 = *(intptr_t*)((char*)&vy + 16);
		PrintMemBuf((void*)p2, 8);
		printf("\n");
	}

	{
		printf("object vm \n");
		printf("addr=%p size=%d \n", &vm, sizeof(vm)); // 8 4 4 | 8 4 4 | 4 4
		PrintMemBuf(&vm, 48);
		printf("vtable:\n");
		intptr_t p = *(intptr_t*)&vm;
		PrintMemBuf((void*)p, 8);
		printf("\n");
	}
}

void classtest()
{
	classtest_inherit();
	classtest_virtual_inherit();
}