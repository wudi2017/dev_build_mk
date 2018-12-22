#include "stdio.h"
#include "string.h"

int main(int argc, char** argv)
{
	printf("testmain begin\n");

	char* buf = new char[40];
	memset(buf, 0, 41);
	delete[] buf;

	return 0;
}