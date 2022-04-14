// GuardTourThread.h: interface for the CGuardTourThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUARDTOURTHREAD_H__5E28ADAB_BD0A_49DD_AAC9_5CEA5D44808A__INCLUDED_)
#define AFX_GUARDTOURTHREAD_H__5E28ADAB_BD0A_49DD_AAC9_5CEA5D44808A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CGuardTourProcess;

class CGuardTourThread : public CWK_Thread  
{
public:
	CGuardTourThread(CGuardTourProcess* pProcess, CConnectionRecord* pCR);
	virtual ~CGuardTourThread();


	// attributes
public:
	CIPCFetchResult*	GetFetchResult();

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);
	virtual	BOOL StopThread();
			void DeleteFetchResult();
			void Lock();
			void Unlock();
			void Cancel();

	// implementation
private:
	BOOL DoFetch();


private:
	CGuardTourProcess*		m_pProcess;
	CConnectionRecord*		m_pConnectionRecord;
	CIPCFetch				m_Fetch;
	CIPCFetchResult*		m_pFetchResult;
	CCriticalSection		m_cs;
	BOOL					m_bCancelled;
	BOOL					m_bFetching;
	volatile static BOOL	m_sbFetching;
};

#endif // !defined(AFX_GUARDTOURTHREAD_H__5E28ADAB_BD0A_49DD_AAC9_5CEA5D44808A__INCLUDED_)
