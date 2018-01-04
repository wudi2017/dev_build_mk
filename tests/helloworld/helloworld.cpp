#include "stdio.h"

#include "addsub.h"
#include "mul.h"
#include "div.h"

int main() {
	printf("hello world\n");

	int x = 10;
	int y = 3;

	printf("call add %d %d = %d \n", x, y, libas_add(x,y));
	printf("call sub %d %d = %d \n", x, y, libas_sub(x,y));
	printf("call mul %d %d = %d \n", x, y, mul_ori(x, y));
	printf("call div %d %d = %d \n", x, y, div_ori(x, y));

	return 0;
}