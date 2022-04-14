// IPCDatabaseDVClient.cpp: implementation of the CIPCDatabaseDVClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "dvclient.h"
#include "IPCDatabaseDVClient.h"

#include "Server.h"
#include "MainFrame.h"
#include "PlayWindow.h"
#include "CPanel.h"
#include "PlayRealTime.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CIPCDatabaseDVClient::CIPCDatabaseDVClient(LPCTSTR shmName, CServer* pServer)
 :CIPCDatabaseClient(shmName)
{
	m_bBackupIgnoreConfirmQuery = FALSE;
	m_pServer = pServer;
#ifdef _UNICODE
	m_wCodePage = pServer->GetDataBaseCodePage();
#endif
	SetConnectRetryTime(50);
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCDatabaseDVClient::~CIPCDatabaseDVClient()
{
	StopThread();
	m_Drives.SafeDeleteAll();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnConfirmConnection()
{
	WK_TRACE(_T("CIPCDatabaseDVClient::OnConfirmConnection %s\n"),m_pServer->GetHostName());
	DoRequestVersionInfo(0);
	RefreshInfo();
	theApp.ForceOnIdle();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
	if(theApp.TKR_Trace())
	{
		TRACE(_T("TKR**** wGroupID: %i  dwVersion: %i\n"),wGroupID,dwVersion );
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnRequestVersionInfo(WORD wGroupID)
{
	// Nr 1 is default
	// Nr 2 is Version 3.6 Build 133
	// Nr 3 is new Recherche

	DoConfirmVersionInfo(0,3);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[])
{
	CIPCDatabaseClient::OnConfirmInfo(iNumRecords, data);

	CWnd* pWnd = theApp.GetMainWnd();
	if (pWnd)
	{
		for (int i=0;i<iNumRecords;i++)
		{
			WORD wID = data[i].GetSubID();

			// post info to main thread
			pWnd->PostMessage(WM_USER,
								MAKELONG(WPARAM_ADD_ARCHIV,0),
								CSecID(SECID_GROUP_ARCHIVE,wID).GetID());
		}

		pWnd->PostMessage(WM_USER,
						  MAKELONG(WPARAM_ADD_ARCHIVS,0),0);
	}
	CPanel* pPanel = theApp.GetPanel();
	if (pPanel)
	{
		pPanel->NotifyCIPCConnect(SC_DATABASE);
	}

	DoRequestDrives();

	// give main thread a chance to create window
	Sleep(100);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnIndicateNewArchiv(const CIPCArchivRecord& data)
{
	CIPCDatabaseClient::OnIndicateNewArchiv(data);
	CWnd* pWnd = theApp.GetMainWnd();
	if (pWnd)
	{
		WORD wID = data.GetSubID();
		pWnd->PostMessage(WM_USER,
						  MAKELONG(WPARAM_ADD_ARCHIV,0),
						  CSecID(SECID_GROUP_ARCHIVE,wID).GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnIndicateRemoveArchiv(WORD wArchivNr)
{
	CIPCDatabaseClient::OnIndicateRemoveArchiv(wArchivNr);
	CWnd* pWnd = theApp.GetMainWnd();
	if (pWnd)
	{
		pWnd->PostMessage(WM_USER,
						  MAKELONG(WPARAM_DEL_ARCHIV,0),
						  CSecID(SECID_GROUP_ARCHIVE,wArchivNr).GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnConfirmSequenceList(WORD wArchivNr, int iNumRecords, 
												 const CIPCSequenceRecord data[])
{
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CPlayWindow* pPW = pMF->GetPlayWindow(wArchivNr);
	//	WK_TRACE(_T("OnConfirmSequenceList %04x %08lx\n"),wArchivNr,(DWORD)pPW);
		if (pPW)
		{
			pPW->IndicateSequenceList(iNumRecords,data);
		}
		else
		{
			WK_TRACE(_T("sequence list without window %04x\n"),wArchivNr);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CPlayWindow* pPW = pMF->GetPlayWindow(wArchivNr);
		TRACE(_T("OnIndicateDeleteSequence %04hx, %d\n"),wArchivNr,wSequenceNr);
		if (pPW)
		{
			pPW->IndicateDeleteSequence(wSequenceNr);
		}
		else
		{
			WK_TRACE(_T("delete sequence without window %04hx, %d\n"),wArchivNr,wSequenceNr);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnIndicateNewSequence(const CIPCSequenceRecord& data,
												 WORD  wPrevSequenceNr,
												 DWORD dwNrOfRecords)
{
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CPlayWindow* pPW = pMF->GetPlayWindow(data.GetArchiveNr());
		if (pPW)
		{
			pPW->IndicateNewSequence(data,wPrevSequenceNr,dwNrOfRecords);
		}
		else
		{
			WK_TRACE(_T("new sequence without window %04hx %d\n"),data.GetArchiveNr(),
														data.GetSequenceNr());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnRequestDisconnect()
{
	DelayedDelete();
	
	// Wenn die Verbindung zur Datenbank unterbrochen wird, soll die Livebilddarstellung
	// davon unberührt bleiben, also auch kein erneuter Login erfolgen. Hat sich die
	// Datenbank beendet, so wird sie vom DVStarter erneut gestartet und eine CIPC-Verbindung
	// angelegt. 
	//	ML kein globales Disconnet durchführen!! AfxGetMainWnd()->PostMessage(WM_USER,MAKELONG(WPARAM_DISCONNECT,0),0);

}
//////////////////////////////////////////////////////////////////////
CIPCArchivRecord* CIPCDatabaseDVClient::GetArchive(CSecID idArchive)
{
	CIPCArchivRecordArray& a = GetRecords();
	int i,c;
	CIPCArchivRecord* pRecord = NULL;
	CAutoCritSec acs(&a.m_cs);
	c = a.GetSize();
	for (i=0;i<c;i++)
	{
		if (a.GetAtFast(i)->GetSubID() == idArchive.GetSubID())
		{
			pRecord = a.GetAtFast(i);
			break;
		}
	}
	return pRecord;
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnConfirmRecordNr(WORD wArchivNr, WORD wSequenceNr, 
											  DWORD dwRecordNr,
											  DWORD dwNrOfRecords,
											  const CIPCPictureRecord &pict,
											  int iNumFields,
											  const CIPCField fields[])
{
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CPlayWindow* pPW = pMF->GetPlayWindow(wArchivNr);
		if (pPW)
		{
			

			if(    theApp.CanRealtime() 
				&& !pPW->ContainsAudio()	//Realtime nicht in audio archiven
				&& pPW->GetPlayRealTime()) 
			{
				CPlayRealTime* pPlayRealTime = pPW->GetPlayRealTime();
				if(    pPW->GetPlayStatus() == CPlayWindow::PS_PLAY_FORWARD
					|| pPW->GetPlayStatus() == CPlayWindow::PS_PLAY_BACK)
				{
					BOOL bPlayNextPicture = FALSE;

					bPlayNextPicture = pPlayRealTime->CheckRealTimePicture(wArchivNr,
																		wSequenceNr,
															  			dwRecordNr,
																		dwNrOfRecords,
																		pict,
																		iNumFields,
																		fields,
																		pPW->m_hWnd);				
					if(bPlayNextPicture)
					{
						pPW->RecordNr(wSequenceNr,dwRecordNr,dwNrOfRecords,pict,iNumFields,fields);
					}
				}
				else
				{
					pPW->RecordNr(wSequenceNr,dwRecordNr,dwNrOfRecords,pict,iNumFields,fields);
				}
			}
			else //kein Realtime
			{
				if(theApp.TKR_Trace())
				{
					TRACE(_T("TKR ### NO REALTIME CIPCDatabaseDVClient::OnConfirmRecordNr:  ID: %d\n"), pPW->GetID());
				}
				pPW->RecordNr(wSequenceNr,dwRecordNr,dwNrOfRecords,pict,iNumFields,fields);
			}
		}
		else
		{
			WK_TRACE(_T("record without window\n"));
		}
	}
}
void CIPCDatabaseDVClient::OnConfirmRecordNr(WORD wArchivNr, 
									  WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCMediaSampleRecord &media,
									  int iNumFields,
									  const CIPCField fields[])
{
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CPlayWindow* pPW = pMF->GetPlayWindow(wArchivNr);
		if (pPW)
		{
	//		TRACE(_T("Mediarecord SNr:%d, RNr:%d, MNr:%d\n"), wSequenceNr, dwRecordNr, media.GetPackageNumber());
			pPW->RecordNr(wSequenceNr, dwRecordNr, dwNrOfRecords, media);
		}
		else
		{
			WK_TRACE(_T("record without window\n"));
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnIndicateError(DWORD dwCmd, CSecID id, 
										   CIPCError error, int iErrorCode,
										   const CString &sErrorMessage)
{
//TODO TKR Errors beim Brennen ebenfalls mit BackupWindow händeln
	if (error == CIPC_ERROR_BACKUP)
	{
		DWORD dwID = id.GetID();
		WORD wArchivNr = HIWORD(dwID);
		WORD wSequenceNr = LOWORD(dwID);
		switch (iErrorCode)
		{
		case 1:
			WK_TRACE(_T("backup disconnect error\n"));
			break;
		case 2:
			WK_TRACE(_T("backup initialize error %08lx\n"),dwID);
			break;
		case 3:
			WK_TRACE(_T("backup copy error %d, %d\n"),wArchivNr,wSequenceNr);
			break;
		case 4:
			WK_TRACE(_T("backup time out error %d, %d\n"),wArchivNr,wSequenceNr);
			break;
		case 5:
			WK_TRACE(_T("backup cancelled by server %d, %d\n"),wArchivNr,wSequenceNr);
			//ein externe Medium, auf das gerade ein backup gemacht wird, wurde entfernt
			CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
			if(pBW)
			{
				pBW->SetCloseBackupWindow(TRUE, TRUE);
			}

			break;
		}
	}
	else if (error == CIPC_ERROR_ACCESS_DENIED)
	{
		switch (dwCmd)
		{
		case CIPC_DB_REQUEST_RECORD_NR:
			// request record nr during server reset
			{
				WORD wArchivNr = id.GetSubID();
				CPlayWindow* pPW = theApp.GetMainFrame()->GetPlayWindow(wArchivNr);
				if (pPW)
				{
					pPW->IndicateError(error,iErrorCode,sErrorMessage);
				}
			}
			break;
		case CIPC_DB_REQUEST_NEW_QUERY:
			// request query during server reset
			{
				WORD wArchivNr = id.GetSubID();
				CPlayWindow* pPW = theApp.GetMainFrame()->GetPlayWindow(wArchivNr);
				if (pPW)
				{
					pPW->ConfirmQuery();
				}
			}
			break;
		default:
			break;
		}
	}
	else if (error == CIPC_ERROR_INVALID_VALUE)
	{
		WORD wArchivNr = HIWORD(dwCmd);

		CPlayWindow* pPW = theApp.GetMainFrame()->GetPlayWindow(wArchivNr);
		if (pPW)
		{
			pPW->IndicateError(error,iErrorCode,sErrorMessage);
		}
	}
	else
	{
		if (!sErrorMessage.IsEmpty())
		{
			WK_TRACE(_T("unknown error indication: %s\n"), LPCTSTR(sErrorMessage));
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnIndicateNewQueryResult(DWORD dwUserID,
													 WORD wArchivNr,
													 WORD wSequenceNr,
													 DWORD dwRecordNr,
													 int iNumFields,
													 const CIPCField fields[])
{
	// always stop query immediately
	DoResponseQueryResult(0);

	if(dwUserID == BACKUP_LAST_RECORD)
	{
		m_bBackupIgnoreConfirmQuery = TRUE;
		if(theApp.TKR_Trace())
		{
			TRACE(_T("TKR ######### OnIndicateNewQueryResult() m_bBackupIgnoreConfirmQuery: %d Status: %d\n"), m_bBackupIgnoreConfirmQuery, dwUserID);
		}
	
		//Suchanfrage aus BackupWindow, um letztes (neustes) Bild des aktuellen backups
		//aus Datenbank zu holen

		CMainFrame *pMF = theApp.GetMainFrame();
		if (pMF)
		{
			CBackupWindow* pBW = pMF->GetBackupWindow();
			if (pBW)
			{
				pBW->SetLastRecordLocalized(wSequenceNr,dwRecordNr);
				WK_TRACE(_T("BACKUP_LAST_RECORD query result Camera %d %d,%d\n"),(wArchivNr&0xFF),wSequenceNr,dwRecordNr);
			}
		}
	}
	else if(dwUserID == PANEL_LAST_RECORD)
	{
		if(theApp.TKR_Trace())
		{
			TRACE(_T("TKR ######### OnIndicateNewQueryResult() m_bBackupIgnoreConfirmQuery: %d Status: %d\n"), m_bBackupIgnoreConfirmQuery, dwUserID);
		}

		//Suchanfrage aus Panel, um nächstes Bild (neueres) hinter dem letztes Bild 
		//des letzten backups aus Datenbank zu holen

		CPanel* pPanel = theApp.GetPanel();
		if (pPanel)
		{
			pPanel->SetEventLastBackupRecord(TRUE);
			WK_TRACE(_T("PANEL_LAST_RECORD query result Camera %d %d,%d\n"),(wArchivNr&0xFF),wSequenceNr,dwRecordNr);
		}
	}
	else
	{
		CMainFrame *pMF = theApp.GetMainFrame();
		CPlayWindow* pPW = NULL;
		if (pMF)
		{
			pPW = pMF->GetPlayWindow(wArchivNr);
			if(theApp.TKR_Trace())
			{
				TRACE(_T("TKR ######### OnIndicateNewQueryResult() m_bBackupIgnoreConfirmQuery: %d Status: %d\n"), m_bBackupIgnoreConfirmQuery, dwUserID);
			}
		}			
		if (pPW)
		{
			if (pPW->IndicateQueryResult(wSequenceNr,dwRecordNr))
			{
				WK_TRACE(_T("query result Camera %d %d,%d\n"),(wArchivNr&0xFF),wSequenceNr,dwRecordNr);
			}
		}
		else
		{
			WK_TRACE(_T("query result without play window\n"));
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnConfirmQuery(DWORD dwUserID)
{
	if(theApp.TKR_Trace())
	{
		TRACE(_T("TKR ######### OnConfirmQuery() m_bBackupIgnoreConfirmQuery: %d Status: %d\n"), m_bBackupIgnoreConfirmQuery, dwUserID);
	}
	if(dwUserID == BACKUP_LAST_RECORD && !m_bBackupIgnoreConfirmQuery)
	{
		//Suchanfrage aus BackupWindow, um letztes (neustes) Bild des aktuellen backups
		//aus Datenbank zu holen gescheitert, kein Bild vorhanden

		CMainFrame *pMF = theApp.GetMainFrame();
		if (pMF)
		{
			CBackupWindow* pBW = pMF->GetBackupWindow();
			if (pBW)
			{
				pBW->SetLastRecordLocalized(0,0,FALSE);
				WK_TRACE(_T("BACKUP_LAST_RECORD OnConfirmQuery\n"));
			}
		}
	}
	else if(dwUserID == PANEL_LAST_RECORD)
	{
		//Suchanfrage aus Panel, um nächstes Bild (neueres) hinter dem letztes Bild 
		//des letzten backups aus Datenbank zu holen gescheitert, kein Bild vorhanden

		CPanel* pPanel = theApp.GetPanel();
		if (pPanel)
		{
			pPanel->SetEventLastBackupRecord(FALSE);
			WK_TRACE(_T("PANEL_LAST_RECORD OnConfirmQuery\n"));
		}
	}
	else
	{
		CSecID id(dwUserID);
		CMainFrame *pMF = theApp.GetMainFrame();
		if (pMF)
		{
			CPlayWindow* pPW = pMF->GetPlayWindow(id.GetSubID());
			if (pPW)
			{
				if(theApp.TKR_Trace())
				{
					TRACE(_T("TKR ### confirm query ID: %d\n"), dwUserID);
				}
				pPW->ConfirmQuery();
			}
			else
			{
				WK_TRACE(_T("confirm query without play window\n"));
			}
		}
	}


}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnConfirmGetValue(CSecID id,
												const CString &sKey,
												const CString &sValue,
												DWORD dwServerData)
{
	if (sKey == CSD_NAME)
	{
		CMainFrame *pMF = theApp.GetMainFrame();
		if (pMF)
		{
			// new archive name
			CPlayWindow* pPW = pMF->GetPlayWindow(id.GetSubID());
			if (pPW)
			{
				pPW->ConfirmGetValue(sKey,sValue);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnConfirmBackup(DWORD dwUserData,
										   const CString& sDestinationPath,
										   DWORD dwID)
{
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CBackupWindow* pBW = theApp.GetMainFrame()->GetBackupWindow();
		if (pBW)
		{
			pBW->ConfirmBackup(dwID);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnConfirmDrives(int iNumDrives,
										   const CIPCDrive drives[])
{
	m_Drives.Lock(_T(__FUNCTION__));
	m_Drives.SafeDeleteAll();
	m_Drives.FromArray(iNumDrives,drives);
	m_Drives.Unlock();
}
//////////////////////////////////////////////////////////////////////
CIPCDrive* CIPCDatabaseDVClient::GetBackupDrive()
{
	CIPCDrive* pDrive = NULL;

	m_Drives.Lock(_T(__FUNCTION__));
	for (int i=0;i<m_Drives.GetSize();i++)
	{
		if (m_Drives[i]->IsWriteBackup())
		{
			pDrive = m_Drives[i];
			break;
		}
	}
	m_Drives.Unlock();
	return pDrive;
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnConfirmBackupByTime(WORD wUserData,
											     DWORD dwID,
												 const CSystemTime& stStartLessMaxCDCapacity,
												 const CSystemTime& stEndMoreMaxCDCapacity,
												 WORD wFlags,
												 CIPCInt64 i64Size)

{
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CBackupWindow* pBW = pMF->GetBackupWindow();
		if(pBW)
		{
			if(pBW->GetBackupUserID() == wUserData)
			{
				if(    (wFlags == BBT_CALCULATE 
					||  wFlags == BBT_CALCULATE_END)
					&& dwID == 0)
				{
					TRACE(_T("TKR----- OnConfirmBackupByTime() 64Size: %i\n"), i64Size.GetInMB());
				}
				pBW->ConfirmBackupByTime(wUserData, dwID, wFlags, i64Size, 
							  			stStartLessMaxCDCapacity, stEndMoreMaxCDCapacity);
			}
			else
			{
				WK_TRACE_WARNING(_T("CIPCDatabaseDVClient::OnConfirmBackupByTime() falsche UserID: %i \n"), wUserData);
			}

		}
		else
		{
			WK_TRACE_WARNING(_T("CIPCDatabaseDVClient::OnConfirmBackupByTime() kein Backup mehr da  User:%i \n"), wUserData);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseDVClient::OnConfirmCancelBackup(DWORD dwUserData)
{
	CMainFrame *pMF = theApp.GetMainFrame();
	if (pMF)
	{
		CBackupWindow* pBW = pMF->GetBackupWindow();
		if(pBW)
		{
			WK_TRACE(_T("CIPCDatabaseDVClient::OnConfirmCancelBackup() UserData:%i \n"), dwUserData);
			pBW->ConfirmCancelBackup(dwUserData);
		}
	}
}
