#include "../header/logger_windows.h"
#include <stdio.h>
#include<ctime>

CFileMem::CFileMem()
{
	m_pMemBlock = NULL;
	m_dwMemBlockSize = 0;
	m_dwPos = 0;
	memset(m_szPathFile, 0, 256);
	::InitializeCriticalSection(&m_csMem);
}

CFileMem::~CFileMem()
{
	Close();
	::DeleteCriticalSection(&m_csMem);
}

bool CFileMem::setLogPath(std::string& ipath)
{
	if (ipath.empty())return 0;
	CFileMem::log_path = ipath.c_str();
	return 1;
}

BOOL CFileMem::Open(const char* pszPathFile, DWORD dwMemBlockSize)
{
	if (!pszPathFile)
		return FALSE;

	Close();

	strcpy_s(m_szPathFile, pszPathFile);

	if (dwMemBlockSize <= 0)
		return FALSE;
	m_pMemBlock = (char*)malloc(dwMemBlockSize);
	if (NULL == m_pMemBlock)
		return FALSE;

	memset(m_pMemBlock, 0, dwMemBlockSize);
	m_dwMemBlockSize = dwMemBlockSize;
	m_dwPos = 0;
	return TRUE;
}


BOOL CFileMem::Write(const char* pszLog, DWORD& dwFileSize)
{
	return Write((const unsigned char*)pszLog, strlen(pszLog), dwFileSize);
}
BOOL CFileMem::Write(const unsigned char* pData, DWORD dwDataSize, DWORD& dwFileSize)
{
	dwFileSize = 0;
	if (NULL == pData || 0 == dwDataSize)
		return FALSE;

	if (NULL == m_pMemBlock || 0 == m_dwMemBlockSize)
	{
		FILE* pFile;
		::fopen_s(&pFile,m_szPathFile, "ab+");
		if (NULL == pFile)
			return FALSE;
		::fwrite(pData, 1, dwDataSize, pFile);
		::fwrite("\r\n", 1, sizeof("\r\n"), pFile);

		::fseek(pFile, 0L, SEEK_END);
		dwFileSize = ::ftell(pFile);
		::fclose(pFile);
		return TRUE;
	}

	::EnterCriticalSection(&m_csMem);

	DWORD dwTotalSize = m_dwPos + dwDataSize;
	if (dwTotalSize >= m_dwMemBlockSize)
	{
		FILE* pFile;
		::fopen_s(&pFile,m_szPathFile, "ab+");
		if (NULL == pFile)
		{
			::LeaveCriticalSection(&m_csMem);
			return FALSE;
		}

		::fwrite(m_pMemBlock, 1, m_dwPos, pFile);
		::fwrite(pData, 1, dwDataSize, pFile);
		::fwrite("\r\n", 1, sizeof("\r\n"), pFile);

		::fseek(pFile, 0L, SEEK_END);
		dwFileSize = ::ftell(pFile);
		::fclose(pFile);

		memset(m_pMemBlock, 0, m_dwMemBlockSize);
		m_dwPos = 0;
	}
	else
	{
		memcpy(m_pMemBlock + m_dwPos, pData, dwDataSize);
		m_dwPos += dwDataSize;
	}
	::LeaveCriticalSection(&m_csMem);
	return TRUE;
}

BOOL CFileMem::Flush(DWORD& dwFileSize)
{
	dwFileSize = 0;
	if (NULL == m_pMemBlock || 0 == m_dwMemBlockSize || 0 == m_dwPos)
		return TRUE;

	::EnterCriticalSection(&m_csMem);
	FILE* pFile;
	::fopen_s(&pFile,m_szPathFile, "ab+");
	if (NULL == pFile)
	{
		::LeaveCriticalSection(&m_csMem);
		return FALSE;
	}
	::fwrite(m_pMemBlock, 1, m_dwPos, pFile);

	::fseek(pFile, 0L, SEEK_END);
	dwFileSize = ::ftell(pFile);
	::fclose(pFile);

	memset(m_pMemBlock, 0, m_dwMemBlockSize);
	m_dwPos = 0;

	::LeaveCriticalSection(&m_csMem);

	return TRUE;
}

void CFileMem::Close()
{
	DWORD dwFileSize = 0;
	Flush(dwFileSize);

	free(m_pMemBlock);
	m_pMemBlock = NULL;
	m_dwMemBlockSize = 0;
	m_dwPos = 0;
	memset(m_szPathFile, 0, 256);
}

void CFileMem::Rename(const char* pszOldPathFile, const char* pszNewPathFile)
{
	::EnterCriticalSection(&m_csMem);
	::rename(pszOldPathFile, pszNewPathFile);
	::LeaveCriticalSection(&m_csMem);
}

void CFileMem::LogTime()
{
	time_t start;
	time(&start);
	char buf[26];
	ctime_s(buf, 26, &start);
	DWORD dwFileSize;
	Write("Time:", dwFileSize);
	Write(buf, dwFileSize);
	Write("\n", dwFileSize);
}

void CFileMem::quickLog(const char* istr)
{
	DWORD dwFileSize;
	Write(istr, dwFileSize);
}
