// CIPCDatabaseServerBackup.cpp: implementation of the CIPCDatabaseServerBackup class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CIPCDatabaseServerBackup.h"
#include "BackupProcess.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCDatabaseServerBackup::CIPCDatabaseServerBackup(CBackupProcess* pProcess, LPCTSTR shmName)
: CIPCDatabase(shmName,FALSE)
{
	m_pProcess = pProcess;
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCDatabaseServerBackup::~CIPCDatabaseServerBackup()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServerBackup::OnReadChannelFound()
{
	DoRequestConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServerBackup::OnConfirmConnection()
{
	DoRequestVersionInfo(0);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServerBackup::OnRequestVersionInfo(WORD wGroupID)
{
	WK_TRACE(_T("OnRequestVersionInfo confirming 3\n"));
	DoConfirmVersionInfo(0,3);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServerBackup::OnConfirmBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 DWORD dwID)
{
	WORD wArchivNr   = HIWORD(dwID);
	WORD wSequenceNr = LOWORD(dwID);
	WK_TRACE(_T("OnConfirmBackup %d, %s, %d, %d\n"),
				dwUserData,
				sDestinationPath,
				wArchivNr,
				wSequenceNr);
	
	if (dwID == 0)
	{
		WK_TRACE(_T("backup finished successfully\n"));
		m_pProcess->OnBackupSuccess();
	}
	else if (wSequenceNr!=0)
	{
		m_dwaConfirmedSequences.Add(dwID);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServerBackup::OnConfirmBackupByTime(WORD wUserData,
													 DWORD dwID,
													 const CSystemTime& start,
													 const CSystemTime& end,
													 WORD wFlags,
													 CIPCInt64 i64Size)
{
	WORD wArchivNr   = HIWORD(dwID);
	WORD wSequenceNr = LOWORD(dwID);
	WK_TRACE(_T("OnConfirmBackupByTime %d, A: %d, S: %d Start: %s  End: %s\n"),
									wUserData,
									wArchivNr,
									wSequenceNr,
									start.GetDateTime(),
									end.GetDateTime());

	if (dwID == 0)
	{
		WK_TRACE(_T("backup by time finished successfully\n"));
		m_pProcess->OnBackupSuccess();
	}
	else if (wSequenceNr!=0)
	{
		m_dwaConfirmedSequences.Add(dwID);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServerBackup::DeleteConfirmedSequences()
{
	CDWordArray dwaDummy;
	dwaDummy.Append(m_dwaConfirmedSequences);
	int iSize = dwaDummy.GetSize();

	for (int i=0;i<iSize;i++)
	{
		DWORD dwID = dwaDummy.GetAt(i);
		WORD wArchivNr   = HIWORD(dwID);
		WORD wSequenceNr = LOWORD(dwID);
		WK_TRACE(_T("deleting sequence by backup archiv %d, sequence %d\n"),
			wArchivNr,wSequenceNr);
		DoRequestDeleteSequence(wArchivNr,wSequenceNr);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServerBackup::OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
	WK_TRACE(_T("backup sequence deleted archiv %d, sequence %d\n"),
			wArchivNr,wSequenceNr);

	// search the sequence id
	DWORD dwID = MAKELONG(wSequenceNr,wArchivNr);
	for (int i=0;i<m_dwaConfirmedSequences.GetSize();i++)
	{
		if (dwID == m_dwaConfirmedSequences.GetAt(i))
		{
			m_dwaConfirmedSequences.RemoveAt(i);
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////
int CIPCDatabaseServerBackup::GetNrOfConfirmedSequencesToDelete() const
{
	return m_dwaConfirmedSequences.GetSize();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServerBackup::OnIndicateError(DWORD dwCmd, 
												CSecID id, 
												CIPCError error,
												int iErrorCode,
												const CString &sErrorMessage)
{
	if (error == CIPC_ERROR_BACKUP)
	{
		switch (iErrorCode)
		{
		case 1:
			WK_TRACE(_T("backup disconnect error\n"));
			break;
		case 2:
			WK_TRACE(_T("backup initialize error: %s\n"), sErrorMessage);
			break;
		case 3:
			WK_TRACE(_T("backup copy error: %s\n"), sErrorMessage);
			break;
		case 4:
			WK_TRACE(_T("backup time out\n"));
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServerBackup::OnConfirmCancelBackup(DWORD dwUserData)
{
	WK_TRACE(_T("OnConfirmCancelBackup %d\n"),dwUserData);
	m_pProcess->OnBackupError();
}