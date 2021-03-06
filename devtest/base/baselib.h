#ifndef BASELIB_H_
#define BASELIB_H_


//////////////////////////////////////////////////////////////////////////////////////////
// constexpr length

int constexpr length(const char* str)
{
    return *str ? 1 + length(str + 1) : 0;
}


//////////////////////////////////////////////////////////////////////////////////////////
// nolock list

#include "stdio.h"
#include <pthread.h> // thread
#include <unistd.h>  // sleep(s) usleep(us)
#include <string.h>  // strlen
#include <string>  // std string
#include <vector>  // std vector

// base No lock queue list
// multi thread add: single thread get
template <typename T>
struct BnlNode // base no lock queue list node
{
   BnlNode(T Data)
   {
      NodeData  = Data;
      pNext = NULL;
   }
   T NodeData;
   BnlNode<T>* pNext;
};
template <typename T>
class BnlQList // base no lock queue list
{
private:
	BnlNode<T>* m_pHead; // head point
	BnlNode<T>* m_pTail; // tail point
public:
	BnlQList(T Dummy)
	{
		m_pHead = new BnlNode<T>(Dummy);
		m_pTail = m_pHead;
	}
	virtual ~BnlQList()
	{
		T tempdata;
		while(true)
		{
			if (!GetFromHead(tempdata))
			{
				break;
			}
		}
		if (NULL != m_pHead)
		{
			delete m_pHead;
			m_pHead = NULL;
			m_pTail = NULL;
		}
	}
public:
	bool AddToTail(T Data)
	{
		BnlNode<T> * pnewnode = new BnlNode<T>(Data);
		BnlNode<T> * ptailnode = m_pTail;
		// try add new node to tail
		do{
			ptailnode = m_pTail;
		}while(!__sync_bool_compare_and_swap(&ptailnode->pNext,NULL,pnewnode));
		// set tailnode
		__sync_bool_compare_and_swap(&m_pTail,ptailnode,pnewnode);
		return true;
	}
	bool GetFromHead(T & Data) // the node will be deleted
	{
		BnlNode<T> * pheadnode = m_pHead;
		// try move head point to next
		do{
			pheadnode = m_pHead;
			if (NULL == pheadnode->pNext)
			{
				return false;
			}
		}while(!__sync_bool_compare_and_swap(&m_pHead,pheadnode,pheadnode->pNext));
		// set return value and delete not used node
		Data = pheadnode->pNext->NodeData;
		delete pheadnode;
		return true;
	}
};

//////////////////////////////////////////////////////////////////////////////////////////
// PrintMemBuf

void PrintMemBuf(const void * buf, const int size);

//////////////////////////////////////////////////////////////////////////////////////////
// CheckSum

unsigned int CheckSum(const void * buf, const int size);

//////////////////////////////////////////////////////////////////////////////////////////
// std string StringSplit

std::vector<std::string> StringSplit(std::string str, std::string pat);

//////////////////////////////////////////////////////////////////////////////////////////
// log

void testlog(const char* Fmt, ...);

//////////////////////////////////////////////////////////////////////////////////////////
// proc sync 

extern void proc_lock_init();
extern void proc_lock();
extern void proc_unlock();

//////////////////////////////////////////////////////////////////////////////////////////
// thread sync 

class ThreadLock
{
public:
	ThreadLock();
	void lock();
	void unlock();

	pthread_mutex_t m_mutex_lock;
};


//////////////////////////////////////////////////////////////////////////////////////////
// proc share mem

extern void proc_mem_init(const char* name, unsigned long size);
extern void* proc_mem_get(const char* name);

//////////////////////////////////////////////////////////////////////////////////////////
// proc share map

extern int psmap_find(const char * key);
extern void psmap_set(const char * key, const char* value);
extern const char* psmap_get(const char * key);

//////////////////////////////////////////////////////////////////////////////////////////
// protect tail mem new

void* protect_tail_new(const int size);
void protect_tail_delete(void* addr);


#endif //#ifndef BASELIB_H_
