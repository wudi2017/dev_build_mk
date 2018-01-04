#ifndef TESTCASE_H_
#define TESTCASE_H_

#include "baselib.h"

// test sample
void testcase_sample();

// multi thread test
void * thread_func_1(void * pv_param);
void testcase_multithread();

#endif