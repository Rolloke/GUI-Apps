// CIPCDatabaseServerBackup.h: interface for the CIPCDatabaseServerBackup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CIPCDATABASESERVERBACKUP_H__9DC06643_CFCA_11D2_B59B_004005A19028__INCLUDED_)
#define AFX_CIPCDATABASESERVERBACKUP_H__9DC06643_CFCA_11D2_B59B_004005A19028__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBackupProcess;

class CIPCDatabaseServerBackup : public CIPCDatabase 
{
	// construction/destruction
public:
	CIPCDatabaseServerBackup(CBackupProcess* pProcess, LPCTSTR shmName);
	virtual ~CIPCDatabaseServerBackup();

	// attributes
public:
	int GetNrOfConfirmedSequencesToDelete() const;

	// operations
public:
	void DeleteConfirmedSequences();

	// overrides
public:
	virtual void OnReadChannelFound();
	virtual void OnConfirmConnection();
	virtual void OnRequestVersionInfo(WORD wGroupID);
	virtual	void OnConfirmBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 DWORD dwID);
	virtual	void OnConfirmBackupByTime(WORD wUserData,
									   DWORD dwID,
									   const CSystemTime& start,
									   const CSystemTime& end,
									   WORD wFlags,
									   CIPCInt64 i64Size);
	virtual void OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr);
	virtual void OnIndicateError(DWORD dwCmd, 
								CSecID id, 
								CIPCError error,
								int iErrorCode,
								const CString &sErrorMessage);
	virtual void OnConfirmCancelBackup(DWORD dwUserData);

	// private data
private:
	CBackupProcess* m_pProcess;
	CDWordArray		m_dwaConfirmedSequences;
};

#endif // !defined(AFX_CIPCDATABASESERVERBACKUP_H__9DC06643_CFCA_11D2_B59B_004005A19028__INCLUDED_)
