#include "addsub.h"

#include "add.h"
#include "sub.h"

int libas_add(int x, int y)
{
	return add_ori(x, y);
}

int libas_sub(int x, int y)
{
	return sub_ori(x, y);
}