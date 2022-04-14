// BackupProcess.h: interface for the CBackupProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BACKUPPROCESS_H__9A914181_CE4C_11D2_B598_004005A19028__INCLUDED_)
#define AFX_BACKUPPROCESS_H__9A914181_CE4C_11D2_B598_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Process.h"

class CBackupThread;
class CIPCDatabaseServerBackup;

class CBackupProcess : public CProcess  
{
public:
	// call and storing process
	CBackupProcess(CProcessScheduler* pScheduler,CActivation *pActivation);
	virtual ~CBackupProcess();

	// attributes
public:
	BOOL IsConnected();
	inline const CString& GetName() const;

	// operations
public:
	void OnBackupSuccess();
	void OnBackupError();

	// overrides
public:
	virtual void Run();

	// implementation
private:
	void StartBackupThread();
	void CheckForConnection();
	void RequestBackup();
	void BackupConfirmed();
	void WaitForDeletes();

private:
	enum BackupState
	{
		BS_INITIALIZED,
		BS_FETCHING,
		BS_CONNECTED,
		BS_RUNNING,
		BS_CONFIRMED,
		BS_DELETING,
		BS_FINISHED,
		BS_FAILED,
		BS_SUSPEND
	};
	// data member
private:
	BackupState		m_eBackupState;
	CBackupThread*	m_pBackupThread;
	CIPCDatabaseServerBackup*	m_pCIPCDatabaseServerBackup;
	DWORD	m_dwBackupID;
	CString m_sName;
	static	CPtrList gm_AutoBackupProcesses; //contains a list of all runnung Backup Processes
	static	CCriticalSection gm_csAutoBackupProcesses;
	static	CDWordArray	 gm_dwaBackupErrorUserData; //contains the UserData of the failed backup
	CSystemTime m_stBackupStart;
};
///////////////////////////////////////////////////////////////////////
inline const CString& CBackupProcess::GetName() const
{
	return m_sName;
}

#endif // !defined(AFX_BACKUPPROCESS_H__9A914181_CE4C_11D2_B598_004005A19028__INCLUDED_)
