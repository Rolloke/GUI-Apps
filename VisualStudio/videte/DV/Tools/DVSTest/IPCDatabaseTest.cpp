// IPCDatabaseTest.cpp: implementation of the CIPCDatabaseTest class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DVSTest.h"
#include "IPCDatabaseTest.h"
#include "DVSTestDlg.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCDatabaseTest::CIPCDatabaseTest(CDVSTestDlg* pDlg, LPCTSTR shmName)
:CIPCDatabaseClient(shmName)
{
	m_pDlg = pDlg;
	StartThread();
}

CIPCDatabaseTest::~CIPCDatabaseTest()
{
	StopThread();
	m_Sequences.DeleteAll();
}
void CIPCDatabaseTest::OnConfirmConnection()
{
	DoRequestVersionInfo(0);
	CIPCDatabaseClient::OnConfirmConnection();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseTest::OnRequestVersionInfo(WORD wGroupID)
{
	// Nr 1 is default
	// Nr 2 is Version 3.6 Build 133
	// Nr 3 is new Recherche
	WK_TRACE(_T("OnRequestVersionInfo confirming 3\n"));
	DoConfirmVersionInfo(0,3);
}
void CIPCDatabaseTest::OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[])
{
	CIPCDatabaseClient::OnConfirmInfo(iNumRecords,data);
	m_pDlg->PostMessage(WM_USER,GetLastCmd());
}
void CIPCDatabaseTest::OnConfirmAlarmListArchives(int iNumRecords, 
												  const CIPCArchivRecord data[])
{
	TRACE(_T("OnConfirmAlarmListArchives\n"));
	for (int i=0;i<iNumRecords;i++)
	{
		OnIndicateNewArchiv(data[i]);
	}
	m_pDlg->PostMessage(WM_USER,GetLastCmd());
}
void CIPCDatabaseTest::OnConfirmSequenceList(WORD wArchivNr, int iNumRecords, 
											 const CIPCSequenceRecord data[])
{
	m_Sequences.Lock();
	for (int i=0;i<iNumRecords;i++)
	{
		m_Sequences.RemoveSequence(data[i].GetArchiveNr(),data[i].GetSequenceNr());
		m_Sequences.Add(new CIPCSequenceRecord(data[i]));
	}
	m_Sequences.Unlock();
	m_pDlg->PostMessage(WM_USER,GetLastCmd(),wArchivNr);
}
void CIPCDatabaseTest::OnConfirmBackupByTime(WORD wUserData,
										     DWORD dwID,
										     const CSystemTime& start,
										     const CSystemTime& end,
										     WORD wFlags,
										     CIPCInt64 i64Size)
{
//	TRACE(_T("OnConfirmBackupByTime(%04hx,%08lx,%x,%s)\n"),wUserData,dwID,wFlags,i64Size.Format(TRUE));
	if (wFlags == BBT_CALCULATE)
	{
		m_pDlg->m_sVolume = i64Size.Format(TRUE);
	}
	else
	{
		m_pDlg->m_sCopied = i64Size.Format(TRUE);
	}
	m_pDlg->PostMessage(WM_USER,GetLastCmd(),wUserData);
}
void CIPCDatabaseTest::OnConfirmBackup(DWORD dwUserData,
										 const CString& sDestinationPath,
										 DWORD dwID)
{
	TRACE(_T("OnConfirmBackup %s\n"),sDestinationPath);
}
void CIPCDatabaseTest::OnRequestDisconnect()
{
	DelayedDelete();
	m_pDlg->PostMessage(WM_USER,1234);
}
void CIPCDatabaseTest::OnIndicateNewSequence(const CIPCSequenceRecord& data)
{
	// TODO
}
void CIPCDatabaseTest::OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
	m_Sequences.RemoveSequence(wArchivNr,wSequenceNr);
	m_pDlg->PostMessage(WM_USER,GetLastCmd(),MAKELONG(wSequenceNr,wArchivNr));
}
void CIPCDatabaseTest::OnConfirmRecords(WORD  wArchivNr, 
										 WORD  wSequenceNr, 
										 CIPCFields fields,
										 CIPCFields records)
{
	m_SequenceFields = fields;
	m_SequenceRecords = records;
	m_pDlg->PostMessage(WM_USER,GetLastCmd(),MAKELONG(wSequenceNr,wArchivNr));

}
