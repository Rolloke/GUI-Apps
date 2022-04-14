// BackupThread.h: interface for the CBackupThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACKUPTHREAD_H__FCAFEC64_C017_11D2_B57C_004005A19028__INCLUDED_)
#define AFX_BACKUPTHREAD_H__FCAFEC64_C017_11D2_B57C_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Backup.h"

class CBackupThread : public CWK_Thread  
{
	// construction / destruction
public:
	CBackupThread(CBackup* pBackup);
	virtual ~CBackupThread();

	//
public:
	inline CBackup* GetBackup();
		   BOOL	    IsTimedOut();
		   BOOL		IsCancelled();

	// operations
public:
	virtual	BOOL Run(LPVOID lpContext);
			void Cancel(BOOL bByClient);
			void HandleTimeOut();
			void SetLastAction();

	// implementation
private:
			BOOL RunCopy();
			BOOL RunCalculate();
			BOOL RunCalculateEnd();
			void DoIndicateError(int iUnitCode, DWORD dwID=(DWORD)-1);

	// data member
private:
	CBackup* m_pBackup;
	BOOL     m_bCancelled;
	int		 m_iCurrentTape;
	DWORD	 m_dwLastAction;
	DWORD	 m_dwTimeOut;
	BOOL	 m_bFinished;
	DWORD	 m_dwID;
	BOOL	 m_dwTimeOutHandled;
	CCriticalSection m_csTimeOut;
};
///////////////////////////////////////////////////
inline CBackup* CBackupThread::GetBackup()
{
	return m_pBackup;
}
///////////////////////////////////////////////////
typedef CBackupThread* CBackupThreadPtr;
WK_PTR_ARRAY_CS(CBackupThreadArray,CBackupThreadPtr,CBackupThreads);
///////////////////////////////////////////////////
class CSequenceSize
{
public:
	CSequenceSize(CTape& s, CBackup& b);

private:
	CSystemTime m_stStart;
	CSystemTime m_stEnd;
	CIPCInt64	m_i64SizeInBytes;
	WORD		m_wArchiveNr;
	WORD		m_wSequenceNr;

	friend class CBackupThread;
};
typedef CSequenceSize* CSequenceSizePtr;
WK_PTR_ARRAY(CSequenceSizeArray,CSequenceSizePtr);

#endif // !defined(AFX_BACKUPTHREAD_H__FCAFEC64_C017_11D2_B57C_004005A19028__INCLUDED_)
