
#ifndef CXX_MAPFILE_H
#	include "MapFile.h"
#endif

#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;


bool
MapFile::OpenFile(const char* file_name, const char* rw_mode)
{
	m_pFile = fopen(file_name, rw_mode);
	return bool(m_pFile != NULL);
}

bool
MapFile::OpenFile(const wchar_t* file_name, const char* rw_mode)
{
    return false;
}

bool
MapFile::IsOpen()
{
	if (NULL == m_pFile)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool
MapFile::CloseFile()
{
	if (NULL != m_pFile)
	{
		bool bResult = !fclose(m_pFile);
		m_pFile = NULL;
		return bResult;
	}
	else
	{
		return false;
	}
}

bool
MapFile::Seek(long offset, long int start)
{
	if (NULL != m_pFile)
	{
		return !fseek(m_pFile,offset,start);
	}
	else
	{
		return false;
	}
}

bool
MapFile::Read(void* buff, long int size,long int count)
{
	size_t nRead = fread(buff,size,count,m_pFile);
	if (ferror(m_pFile))
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool
MapFile::Write(const void* buff, long int size,long int count)
{
	return ((unsigned long int)(size) == fwrite(buff,size,count, m_pFile));
}

bool
MapFile::WriteExt(void* buff, long int size, long int count, unsigned int param)
{
	//return m_cFile.WriteExt(buff,size,count,param);
	return false;
}

bool
MapFile::Flush()
{
	return !fflush(m_pFile);
}
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
/*----------                   >>ŠÖ”‹æØ‚è<<                     -----------*/
/*-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-*/
bool
MapFile::Delete(const char* file_name)
{
	return false;
}

bool
MapFile::Delete(const wchar_t* file_name)
{
	return false;
}

unsigned int
MapFile::GetFileSize()
{
	fseek (m_pFile , 0 , SEEK_END);
	unsigned int dwSize = ftell (m_pFile);
	rewind (m_pFile);
	return dwSize;
}

char*
MapFile::Gets(char* buff, int n)
{
	return NULL;
}

wchar_t*
MapFile::Gets(wchar_t* buff, int n)
{
	return NULL;
}

