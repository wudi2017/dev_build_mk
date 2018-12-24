#ifndef MTPROTECTMEM_H
#define MTPROTECTMEM_H

#include <string.h>
#include <memory.h>
#include <list>
#include <ctime>
#include <map>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>


void* protect_tail_new(const int size);
bool protect_tail_try_delete(void* addr);
void run_task_save_protectblock_status();

#endif