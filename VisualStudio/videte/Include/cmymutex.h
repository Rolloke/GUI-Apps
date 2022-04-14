#ifndef __CMYMUTEX_H__
#define __CMYMUTEX_H__
#include "win16_32.h"

class __export CMyMutex
{
public:
	CMyMutex();
	CMyMutex(DWORD dwTimeOut);
	~CMyMutex();

	BOOL IsValid();
	BOOL Lock();
	BOOL Unlock();
	void SetTimeOut(DWORD dwTimeOut);

private:
	BOOL	Init();
	HANDLE	m_hMutex;
	DWORD	m_dwTimeOut;
	BOOL	m_bOk;
};

#endif	// __CMYMUTEX_H__
