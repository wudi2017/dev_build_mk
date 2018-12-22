

#ifndef MEM_PRIVATE_H
#define MEM_PRIVATE_H

const int EXTRA_MAGIC_CODE = 0xa5a5;
const int EXTRA_MAGIC_LEN  = sizeof(int);

void* MEM_malloc_private(unsigned int sz);

void MEM_free_private(void* mem);

bool MEM_Log_private(bool bLog2File, const char* pszString, ...);

bool MEM_Log2Serial_private(const char* pszString, ...);

void Log2File(const char* pszString, const int& nLogLen);

void FlushLogFile();

void CloseLogFile();

#endif
/* EOF */