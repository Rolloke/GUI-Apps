// BackupThread.h: interface for the CBackupThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACKUPTHREAD_H__9DC06644_CFCA_11D2_B59B_004005A19028__INCLUDED_)
#define AFX_BACKUPTHREAD_H__9DC06644_CFCA_11D2_B59B_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBackupProcess;

class CBackupThread : public CWK_Thread  
{
	// construction / destruction
public:
	CBackupThread(CBackupProcess* pBackupProcess);
	virtual ~CBackupThread();

	// attributes
public:
	CIPCFetchResult*	GetFetchResult();

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);
			void DeleteFetchResult();

	// implementation
private:
	BOOL DoFetch();

	// data member
private:
	CBackupProcess*		m_pBackupProcess;
	CIPCFetchResult*	m_pFetchResult;
	CCriticalSection	m_cs;
	int					m_iTimeOutCounter;
};

#endif // !defined(AFX_BACKUPTHREAD_H__9DC06644_CFCA_11D2_B59B_004005A19028__INCLUDED_)
