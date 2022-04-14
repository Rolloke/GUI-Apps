// RequestThread.h: interface for the CRequestThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_REQUESTTHREAD_H__DB8EAFA1_8C75_11D3_A870_004005A19028__INCLUDED_)
#define AFX_REQUESTTHREAD_H__DB8EAFA1_8C75_11D3_A870_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCameraGroup;

class CRequestThread : public CWK_Thread  
{
	// construction / destruction
public:
	CRequestThread(const CString& sName);
	virtual ~CRequestThread();

	// attributes
public:
		   BOOL	 IsTimedOut();

	// operations
public:
	void SetCameraGroup(CCameraGroup* pGroup);

	// overrides
public:
	virtual	BOOL Run(LPVOID lpContext);

private:
	CCameraGroup* m_pCameraGroup;
	DWORD m_dwTick;
	CCriticalSection m_csTick;
};

#endif // !defined(AFX_REQUESTTHREAD_H__DB8EAFA1_8C75_11D3_A870_004005A19028__INCLUDED_)
