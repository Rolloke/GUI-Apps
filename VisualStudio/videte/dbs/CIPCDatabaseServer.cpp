// CIPCDatabaseServer.cpp: implementation of the CIPCDatabaseServer class.
//
//////////////////////////////////////////////////////////////////////
// Error Codes during Reset
//  0 request info during server reset
//  1 no permission
//  2 no permission
//  3 request file list during server reset
//  4 request sequence list during server reset
//  5 request save picture during server reset
//  6 request new save picture during server reset
//  7 request new save picture for host during server reset
//  8 request save picture for host during server reset
//  9 request open file during server reset
// 10 request rec no during server reset
// 11 request record nr during server reset
// 12 request camera names
// 13 request file close
// 14 request query
// 15 responce query result
// 16 request new query
// 17 request remove file 
// 18 request delete sequence
// 19 request delete archive
// 20 request drives
// 21 request backup
// 22 request cancel backup
// 23 request field info during server reset

#include "stdafx.h"
#include "DBS.h"
#include "Mainfrm.h"
#include "CIPCDatabaseServer.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCDatabaseServer::CIPCDatabaseServer(CClient* pClient, LPCTSTR szShmName, CPermission* pPermission)
	: CIPCDatabase(szShmName, TRUE)
{
	m_pClient = pClient;
	m_dwClientVersion = 0;

	m_wLastRequestArchiveNr = 0;
	m_wLastRequestSequenceNr = 0;
	m_dwLastRequestRecordNr = 0;

	m_bArchiveRequests = FALSE;
	m_bAlarmListRequests = FALSE;
	m_bDriveRequests = FALSE;
	m_bFieldRequests = FALSE;

	if (pPermission)
	{
		m_pPermission = new CPermission(*pPermission);
	}
	else
	{
		m_pPermission = NULL;
	}
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCDatabaseServer::~CIPCDatabaseServer()
{
	StopThread();
	WK_DELETE(m_pPermission)
	if (WK_IS_WINDOW(theApp.m_pMainWnd))
	{
		PostMessage(theApp.m_pMainWnd->m_hWnd,WM_COMMAND,ID_FILE_ACTUALIZE,0);
	}
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDatabaseServer::IsAllowed(WORD wArchivNr)
{
	if (m_pPermission)
	{
		DWORD dwPerm = m_pPermission->GetArchivFlags(CSecID(SECID_GROUP_ARCHIVE,wArchivNr));
		if (dwPerm & WK_ALLOW_PICTURE)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDatabaseServer::IsDataAllowed(WORD wArchivNr)
{
	if (m_pPermission)
	{
		DWORD dwPerm = m_pPermission->GetArchivFlags(CSecID(SECID_GROUP_ARCHIVE,wArchivNr));
		if (dwPerm & WK_ALLOW_DATA)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
BOOL CIPCDatabaseServer::IsDeleteAllowed(WORD wArchivNr)
{
	if (m_pPermission)
	{
		DWORD dwPerm = m_pPermission->GetArchivFlags(CSecID(SECID_GROUP_ARCHIVE,wArchivNr));
		if (dwPerm & WK_ALLOW_DELETE)
		{
			return TRUE;
		}
	}
	return FALSE;
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestVersionInfo(WORD wGroupID)
{
	// Nr 1 is default
	// Nr 2 is Version 3.6 Build 133
	TRACE(_T("OnRequestVersionInfo confirming 3\n"));
	DoConfirmVersionInfo(0,3);
	TRACE(_T("requesting client version\n"));
	DoRequestVersionInfo(0);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
	m_dwClientVersion = dwVersion;
	TRACE(_T("client version is %d\n"),m_dwClientVersion);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::ArchiveToArchiveRecord(CArchiv& arc, CIPCArchivRecord& rec)
{
	rec.SetName(arc.GetName());
	rec.SetID(arc.GetNr());

	BYTE bFlags = 0;
	switch (arc.GetType())
	{
	case ALARM_ARCHIV:
		bFlags |= CAR_IS_ALARM;
		break;
	case RING_ARCHIV:
		bFlags |= CAR_IS_RING;
		break;
	case SICHERUNGS_RING_ARCHIV:
		bFlags |= CAR_IS_SAFE_RING;
		break;
	case SUCHERGEBNIS_ARCHIV:
		bFlags |= CAR_IS_SEARCH;
		break;
	case BACKUP_ARCHIV:
		bFlags |= CAR_IS_BACKUP;
		break;
	case ALARM_LIST_ARCHIV:
		bFlags |= CAR_IS_ALARM_LIST;
		break;
	}
	if (arc.IsSearch())
	{
		bFlags |= CAR_IS_SEARCH;
	}
	rec.SetFlags(bFlags);
	rec.SetSizeInMB(arc.GetSizeMB());
	rec.SetBenutzterPlatz(arc.GetSizeBytes().GetInt64());
}
//////////////////////////////////////////////////////////////////////
// send list of all archivs
void CIPCDatabaseServer::OnRequestInfo()
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
						SECID_NO_ID, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_ARCHIVE_DURING_RESET);
		return;
	}

	theApp.m_Archives.Lock(_T(__FUNCTION__));

	CArchiv* pArchiv;
	int i,c;
	int iNumRecords = 0;
	CIPCArchivRecord *pData = NULL;
	
	c = theApp.m_Archives.GetSize();

	if (m_pPermission && c>0)
	{
		for (i=0; i<c; i++)
		{
			pArchiv = theApp.m_Archives.GetAtFast(i);
			if (  (!pArchiv->IsAlarmList() || m_bAlarmListRequests)
				&& IsAllowed(pArchiv->GetNr()))
			{
				iNumRecords++;
			}
		}
	}
	
	if (iNumRecords>0)
	{
		int nCurRecord = 0;

		pData = new CIPCArchivRecord[iNumRecords];
									   
		for (i=0; i<c; i++)
		{
			pArchiv = theApp.m_Archives.GetAtFast(i);
			if (  (!pArchiv->IsAlarmList() || m_bAlarmListRequests)
				&& IsAllowed(pArchiv->GetNr()))
			{
				ArchiveToArchiveRecord(*pArchiv,pData[nCurRecord++]);
			}
		}
	}

	theApp.m_Archives.Unlock();

	if (pData!=NULL)
	{
		DoConfirmInfo(iNumRecords, pData);
	}

	WK_DELETE_ARRAY(pData);
	m_bArchiveRequests = TRUE;

	if (WK_IS_WINDOW(theApp.m_pMainWnd))
	{
		PostMessage(theApp.m_pMainWnd->m_hWnd,WM_COMMAND,ID_FILE_ACTUALIZE,0);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestBackupInfo()
{
	// do nothing just prevent from 
	// cmd DB_REQUEST_BACKUP_INFO not overridden
	// message
}
//////////////////////////////////////////////////////////////////////
// send list of all fields
void CIPCDatabaseServer::OnRequestFieldInfo()
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
						SECID_NO_ID, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_FIELD_INFO_DURING_RESET);
		return;
	}
	DoConfirmFieldInfo(theApp.m_Fields);
	m_bFieldRequests = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::SequenceToSequenceRecord(const CSequence& seq, CIPCSequenceRecord& rec)
{
	BYTE bFlags = seq.GetFlags();
	if (seq.IsBackup())
	{
		bFlags |= CAR_IS_BACKUP;
		if (!seq.IsRO())
		{
			bFlags |= CAR_CAN_DELETE;
		}
	}
	else
	{
		if ((!seq.IsRO()) 
			&& IsDeleteAllowed(seq.GetArchivNr()))
		{
			bFlags |= CAR_CAN_DELETE;
		}
	}
	rec.SetFlags(seq.GetFlags());
	CString s = seq.GetName();
	if (theApp.IsIDIP())
	{
		if (s.IsEmpty())
		{
			if (   m_pClient->GetVersion().IsEmpty()
				|| m_pClient->GetVersion()<"4.3.1"
				)
			{
				s = seq.GetFirstTime().GetDateTime();
			}
		}
	}
	rec.SetName(s);
	rec.SetArchiveNr(seq.GetArchivNr());
	rec.SetSequenceNr(seq.GetNr());
	rec.SetFlags(bFlags);
	rec.SetNrOfPictures(seq.GetNrOfPictures());
	rec.SetSize(seq.GetSizeBytes());
	rec.SetTime(seq.GetFirstTime());
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestSequenceList(WORD wArchivNr)
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
		DoIndicateError(GetLastCmd(), 
						SECID_NO_ID, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_SEQUENCE_DURING_RESET);
		return;
	}
	CArchiv* pArchiv;
	pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);
	if (pArchiv)
	{
		int i,n;
		CIPCSequenceRecord* pData = NULL;
		if (   !pArchiv->IsAlarmList()
			|| m_bAlarmListRequests)
		{
			pArchiv->Lock(_T(__FUNCTION__));
			n = pArchiv->GetNrSequences();
			if (n>0)
			{
				pData = new CIPCSequenceRecord[n];
				for (i=0; i<n; i++)
				{
					CSequence* pSequence = pArchiv->GetSequenceIndex(i);
					if (pSequence)
					{
						SequenceToSequenceRecord(*pSequence,pData[i]);
					}
					else
					{
						WK_TRACE_ERROR(_T("sequence doesn't exist for client request %s\n"),
							pArchiv->GetName());
					}
				}
			}
			pArchiv->Unlock();
			//TRACE(_T("confirm slist %s %d %d\n"),pArchiv->GetName(),n,m_bAlarmListRequests);
			DoConfirmSequenceList(wArchivNr, n, pData);
			WK_DELETE_ARRAY(pData);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestSavePicture(int numArchives,
											  const BYTE archiveIDs[],
											  const CIPCPictureRecord &data)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
						SECID_NO_ID, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_SAVE_DURING_RESET,	
						NULL);
		return;
	}
	CArchiv* pArchiv;
	WORD wArchivNr;
	BOOL bFound = FALSE;
	
	for (int i=0;i<numArchives;i++)
	{
		wArchivNr = archiveIDs[i];
		pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);
		if (pArchiv)
		{
			theApp.m_SaveThread.AddSaveData(new CSaveData(pArchiv,data));
			bFound = TRUE;
		}
	}
	if (!bFound)
	{
		WK_TRACE(_T("no archive found to save picture\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestNewSavePicture(WORD wArchivNr1,
												 WORD wArchivNr2,
			 									 const CIPCPictureRecord &pict,
												 int iNumRecords,
												 const CIPCField fields[])
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr1);
		DoIndicateError(GetLastCmd(), id, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_SAVE_DURING_RESET,	NULL);
		return;
	}

	WORD wA[2];

	wA[0] = wArchivNr1;
	wA[1] = wArchivNr2;

	for (int i=0;i<2;i++)
	{
		if (wA[i]>0)
		{
			CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wA[i]);
			if (pArchiv)
			{
				if (   pict.GetDataLength() > 0
					&& pict.GetData())
				{
					theApp.m_SaveThread.AddSaveData(new CSaveData(pArchiv,pict,iNumRecords,fields));
				}
				else
				{
					WK_TRACE(_T("no picture to save picture %d\n"),wA[i]);
				}
			}
			else
			{
				WK_TRACE(_T("no archive found to save picture %d\n"),wA[i]);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestSaveMedia(WORD wArchiveNr,
											CSecID hostID,
											BOOL	bIsAlarmConnection,
											BOOL	bIsSearchResult,
			 								const CIPCMediaSampleRecord &media,
											int iNumRecords,
											const CIPCField fields[])
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchiveNr);
		DoIndicateError(GetLastCmd(), id, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_SAVE_DURING_RESET,	NULL);
		return;
	}
	CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchiveNr);

	if (pArchiv == NULL)
	{
		// may be a host specific archive?
		CSecID id = GetAndCreateArchiveForHost(hostID,bIsAlarmConnection,bIsSearchResult);
		if (id != SECID_NO_ID)
		{
			WORD wArchivNr = id.GetSubID();
			pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);
		}
	}

	if (pArchiv)
	{
		theApp.m_SaveThread.AddSaveData(new CSaveData(pArchiv,media,iNumRecords,fields));
	}
	else
	{
		WK_TRACE_ERROR(_T("no archive found to save media %d\n"),wArchiveNr);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestSaveData(WORD wArchivNr,
											int iNumRecords,
											const CIPCField fields[])
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
		DoIndicateError(GetLastCmd(), id, 
			CIPC_ERROR_ACCESS_DENIED, 
			DBEC_SAVE_DURING_RESET,	NULL);
		return;
	}
	CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);
	if (pArchiv)
	{
		theApp.m_SaveThread.AddSaveData(new CSaveData(pArchiv,iNumRecords,fields));
	}
	else
	{
		WK_TRACE(_T("no archive found to save data %d\n"),wArchivNr);
	}
}
//////////////////////////////////////////////////////////////////////
DWORD CIPCDatabaseServer::GetVersion()
{
	return m_dwClientVersion;
}
//////////////////////////////////////////////////////////////////////
CSecID CIPCDatabaseServer::GetAndCreateArchiveForHost(CSecID hostID,
													  BOOL	bIsAlarmConnection,
													  BOOL	bIsSearchResult)
{
	theApp.LoadHosts();
	CHost* pHost = theApp.GetHosts().GetHost(hostID);
	CSecID id;

	if (pHost)
	{
		if (bIsAlarmConnection)
		{
			id = pHost->GetAlarmArchive();
		}
		else if (bIsSearchResult)
		{
			if (hostID==SECID_LOCAL_HOST)
			{
				id = CSecID(SECID_GROUP_ARCHIVE,255);
			}
			else
			{
				id = pHost->GetSearchArchive();
			}
		}
		else
		{
			id = pHost->GetNormalArchive();
		}

		CArchiv* pArchiv = theApp.m_Archives.GetArchiv(id.GetSubID());
		if (   pArchiv == NULL
			&& id != SECID_NO_ID)
		{
			WK_TRACE(_T("archive for host does not exist reset id\n"));
			id = SECID_NO_ID;
		}

		if (id == SECID_NO_ID)
		{
			if (theApp.CreateArchiveForHost(hostID,bIsAlarmConnection,bIsSearchResult))
			{
				pHost = theApp.GetHosts().GetHost(hostID);
				if (bIsAlarmConnection)
				{
					id = pHost->GetAlarmArchive();
				}
				else if (bIsSearchResult)
				{
					if (hostID==SECID_LOCAL_HOST)
					{
						id = CSecID(SECID_GROUP_ARCHIVE,255);
					}
					else
					{
						id = pHost->GetSearchArchive();
					}
				}
				else
				{
					id = pHost->GetNormalArchive();
				}
			}
		}
	}

	return id;
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestNewSavePictureForHost(CSecID hostID,
												   BOOL	bIsAlarmConnection,
												   BOOL	bIsSearchResult,
			 									   const CIPCPictureRecord &pict,
												   int iNumRecords,
												   const CIPCField fields[])
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
						hostID, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_SAVE_DURING_RESET);
		return;
	}
	CSecID id = GetAndCreateArchiveForHost(hostID,bIsAlarmConnection,bIsSearchResult);

	if (id != SECID_NO_ID)
	{
		WORD wArchivNr = id.GetSubID();
		CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);

		if (pArchiv)
		{
			theApp.m_SaveThread.AddSaveData(new CSaveData(pArchiv,pict,iNumRecords,fields));
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("OnRequestNewSavePictureForHost no host found %08lx\n"),
															hostID.GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestSavePictureForHost(
							CSecID	hostID,
							const	CIPCPictureRecord &data,
							BOOL	bIsAlarmConnection,
							BOOL	bIsSearchResult
							)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
						hostID, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_SAVE_DURING_RESET);
		return;
	}
	CHost* pHost = theApp.GetHosts().GetHost(hostID);
	if (pHost)
	{
		CSecID id;

		if (bIsAlarmConnection)
		{
			id = pHost->GetAlarmArchive();
		}
		if (bIsSearchResult)
		{
			if (hostID==SECID_LOCAL_HOST)
			{
				id = CSecID(SECID_GROUP_ARCHIVE,255);
			}
			else
			{
				id = pHost->GetSearchArchive();
			}
		}
		if (!bIsSearchResult && !bIsAlarmConnection)
		{
			id = pHost->GetNormalArchive();
		}

		CArchiv* pArchiv = theApp.m_Archives.GetArchiv(id.GetSubID());
		if (   pArchiv == NULL
			&& id != SECID_NO_ID)
		{
			WK_TRACE(_T("archive for host does not exist reset id\n"));
			id = SECID_NO_ID;
		}
		if (id == SECID_NO_ID)
		{
			if (theApp.CreateArchiveForHost(hostID,bIsAlarmConnection,bIsSearchResult))
			{
				pHost = theApp.GetHosts().GetHost(hostID);
				if (bIsAlarmConnection)
				{
					id = pHost->GetAlarmArchive();
				}
				else if (bIsSearchResult)
				{
					if (hostID==SECID_LOCAL_HOST)
					{
						id = CSecID(SECID_GROUP_ARCHIVE,255);
					}
					else
					{
						id = pHost->GetSearchArchive();
					}
				}
				else
				{
					id = pHost->GetNormalArchive();
				}
			}
		}
		if (id != SECID_NO_ID)
		{
			WORD wArchivNr = id.GetSubID();
			CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);

			if (pArchiv)
			{
				theApp.m_SaveThread.AddSaveData(new CSaveData(pArchiv,data));
			}
		}
		else
		{
			WK_TRACE(_T("TODO: OnRequestSavePictureForHost no archive found\n"));
		}
	}
	else
	{
		WK_TRACE(_T("OnRequestSavePictureForHost no host found %08lx\n"),hostID.GetID());
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestDisconnect()
{
	DelayedDelete();
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::SetLastConfirm(DWORD dwRecordNr)
{
	m_dwLastRequestRecordNr = dwRecordNr;
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestRecordNr(WORD  wArchivNr, 
										   WORD  wSequenceNr, 
										   DWORD dwCurrentRecordNr,
										   DWORD dwNewRecordNr,
										   DWORD dwCamID)
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
		DoIndicateError(GetLastCmd(), id, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_RECORD_NR_DURING_RESET, NULL);
		return;
	}
	if (theApp.TraceRequests())
	{
		WK_TRACE(_T("OnRequestRecNo A=%d S=%d CR=%d NR=%d C=%08lx\n"),
			wArchivNr,wSequenceNr,dwCurrentRecordNr,
			dwNewRecordNr,dwCamID);
	}
	
	CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);
	CSequence* pSequence = NULL;
	if (pArchiv)
	{
		pSequence = pArchiv->GetSequenceID(wSequenceNr);
		if (pSequence)
		{
			pSequence->NewConfirmRecord(dwCurrentRecordNr,
									    dwNewRecordNr,
									    dwCamID,
									    this);
		}
		else
		{
			WK_TRACE_ERROR(_T("OnRequestRecNo no sequence\n"));
			DWORD dwID = MAKELONG(wSequenceNr,wArchivNr);
			DoIndicateError(dwID,dwNewRecordNr,
				CIPC_ERROR_INVALID_VALUE,DBEC_RECORD_NR_NO_SEQUENCE);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("OnRequestRecNo no archive\n"));
		DWORD dwID = MAKELONG(wSequenceNr,wArchivNr);
		DoIndicateError(dwID,dwNewRecordNr,
			CIPC_ERROR_INVALID_VALUE,DBEC_RECORD_NR_NO_ARCHIVE);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestCameraNames(WORD wArchivNr,DWORD dwUserData)
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
		DoIndicateError(GetLastCmd(), id, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_CAMERA_NAMES_DURING_RESET,	NULL);
		return;
	}
	if (theApp.TraceRequests())
	{
		WK_TRACE(_T("OnRequestCameraNames A=%d\n"),wArchivNr);
	}

	CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);
	if (pArchiv)
	{
		pArchiv->DoConfirmCameraNames(this,dwUserData);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestQuery(DWORD dwUserID, const CString& sQuery)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_QUERY_DURING_RESET, NULL);
		return;
	}
	if (theApp.TraceRequests())
	{
		WK_TRACE(_T("OnRequestQuery <%s> <%s>\n"),m_pClient->GetName(),sQuery);
	}
	theApp.m_Queries.SafeAddTail(new CQuery(dwUserID,m_pClient->GetID(),sQuery));
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER,WPARAM_REQUEST_QUERY);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnResponseQueryResult(DWORD dwUserID)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_QUERY_DURING_RESET, NULL);
		return;
	}
	CQueryThread* pQueryThread = NULL;
	for (int i=0;i<theApp.m_QueryThreads.GetSize();i++)
	{
		pQueryThread = theApp.m_QueryThreads.GetAtFast(i);
		if (pQueryThread->GetQuery()->GetClientID() == m_pClient->GetID())
		{
			if (dwUserID==0)
			{
				// search is to be cancelled
				//TRACE(_T("TKR ### OnResponseQueryResult()  ID: 0x%x\n"),dwUserID);
				pQueryThread->Cancel();
			}
			else
			{
				//
				pQueryThread->GetQuery()->IncreaseResponses();
			}
			break;
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestNewQuery(DWORD dwUserID,
									  int iNumArchives,
									  const WORD archives[],
									  int iNumFields,
									  const CIPCField fields[],
									  DWORD dwMaxQueryResults/*=0*/)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_QUERY_DURING_RESET, NULL);
		return;
	}
	if (theApp.TraceRequests())
	{
		WK_TRACE(_T("OnRequestNewQuery <%s>\n"),m_pClient->GetName());
	}
	theApp.m_Queries.SafeAddTail(new CQuery(dwUserID,
									  m_pClient->GetID(),
									  iNumArchives,
									  archives,
									  iNumFields,
									  fields,
									  dwMaxQueryResults));
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER,WPARAM_REQUEST_QUERY);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
						CSecID(SECID_GROUP_ARCHIVE,wArchivNr), 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_DELETE_SEQUENCE_DURING_RESET);
		return;
	}
	if (IsDeleteAllowed(wArchivNr))
	{
		CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);
		BOOL bDeleted = pArchiv->DeleteSequence(wSequenceNr);
		DoIndicateDeleteSequence(wArchivNr, wSequenceNr);
		if(!bDeleted)
		{	
			WK_TRACE_ERROR(_T("OnRequestDeleteSequence: %s"), GetLastErrorString());
		}
	}
	else
	{
		CString s;
		s.LoadString(IDS_DELETE_SEQUENCE_DENIED);
		DoIndicateError(GetLastCmd(),
						CSecID(SECID_GROUP_ARCHIVE,wArchivNr),
						CIPC_ERROR_ACCESS_DENIED,
						DBEC_DELETE_SEQUENCE_DENIED,s);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestDeleteArchiv(WORD wArchivNr)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), CSecID(SECID_GROUP_ARCHIVE,wArchivNr), CIPC_ERROR_ACCESS_DENIED, 19, NULL);
		return;
	}
	if (   m_pPermission 
		&& (m_pPermission->GetFlags() & WK_ALLOW_BACKUP))
	{
		if (theApp.m_Archives.DeleteArchive(wArchivNr))
		{
			WK_TRACE(_T("archive %d deleted\n"),wArchivNr);
			return;
		}
		else
		{
			WK_TRACE_ERROR(_T("cannot delete archive %d\n"),wArchivNr);
			CString s;
			s.Format(IDP_CANNOT_DELETE_ARCHIV,wArchivNr);
			DoIndicateError(GetLastCmd(),
							CSecID(SECID_GROUP_ARCHIVE,wArchivNr),
							CIPC_ERROR_ARCHIVE,DBEC_ARCHIVE_DELETE_FAILED,s);
		}
	}
	else
	{
		CString s;
		s.LoadString(IDS_DELETE_ARCHIVE_DENIED);
		DoIndicateError(GetLastCmd(),
						CSecID(SECID_GROUP_ARCHIVE,wArchivNr),
						CIPC_ERROR_ACCESS_DENIED,
						DBEC_ARCHIVE_DELETE_DENIED,s);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestDrives()
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
						SECID_NO_ID, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_DRIVES_DURING_RESET);
		return;
	}
	if (theApp.TraceRequests())
	{
		WK_TRACE(_T("<%s> requests drives\n"),m_pClient->GetName());
	}
	theApp.m_Drives.CheckSpace();
	theApp.m_Drives.Lock();
	DoConfirmDrives(theApp.m_Drives);
	theApp.m_Drives.Unlock();
	m_bDriveRequests = TRUE;
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestBackup(DWORD dwUserData,
										 const CString& sDestinationPath,
										 const CString& sName,
										 int iNumIDs,
										 const DWORD dwIDs[])
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
						dwUserData, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_BACKUP_DURING_RESET);
		return;
	}
	if (theApp.TraceRequests())
	{
		WK_TRACE(_T("OnRequestBackup %d\n"),dwUserData);
	}
	theApp.m_Backups.SafeAddTail(new CBackup(dwUserData,
											 m_pClient->GetID(),
											 sDestinationPath,
											 sName,
											 iNumIDs,
											 dwIDs
											 ));
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestBackupByTime(WORD  wUserData,
											    const CString& sDestinationPath,
											    const CString& sName,
											    int iNumIDs,
											    const DWORD dwIDs[],
											    const CSystemTime& start,
											    const CSystemTime& end,
											    WORD  wFlags,
												DWORD dwMaximumInMB)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), SECID_NO_ID, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_BACKUP_DURING_RESET, NULL);
		return;
	}
	CString sIDs,sID;
	for (int i=0;i<iNumIDs;i++)
	{
		sID.Format(_T("%08x "),dwIDs[i]);
		sIDs += sID;
	}
	WK_TRACE(_T("OnRequestBackupByTime ID=%d <%s>-<%s> IDs=%d, %s FLAGS=%x max MB=%d\n"),
		wUserData,start.GetDateTime(),end.GetDateTime(),iNumIDs, sIDs,wFlags,dwMaximumInMB);
	theApp.m_Backups.SafeAddTail(new CBackup(wUserData,
											 m_pClient->GetID(),
											 sDestinationPath,
											 sName,
											 iNumIDs,
											 dwIDs,
											 start,
											 end,
											 wFlags,
											 dwMaximumInMB
											 ));
	CWnd* pWnd = AfxGetMainWnd();
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER,WPARAM_REQUEST_BACKUP);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestCancelBackup(DWORD dwUserData)
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
						dwUserData, 
						CIPC_ERROR_ACCESS_DENIED, 
						DBEC_BACKUP_DURING_RESET);
		return;
	}
	if (theApp.TraceRequests())
	{
		WK_TRACE(_T("OnRequestCancelBackup %d\n"),dwUserData);
	}
	theApp.CancelBackup(dwUserData);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestGetValue(CSecID id,
										   const CString &sKey,
										   DWORD dwServerData)
{
	if (id == SECID_NO_ID)
	{
		if (0 == sKey.CompareNoCase(CSD_IS_DV))
		{
			DoConfirmGetValue(id,sKey,CSD_OFF,dwServerData);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestRecords(WORD  wArchivNr, 
										   WORD  wSequenceNr, 
										   DWORD dwFirstRecordNr, // 0 for all
										   DWORD dwLastRecordNr) // 0 for all
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
		DoIndicateError(GetLastCmd(), id, 
			CIPC_ERROR_ACCESS_DENIED, 
			DBEC_RECORD_NR_DURING_RESET, NULL);
		TRACE(_T("DoIndicateError CMD=%08lx, id=%08lx\n"),GetLastCmd(),id.GetID());
		return;
	}
	CArchiv* pArchive = theApp.m_Archives.GetArchiv(wArchivNr);
	CSequence* pSequence = NULL;
	if (pArchive)
	{
		pSequence = pArchive->GetSequenceID(wSequenceNr);
		if (pSequence)
		{
			CIPCFields fields;
			CIPCFields records;

			pSequence->GetFieldsAndRecords(dwFirstRecordNr,dwLastRecordNr,fields,records);
			DoConfirmRecords(wArchivNr,wSequenceNr,fields,records);
		}
		else
		{
			WK_TRACE_ERROR(_T("OnRequestRecords no Sequence A=%04hx S=%d\n"),wArchivNr,wSequenceNr);
			DWORD dwID = MAKELONG(wSequenceNr,wArchivNr);
			DoIndicateError(dwID,SECID_NO_ID,CIPC_ERROR_INVALID_VALUE,
				DBEC_RECORD_NR_NO_SEQUENCE);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("OnRequestRecords no archive A=%04hx\n"),wArchivNr);
		DWORD dwID = MAKELONG(wSequenceNr,wArchivNr);
		DoIndicateError(dwID,SECID_NO_ID,
			CIPC_ERROR_INVALID_VALUE,DBEC_RECORD_NR_NO_ARCHIVE);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestOpenSequence(WORD wArchiveNr, WORD wSequenceNr)
{
	CArchiv* pArchive = theApp.m_Archives.GetArchiv(wArchiveNr);
	CSequence* pSequence = NULL;
	if (pArchive)
	{
		pSequence = pArchive->GetSequenceID(wSequenceNr);
		if (pSequence)
		{
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestCloseSequence(WORD wArchiveNr, WORD wSequenceNr)
{
	CArchiv* pArchive = theApp.m_Archives.GetArchiv(wArchiveNr);
	CSequence* pSequence = NULL;
	if (pArchive)
	{
		pSequence = pArchive->GetSequenceID(wSequenceNr);
		if (pSequence)
		{
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestAlarmListArchives()
{
	if (theApp.IsResetting())
	{
		DoIndicateError(GetLastCmd(), 
			SECID_NO_ID, 
			CIPC_ERROR_ACCESS_DENIED, 
			DBEC_ARCHIVE_DURING_RESET,
			NULL);
		return;
	}
	theApp.m_Archives.Lock(_T(__FUNCTION__));

	CArchiv* pArchive;
	int i,c;
	CIPCArchivRecord *pData = NULL;

	c = 0;

	for (i=0;i<theApp.m_Archives.GetSize();i++)
	{
		pArchive = theApp.m_Archives.GetAtFast(i);
		if (   pArchive 
			&& pArchive->IsAlarmList()
			)
		{
			c++;
		}
	}

	if (c>0)
	{
		int j=0;
		pData = new CIPCArchivRecord[c];

		for (i=0; i<theApp.m_Archives.GetSize(); i++)
		{
			pArchive = theApp.m_Archives.GetAtFast(i);
			if (   pArchive 
				&& pArchive->IsAlarmList()
				)
			{
				ArchiveToArchiveRecord(*pArchive,pData[j++]);
			}
		}
	}

	theApp.m_Archives.Unlock();

	m_bAlarmListRequests = TRUE;
	if (pData!=NULL)
	{
		DoConfirmAlarmListArchives(c, pData);
	}

	WK_DELETE_ARRAY(pData);
	if (WK_IS_WINDOW(theApp.m_pMainWnd))
	{
		PostMessage(theApp.m_pMainWnd->m_hWnd,WM_COMMAND,ID_FILE_ACTUALIZE,0);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseServer::OnRequestAlarmListRecord(WORD  wArchivNr, 
												   WORD  wSequenceNr, 
												   DWORD dwRecordNr)
{
	if (theApp.IsResetting())
	{
		CSecID id (SECID_GROUP_ARCHIVE,wArchivNr);
		DoIndicateError(GetLastCmd(), id, 
			CIPC_ERROR_ACCESS_DENIED, DBEC_RECORD_NR_DURING_RESET);
		TRACE(_T("DoIndicateError CMD=%08lx, id=%08lx\n"),GetLastCmd(),id.GetID());
		return;
	}
	CArchiv* pArchiv = theApp.m_Archives.GetArchiv(wArchivNr);
	CSequence* pSequence = NULL;
	if (pArchiv)
	{
		pSequence = pArchiv->GetSequenceID(wSequenceNr);
		if (pSequence)
		{
			CIPCFields fields;
			pSequence->GetFields(dwRecordNr,fields);
			DoConfirmAlarmListRecord(wArchivNr,wSequenceNr,dwRecordNr,
				pSequence->GetNrOfPictures(),fields);
		}
		else
		{
			WK_TRACE_ERROR(_T("OnRequestAlarmListRecord no Sequence A=%04hx S=%d\n"),wArchivNr,wSequenceNr);
			DWORD dwID = MAKELONG(wSequenceNr,wArchivNr);
			DoIndicateError(dwID,dwRecordNr,CIPC_ERROR_INVALID_VALUE,3);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("OnRequestAlarmListRecord no archive A=%04hx\n"),wArchivNr);
		DWORD dwID = MAKELONG(wSequenceNr,wArchivNr);
		DoIndicateError(dwID,dwRecordNr,CIPC_ERROR_INVALID_VALUE,4);
	}
}
