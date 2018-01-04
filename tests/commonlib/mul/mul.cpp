#include "mul.h"

extern "C"
{
__attribute__((visibility("default"))) int mul_ori(int x, int y)
{
	return x*y;
}
}