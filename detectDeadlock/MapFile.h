#ifndef CXX_MAPFILE_H
#define CXX_MAPFILE_H

#include "stdio.h"

#ifndef SEEK_SET
    #define SEEK_SET			0
    #define SEEK_CUR			1
    #define SEEK_END			2
#endif

#define NFILE_WRITE_FLUSH		(0x00000000)
#define NFILE_WRITE_ONLY		(0x00000001)
#define NFILE_NO_CHECKSUM		(0x00000002)

const static unsigned int AL_FILE_PARTITION_NDATA = 0x00000002;
const static unsigned int AL_FILE_PARTITION_RDATA = 0x00000003;

const static int    WCHAR_CODE = 936;

class  MapFile{
public:
	MapFile() : m_pFile(NULL){};

	unsigned int GetLastError()
	{
		return 0;
	}

	virtual bool SetEndOfFile()
	{
		return false;
	}

	virtual ~MapFile(){};

	virtual bool OpenFile(const char* file_name, const char* rw_mode = "rb");

	virtual bool OpenFile(const wchar_t* file_name, const char* rw_mode = "rb");

	virtual bool IsOpen();

	virtual bool CloseFile();

	virtual bool Seek(long offset, long int start = SEEK_SET);

	virtual bool Read(void* buff, long int size,long int count);

	virtual bool Write(const void* buff, long int size,long int count);

	virtual bool WriteExt(void*, long int size, long int count, unsigned int param = NFILE_WRITE_FLUSH);

	virtual bool Flush();

	virtual unsigned int GetFileSize();

	virtual char* Gets(char* buff, int n);

	virtual wchar_t* Gets(wchar_t* buff, int n);

private:

	virtual bool Delete(const char* file_name);

	virtual bool Delete(const wchar_t* file_name);

	FILE* m_pFile;
};

#endif

