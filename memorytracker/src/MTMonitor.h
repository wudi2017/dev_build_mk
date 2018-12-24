#include "stdio.h"
#include "string.h"
#include "pthread.h"
#include "unistd.h"

class MTMonitor
{
public:
	MTMonitor();
	~MTMonitor();

	void start();

private:
	bool b_startFlag;
};