// CIPCDatabaseIdipClient.cpp: implementation of the CIPCDatabaseIdipClient class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IdipClient.h"
#include "CIPCDatabaseIdipClient.h"

#include "mainfrm.h"
#include "IdipClientDoc.h"
#include "ViewIdipClient.h"
#include "ViewControllItems.h"
#include "ViewArchive.h"

#include "DlgBackup.h"
#include "Server.h"
#include "SyncCoolBar.h"
#include "PlayRealTime.h"
#include "CIPCAudioIdipClient.h"
#include "WndPlay.h"
#include "WndAlarmlist.h"
#include "DlgDatabaseInfo.h"
#include "DlgSearchResult.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CIPCDatabaseIdipClient, CObject)
CIPCDatabaseIdipClient::CIPCDatabaseIdipClient(LPCTSTR shmName, 
											 CServer* pServer, CSecID id, WORD wCodePage)
	: CIPCDatabaseClient(shmName)
{
	m_iCurrentArchiveRequestingSequenceList = 0;
	m_pServer = pServer;
	m_dwServerVersion = 0; // assume iST 3.0
	m_ID = id;
	m_wCodePage = wCodePage;
	m_bGotInfo = FALSE;
	m_bAlarmListRequested = FALSE;
	m_bActualizing = FALSE;
	m_dwLastActualizing = GetTickCount();
#ifdef _DEBUG
	CIPCArchivRecordArray& a = GetRecords();
	a.m_nCurrentThread = 0;
#endif
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCDatabaseIdipClient::~CIPCDatabaseIdipClient()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmConnection()
{
	if (m_wCodePage != CODEPAGE_UNICODE)			// request only, if unicode is not used
	{
		WORD wCodePage = m_wCodePage;
		m_wCodePage = CODEPAGE_UNICODE;				// request is made in unicode
		DoRequestGetValue(CSecID(), CRF_CODEPAGE);	// Do you speak Unicode?
		m_wCodePage = wCodePage;
	}
	if (   (0!=m_pServer->GetKind().CompareNoCase(_T("Mini B3")))
		&& (0!=m_pServer->GetKind().CompareNoCase(_T("TeleObserver")))
		) 
	{
		DoRequestGetValue(SECID_NO_ID,CSD_IS_DV,0);
	}
	DoRequestVersionInfo(0);
	if (m_pServer->GetMajorVersion() == 0)
	{
		DoRequestGetValue(SECID_NO_ID,_T("GetFileVersion"),0);
	}
	RefreshInfo();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmGetValue(CSecID id, 
											   const CString &sKey, 
											   const CString &sValue, 
											   DWORD dwServerData)
{
	if (sKey == CSD_IS_DV)
	{
		if (sValue == CSD_ON)
		{
			m_pServer->SetDTS(TRUE);
		}
	}
	else if (0==sKey.CompareNoCase(_T("GetFileVersion")))
	{
		m_pServer->SetVersion(sValue);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
	m_dwServerVersion = dwVersion;
	if (m_dwServerVersion>=3)
	{
		DoRequestFieldInfo();
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmAlarmListArchives(int iNumRecords, 
													   const CIPCArchivRecord data[])
{
	for (int i=0;i<iNumRecords;i++)
	{
		// just map the method to one
		// that does everything
		const CIPCArchivRecord& rec = data[i];
		BOOL bFound = FALSE;
		CIPCArchivRecords& records = GetRecords();
		for (int j=0;j<records.GetSize()&&!bFound;j++)
		{
			bFound = records.GetAtFast(j)->GetSubID() == rec.GetSubID();
		}
		if (!bFound)
		{
			OnIndicateNewArchiv(rec);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[])
{
	BOOL bGotInfo = m_bGotInfo;
	TRACE(_T("CIPCDatabaseIdipClient::OnConfirmInfo(%d)\n"), iNumRecords);

	CIPCDatabaseClient::OnConfirmInfo(iNumRecords, data);
	m_dwLastActualizing = GetTickCount();
	if (!m_bActualizing)
	{
		m_bGotInfo = TRUE;
		// post only 1st time
		if (!bGotInfo)
		{
			CViewArchive* pAV = theApp.GetMainFrame()->GetViewArchive();
			if (pAV)
			{
				pAV->PostMessage(WM_USER, MAKELONG(WPARAM_DATABASE_CONNECTED,m_pServer->GetID()));
			}
		}

		CIPCArchivRecordArray& a = GetRecords();
		a.Lock(_T(__FUNCTION__));
		if (m_iCurrentArchiveRequestingSequenceList<a.GetSize())
		{
			DoRequestSequenceList(a.GetAtFast(m_iCurrentArchiveRequestingSequenceList++)->GetID());
		}
		a.Unlock();
		DoRequestDrives();
	}
	else
	{
		CDlgDatabaseInfo* pDlg = m_pServer->GetInfoDialog();
		if (pDlg)
		{
			pDlg->PostMessage(WM_COMMAND,ID_ARCHIV_INFO);
		}

		CView *pView = (CView*)theApp.GetMainFrame()->GetViewArchive();
		if (pView)
		{
			pView->PostMessage(WM_USER, MAKELONG(WPARAM_ACTUALIZE, m_pServer->GetID()));
		}

	}
	m_bActualizing = FALSE;
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmFieldInfo(int iNumRecords, const CIPCField data[])
{
	m_pServer->OnConfirmFieldInfo(iNumRecords,data);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::ActualizeArchivInfo()
{
	if (!m_bActualizing && (GetTimeSpan(m_dwLastActualizing)>60*1000))
	{
		m_bActualizing = TRUE;
		RefreshInfo();
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmSequenceList(WORD wArchivNr, int iNumRecords, 
									   const CIPCSequenceRecord data[])
{
//	TRACE(_T("OnConfirmSequenceList %d,%d,%d to Server: %x\n"),wArchivNr,iNumRecords,m_iCurrentArchiveRequestingSequenceList, m_pServer);
	if (m_pServer)
	{
		m_pServer->AddNewSequences(iNumRecords, data);

		CViewArchive* pAV = theApp.GetMainFrame()->GetViewArchive();
		if (pAV)
		{
			pAV->PostMessage(WM_USER, MAKELONG(WPARAM_ADD_SEQUENCE,m_pServer->GetID()));
#ifndef _DEBUG
			Sleep(10);
#endif
		}

		CIPCArchivRecordArray& a = GetRecords();
		a.Lock(_T(__FUNCTION__));
		if (m_iCurrentArchiveRequestingSequenceList<a.GetSize())
		{
			DoRequestSequenceList(a.GetAtFast(m_iCurrentArchiveRequestingSequenceList++)->GetID());
		}
		else if (!m_bAlarmListRequested)
		{
			if (m_pServer->IsDTS())
			{
				DoRequestAlarmListArchives();
				m_bAlarmListRequested = TRUE;
			}
			else
			{
#ifndef _DEBUG
				if (m_pServer->GetVersion()>=_T("5.0.4"))
#endif
				{
					DoRequestAlarmListArchives();
					m_bAlarmListRequested = TRUE;
				}
			}
		}
		a.Unlock();
	}
	else
	{
		TRACE(_T("No Server\n"));
	}

}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
	AfxGetMainWnd()->PostMessage(WM_USER, MAKELONG(REMOVE_SEQUENCE, m_pServer->GetID()), MAKELONG(wArchivNr, wSequenceNr));
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnIndicateNewSequence(const CIPCSequenceRecord& data, WORD  wPrevSequenceNr, DWORD dwNrOfRecords)
{
	m_pServer->AddNewSequence(data, wPrevSequenceNr, dwNrOfRecords);
	CViewArchive* pAV = theApp.GetMainFrame()->GetViewArchive();
	if (pAV)
	{
		pAV->PostMessage(WM_USER, MAKELONG(WPARAM_ADD_SEQUENCE,m_pServer->GetID()));
#ifndef _DEBUG
		Sleep(10);
#endif
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmCameraNames(WORD wArchivNr,
									  DWORD dwUserData,
									  int iNumFields,
									  const CIPCField fields[])
{
	WORD wSequenceNr = HIWORD(dwUserData);
	CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
	if (pViewIdipClient)
	{
		CWndPlay* pDW = pViewIdipClient->GetWndPlay(m_pServer->GetID(),
									CSecID(wArchivNr,wSequenceNr));
		if (pDW)
		{
			pDW->CameraNames(iNumFields,fields);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmRecordNr(WORD wArchivNr, WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCPictureRecord &pict,
									  int iNumFields,
									  const CIPCField fields[])
{
//	TRACE(_T("OnConfirmRecordNr %d,%d,%d\n"), wArchivNr,wSequenceNr, dwRecordNr);

	CViewArchive* pViewArchive = theApp.GetMainFrame()->GetViewArchive();
	if (pViewArchive==NULL)
	{
		return;
	}
	CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
	if (pViewIdipClient==NULL)
	{
		return;
	}
	CIdipClientDoc* pDoc = pViewArchive->GetDocument();

	CDlgSearchResult* pSRD = NULL;
	BOOL bShowQueryResults = FALSE; //wenn nach einer Suche Bilder kopiert werden sollen,	
									//dann in PictureData() die Variable für das
									//aktuell angezeigte Bild korrekt setzen

	CIPCFields fieldscopy;
	fieldscopy.FromArray(iNumFields,fields);
	
	pSRD = pViewArchive->GetDlgSearchResult();

	BOOL bIsQueryResultToCopy = FALSE;

	if (theApp.GetQueryParameter().CopyPictures())
	{
		bIsQueryResultToCopy = m_pServer->CopyNextQueryResult(wArchivNr,wSequenceNr,dwRecordNr);
		if (bIsQueryResultToCopy)
		{
			CIPCDatabaseIdipClientLocal* pDatabase = pDoc->GetLocalDatabase();
			if (   pDatabase
				&& pDatabase->GetIPCState() == CIPC_STATE_CONNECTED)
			{

				if (pSRD)
				{
					CString sSearchName = pSRD->GetSearchName();
					sSearchName.Replace(_T("\n"),_T(""));
					sSearchName.Replace(_T("\r"),_T(""));
					fieldscopy.SafeAdd(new CIPCField(CIPCField::m_sfSqNm,
														sSearchName,
														(WORD)sSearchName.GetLength(),
														_T('C'))
										);
					pSRD->PostMessage(WM_COMMAND,ID_SEARCH_RESULT_COPY);
				}

				pDatabase->DoRequestNewSavePictureForHost(m_pServer->GetHostID(),
															FALSE,
															TRUE,
															pict,
															fieldscopy);
				
			}
			else
			{
				WK_TRACE_ERROR(_T("local database not connected\n"));
			}
			bShowQueryResults = theApp.GetQueryParameter().ShowPictures();
		}
		else
		{
			//TRACE(_T("record confirmed not to copy\n"));
		}
	}

	if (pViewIdipClient)
	{
		CWndPlay* pDW = pViewIdipClient->GetWndPlay(m_pServer->GetID(),wArchivNr,wSequenceNr);
		if (pDW)
		{
			CPlayRealTime* pPlayRealTime = pDW->GetPlayRealTime();

			if(   !pDW->ContainsAudio() 
			   && theApp.CanPlayRealtime() 
			   && pPlayRealTime
			   && pDW->IsQueryRectEmpty())
			{
				//Play Realtime
				if(    pDW->GetPlayStatus() == PS_PLAY_FORWARD
					|| pDW->GetPlayStatus() == PS_PLAY_BACK)
				{
					BOOL bPlayNextPicture = FALSE;

					bPlayNextPicture = pPlayRealTime->CheckRealTimePicture(wArchivNr,
																		   wSequenceNr,
															  			   dwRecordNr,
																		   dwNrOfRecords,
																		   pict,
																		   iNumFields,
																		   fields,
																		   pDW->m_hWnd);				
					if(bPlayNextPicture)
					{
						pViewIdipClient->PlayPictureData(dwRecordNr, dwNrOfRecords, pict, pDW, fieldscopy, bShowQueryResults, m_pServer->GetID(), wArchivNr, wSequenceNr);
//						pDW->PictureData(dwRecordNr,dwNrOfRecords, pict, fieldscopy,bShowQueryResults);
					}
				}
				else
				{
					pViewIdipClient->PlayPictureData(dwRecordNr, dwNrOfRecords, pict, pDW, fieldscopy, bShowQueryResults, m_pServer->GetID(), wArchivNr, wSequenceNr);
//					pDW->PictureData(dwRecordNr,dwNrOfRecords, pict, fieldscopy,bShowQueryResults);
				}
			}
			else //kein Realtime
			{
				pViewIdipClient->PlayPictureData(dwRecordNr, dwNrOfRecords, pict, pDW, fieldscopy, bShowQueryResults, m_pServer->GetID(), wArchivNr, wSequenceNr);
//				pDW->PictureData(dwRecordNr, dwNrOfRecords, pict, fieldscopy,bShowQueryResults);
			}
		}
		else
		{
			if (bShowQueryResults)
			{
				// kein Playwindow fuer PictureRecord Suche ?
//				TRACE(_T("kein Playwindow fuer PictureRecord Suche\n"));
				CWndPlay *pDW = pViewIdipClient->GetWndPlay(m_pServer->GetID(),wArchivNr,0);
				if (pDW)
				{
					// selber Server, selbes Archiv
					if (pDW->GetSequenceNr() != wSequenceNr)
					{
						CIPCSequenceRecord* pSRNext = m_pServer->GetCIPCSequenceRecord(wArchivNr,wSequenceNr);
						pDW->SetNewSequence(pSRNext);
					}
					pViewIdipClient->PlayPictureData(dwRecordNr, dwNrOfRecords, pict, pDW, fieldscopy, bShowQueryResults, m_pServer->GetID(), wArchivNr, wSequenceNr);
//					pDW->PictureData(dwRecordNr, dwNrOfRecords, pict, fieldscopy,bShowQueryResults);
				}
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmRecordNr(WORD wArchivNr, 
									  WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCMediaSampleRecord &media,
									  int iNumFields,
									  const CIPCField fields[])
{
	CViewArchive* pViewArchive = theApp.GetMainFrame()->GetViewArchive();
	if (pViewArchive==NULL)
	{
		return;
	}
	CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
	if (pViewIdipClient==NULL)
	{
		return;
	}
	CIdipClientDoc* pDoc = pViewArchive->GetDocument();
	CDlgSearchResult* pSRD = pViewArchive->GetDlgSearchResult();

	if (theApp.GetQueryParameter().CopyPictures())
	{
		if (m_pServer->IsQueryResult(wArchivNr,wSequenceNr,dwRecordNr))
		{
			CIPCDatabaseIdipClientLocal* pDatabase = pDoc->GetLocalDatabase();
			if (   pDatabase
				&& pDatabase->GetIPCState() == CIPC_STATE_CONNECTED)
			{
				CIPCFields fieldscopy;
				fieldscopy.FromArray(iNumFields,fields);

				if (pSRD)
				{
					CString sSearchName = pSRD->GetSearchName();
					sSearchName.Replace(_T("\n"),_T(""));
					sSearchName.Replace(_T("\r"),_T(""));
					fieldscopy.SafeAdd(new CIPCField(CIPCField::m_sfSqNm,
														sSearchName,
														(WORD)sSearchName.GetLength(),
														_T('C'))
										);
				}

				pDatabase->DoRequestSaveMedia(0,
												m_pServer->GetHostID(),
												FALSE,
												TRUE,
			 									media,
												fieldscopy);
			}
			if (m_pServer->CopyNextQueryResult(wArchivNr,wSequenceNr,dwRecordNr))
			{
				if (pSRD)
				{
					pSRD->PostMessage(WM_COMMAND,ID_SEARCH_RESULT_COPY);
				}
			}
		}
	}

	CWndPlay* pDW = pViewIdipClient->GetWndPlay(m_pServer->GetID(),CSecID(wArchivNr,wSequenceNr));
	if (pDW)
	{
		pDW->AudioData(dwRecordNr, dwNrOfRecords, media, iNumFields, fields);
	}
}
//////////////////////////////////////////////////////////////////////
CIPCArchivRecord* CIPCDatabaseIdipClient::GetArchive(WORD wArchiv)
{
	int i,c;
	CIPCArchivRecord* pRecord = NULL;
	CIPCArchivRecordArray& a = GetRecords();

	a.Lock(_T(__FUNCTION__));
	c = a.GetSize();
	for (i=0;i<c;i++)
	{
		if (a.GetAtFast(i)->GetSubID() == wArchiv)
		{
			pRecord = a.GetAtFast(i);
			break;
		}
	}
	a.Unlock();
	return pRecord;
}
//////////////////////////////////////////////////////////////////////
CString	CIPCDatabaseIdipClient::GetArchiveName(WORD wArchiv)
{
	int i,c;
	CString s;
	CIPCArchivRecordArray& a = GetRecords();

	a.Lock(_T(__FUNCTION__));
	c = a.GetSize();
	for (i=0;i<c;i++)
	{
		if (a.GetAtFast(i)->GetSubID() == wArchiv)
		{
			s = a.GetAtFast(i)->GetName();
			break;
		}
	}
	a.Unlock();
	return s;
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnIndicateError(DWORD dwCmd, 
											CSecID id, 
											CIPCError error,
											int iErrorCode,
											const CString &sErrorMessage)
{
	if (error == CIPC_ERROR_ARCHIVE_SEARCH)
	{
		switch (iErrorCode)
		{
		case SEARCH_SUCCESS:
		case SEARCH_FAILED_NO_OPEN_FILES:
			OnConfirmQuery(dwCmd);
			break;
		case SEARCH_CANCELED:
			WK_TRACE(_T("search canceled\n"));
			break;
		default:
			break;
		}
	}
	else if (error == CIPC_ERROR_ARCHIVE_LOCAL_SEARCH)
	{
		// Indiziert, daß aufgrund einer lokalen Suche ein Bild gefunden wurde
		// iErrorCode entspricht dwUserID
		DoResponseQueryResult((DWORD)iErrorCode);

		CViewArchive* pViewArchive = theApp.GetMainFrame()->GetViewArchive();
		if (pViewArchive)
		{
			CDlgSearchResult* pSRD = pViewArchive->GetDlgSearchResult();
			if (pSRD)
			{
				pSRD->PostMessage(WM_COMMAND,ID_SEARCH_RESULT);
			}
		}
	}
	else if (error == CIPC_ERROR_ACCESS_DENIED)
	{
		switch (iErrorCode)
		{
		case 1:
			WK_TRACE(_T("sequence delete denied\n"));
			m_pServer->AddErrorMessage(sErrorMessage);
			break;
		case 2:
			WK_TRACE(_T("backup archive delete denied\n"));
			m_pServer->AddErrorMessage(sErrorMessage);
			break;
		}
	}
	else if (error == CIPC_ERROR_BACKUP)
	{
		switch (iErrorCode)
		{
		case 1:
			WK_TRACE(_T("backup disconnect error\n"));
			break;
		case 2:
			WK_TRACE(_T("backup initialize error\n"));
			break;
		case 3:
			WK_TRACE(_T("backup copy error\n"));
			break;
		}
		CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
		if (pViewIdipClient)
		{
			CDlgBackup* pBD = pViewIdipClient->GetDlgBackup();
			if (pBD)
			{
				pBD->PostMessage(WM_USER, ID_BACKUP_ERROR, iErrorCode);
			}
		}
	}
	else if (error == CIPC_ERROR_INVALID_VALUE)
	{
		WORD wArchivNr = HIWORD(dwCmd);
		WORD wSequenceNr = LOWORD(dwCmd);
		switch (iErrorCode)
		{
		case 1: // no record found for cam
			{
				CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
				CWndPlay* pDW = pViewIdipClient->GetWndPlay(m_pServer->GetID(),wArchivNr,wSequenceNr);
				if (pDW)
				{
					WK_TRACE(_T("no more pictures found for cam %d, %d, %08lx\n"),
						wArchivNr,wSequenceNr,id.GetID());
					pDW->NoPictureFoundForCamera(id);
				}
				else
				{
					TRACE(_T("no display window for error message\n"));
				}
			}
			break;
		case 2: // record nr out of range
			{
				// could happen, if 1/1 presses next!
				TRACE(_T("record nr out of range\n"));
				CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
				CWndPlay* pDW = pViewIdipClient->GetWndPlay(m_pServer->GetID(),wArchivNr,wSequenceNr);

				if (pDW)
				{
					WK_TRACE(_T("record nr out of range for cam %d, %d, %08lx\n"),
						wArchivNr,wSequenceNr,id.GetID());
					pDW->RecordNrOutOfRange(id);
				}
				else
				{
					TRACE(_T("no display window for error message\n"));
				}
			}
			break;
		case 3: // sequence nr out of range
			{
				// it may be that the sequence is a query result
				// and already deleted during copy progress
				// so we should copy
				DWORD dwRecNo = id.GetID();
				if (m_pServer->IsQueryResult(wArchivNr,wSequenceNr,dwRecNo))
				{
					TRACE(_T("query result out of range srv%d arc%d seq%d rec%d\n"),
						m_pServer->GetID(),wArchivNr,wSequenceNr,dwRecNo);
					m_pServer->CopyNextQueryResult(wArchivNr,wSequenceNr,dwRecNo);
				}
				TRACE(_T("sequence nr out of range\n"));
			}
			break;
		case 4: // archiv nr out of range
			{
				// an invalid archive nr should never happen
				TRACE(_T("archiv nr out of range\n"));
			}
			break;
		case 5:
			TRACE(_T("no more record found for track info S forward A=%d, S=%d\n"),wArchivNr,wSequenceNr);
			{
				CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
				CWndPlay* pDW = pViewIdipClient->GetWndPlay(m_pServer->GetID(),wArchivNr,wSequenceNr);
				if (pDW)
				{
					pDW->SetPlayStatus(PS_SKIP_FORWARD);
					pDW->NoPictureFoundForCamera(id);
				}
			}
			break;
		case 6:
			TRACE(_T("no more record found for track info S backward A=%d, S=%d\n"),wArchivNr,wSequenceNr);
			{
				CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
				CWndPlay* pDW = pViewIdipClient->GetWndPlay(m_pServer->GetID(),wArchivNr,wSequenceNr);
				if (pDW)
				{
					pDW->SetPlayStatus(PS_SKIP_BACK);
					pDW->NoPictureFoundForCamera(id);
				}
			}
			break;
		case 7: // cannot read dbf
		case 8:	// cannot read dat
			{
				CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
				CWndPlay* pDW = pViewIdipClient->GetWndPlay(m_pServer->GetID(),
															wArchivNr,
															wSequenceNr);
				if (pDW)
				{
					WK_TRACE(_T("cannot read picture for cam %d, %d, %08lx\n"),
						wArchivNr,wSequenceNr,id.GetID());
					pDW->CannotReadPictureForCamera();
				}
				else
				{
					TRACE(_T("no display window for error message\n"));
				}
			}
			break;
		default:
			WK_TRACE(_T("error invalid value: %d %s\n"), iErrorCode,(LPCTSTR)sErrorMessage);
			break;
		}
	}
	else if (error == CIPC_ERROR_INVALID_SERIAL)
	{
		WK_TRACE(_T("error invalid serial\n"));
	}
	else
	{
		if (!sErrorMessage.IsEmpty())
		{
			WK_TRACE(_T("error indication: %s\n"), (LPCTSTR)sErrorMessage);
			CErrorMessage* pEM = new CErrorMessage(m_pServer->GetName(),sErrorMessage);
			theApp.m_ErrorMessages.SafeAddTail(pEM);
			AfxGetMainWnd()->PostMessage(WM_USER);
		}

	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmQuery(DWORD dwUserID)
{
	TRACE(_T("OnConfirmQuery ID: %08lx  SequFromID: %d\n"),dwUserID,LOWORD(dwUserID));
	CMainFrame* pMF = theApp.GetMainFrame();
	CViewArchive* pViewArchive = pMF->GetViewArchive();
	CViewIdipClient* pViewIdipClient = pMF->GetViewIdipClient();

	if (pViewIdipClient)
	{
		WORD wa = HIWORD(dwUserID);
		WORD ws = LOWORD(dwUserID);

		CWndAlarmList* pALW = pViewIdipClient->GetWndAlarmList(m_pServer->GetID(),wa,ws);
		if (pALW)
		{
			if (pALW->IsInQuery())
			{
				pALW->ConfirmQuery();
			}
			return;
		}
		else
		{
			CWndPlay* pWndPlay = pViewIdipClient->GetWndPlay(m_pServer->GetID(),wa,ws);
			if (pWndPlay)
			{
				if (pWndPlay->IsInRectQuery())
				{
					//TRACE(_T("TKR ####### IsInRectQuery = TRUE\n"));

					//Ende der Rechtecksuche erreicht (kein weiteres Suchergebnis)
					//entsprechend der Suchrichtung die Button SingleStep und Play disablen

					RectPlayStatus RectPlay	= pWndPlay->GetRectPlayStatus();
					PlayStatus		   Play = pWndPlay->GetPlayStatus();
					if(Play == PS_PLAY_FORWARD || RectPlay == PS_RECT_NAVIGATE_NEXT)
					{
						pWndPlay->SetCanRectPlayForward(FALSE);
					}
					else if(Play == PS_PLAY_BACK || RectPlay == PS_RECT_NAVIGATE_PREV)
					{
						pWndPlay->SetCanRectPlayBack(FALSE);
					}
	
					pWndPlay->ConfirmQuery();
				}
				else
				{
					//TRACE(_T("TKR ####### IsInRectQuery = FALSE\n"));
				}
				return;
			}
		}
	}

	CSyncCoolBar* pSCB = pMF->GetSyncCoolBar();
	if (pSCB)
	{
		if (pSCB->IsInQuery(dwUserID))
		{
			pSCB->ConfirmQuery(dwUserID);
			return;
		}
	}
	
	WK_TRACE(_T("search <%s> %d finished\n"), m_pServer->GetName(),dwUserID);
	m_pServer->ClearSearch();

	if (pViewArchive)
	{
		CDlgSearchResult* pSRD = pViewArchive->GetDlgSearchResult();
		if (pSRD)
		{
			pSRD->PostMessage(WM_COMMAND,ID_SEARCH_FINISHED);
		}
		else
		{
			// TODO! GF Maybe we do a search for the Live-To-Play-Change
			WK_TRACE_ERROR(_T("OnConfirmQuery without search result dialog %08lx\n"),dwUserID);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("OnConfirmQuery without object view\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnIndicateQueryResult(DWORD dwUserID, const CIPCPictureRecord &pictData)
{
	DoResponseQueryResult(dwUserID);

	CViewArchive* pViewArchive = theApp.GetMainFrame()->GetViewArchive();
	if (pViewArchive == NULL)
	{
		return;
	}
	CIdipClientDoc*pDoc = pViewArchive->GetDocument();
	CDlgSearchResult* pSRD = pViewArchive->GetDlgSearchResult();
	if (pSRD)
	{
		pSRD->AddResult(m_pServer->GetID(),pictData);
		pSRD->PostMessage(WM_COMMAND,ID_SEARCH_RESULT);
	}


	CIPCDatabaseIdipClientLocal* pDatabase = pDoc->GetLocalDatabase();
	if (   pDatabase
		&& pDatabase->GetIPCState() == CIPC_STATE_CONNECTED)
	{
		CViewArchive* pViewArchive = theApp.GetMainFrame()->GetViewArchive();
		CDlgSearchResult* pSRD = pViewArchive->GetDlgSearchResult();

		if (pSRD)
		{
			pSRD->PostMessage(WM_COMMAND,ID_SEARCH_RESULT_COPY);
		}

		CIPCFields fields;
		fields.FromString(pictData.GetInfoString());
		fields.SafeAdd(new CIPCField(CIPCField::m_sfStNm,
								    m_pServer->GetName(),32,_T('C')));
		if (pSRD)
		{
			CString sSearchName = pSRD->GetSearchName();
			sSearchName.Replace(_T("\n"),_T(""));
			sSearchName.Replace(_T("\r"),_T(""));
			fields.SafeAdd(new CIPCField(CIPCField::m_sfSqNm,
											sSearchName,
											(WORD)sSearchName.GetLength(),
											_T('C')));
		}
		pDatabase->DoRequestNewSavePictureForHost(m_pServer->GetHostID(),
													FALSE,
													TRUE,
													pictData,
													fields);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnIndicateNewQueryResult(DWORD dwUserID,
													 WORD wArchivNr,
													 WORD wSequenceNr,
													 DWORD dwRecordNr,
												     int iNumFields,
													 const CIPCField fields[])
{
/*

	TRACE(_T("OnIndicateNewQueryResult ID: %08x SeqNrFromID: %d  SeqNr: %d  RecNr: %d\n")
		,dwUserID,LOWORD(dwUserID),wSequenceNr,dwRecordNr);
*/

	CMainFrame* pMF = theApp.GetMainFrame();
	CViewIdipClient* pViewIdipClient = pMF->GetViewIdipClient();

	if (pViewIdipClient)
	{
		WORD wa = HIWORD(dwUserID);
		WORD ws = LOWORD(dwUserID);

		CWndAlarmList* pALW = pViewIdipClient->GetWndAlarmList(m_pServer->GetID(),wa,ws);
		if (pALW)
		{
			if (pALW->IsInQuery())
			{
				pALW->ConfirmQuery(dwUserID,wArchivNr,wSequenceNr,dwRecordNr,iNumFields,fields);
			}
			DoResponseQueryResult(0);
			return;
		}
		else
		{	
			//bei einem Suchergebnis mittels Rechtecksuche entha wa und ws

			CWndPlay* pWndPlay = pViewIdipClient->GetWndPlay(m_pServer->GetID(),wa,ws);
			if(!pWndPlay)
			{
				pWndPlay = pViewIdipClient->GetWndPlay(m_pServer->GetID(),wArchivNr,wSequenceNr);
			}
			if (pWndPlay)
			{
				if (pWndPlay->IsInRectQuery())
				{
					pWndPlay->ConfirmRectQuery(dwUserID,wArchivNr,wSequenceNr,dwRecordNr,iNumFields,fields);
				}
				else
				{
					//TRACE(_T("TKR ### keine Rechtecksuche 0x%x\n"), pWndPlay);
				}
				//hier nicht mit (0) aufrufen (bedeutet ein canceln der query in der Datenbank)
				//da im Falle der Rechtecksuche (mittels Abspielbutton PlayyForward/Back)
				//die Datenbank die Cancelbedingung selbständig ermittelt

				//DoResponseQueryResult(0);
			}
			else
			{
				// kein Playwindow fuer Rechtecksuche
				//TRACE(_T("### kein PlayWindow 0x%x\n"), pWndPlay);
			}
		}
	}
	CSyncCoolBar* pSCB = pMF->GetSyncCoolBar();
	if (pSCB)
	{
		if (pSCB->IsInQuery(dwUserID))
		{
			pSCB->ConfirmQuery(dwUserID,
							   m_pServer->GetID(),
							   wArchivNr,
							   wSequenceNr,
							   dwRecordNr,
							   iNumFields,
							   fields);
			// cancel Query immediately

			// response ever
			DoResponseQueryResult(dwUserID);
			return;
		}
	}

	// response ever
	DoResponseQueryResult(dwUserID);

	if (!m_pServer->IsSearching())
	{
		// handle result only if we are still searching
		// but may be it was a Switch-Live-To-Play-Window search
		if (pViewIdipClient)
		{
			pViewIdipClient->ConfirmQuery(dwUserID,
											m_pServer->GetID(),
											wArchivNr,
											wSequenceNr,
											dwRecordNr,
											iNumFields,
											fields);
		}
		return;
	}

	CViewArchive* pViewArchive = pMF->GetViewArchive();
	if (pViewArchive)
	{
		CDlgSearchResult* pSRD = pViewArchive->GetDlgSearchResult();
		if (pSRD)
		{
			pSRD->AddResult(dwUserID,m_pServer->GetID(),wArchivNr,wSequenceNr,dwRecordNr,iNumFields,fields);
		}
		else
		{
			// tritt auf, wenn die suche abgenrochen wird,
			// und der server noch einen moment ergebnisse sendet.
			WK_TRACE_ERROR(_T("QueryResult without CDlgSearchResult\n"));
		}
	}
	if (theApp.GetQueryParameter().CopyPictures())
	{
		m_pServer->AddQueryResult(dwUserID,wArchivNr,wSequenceNr,dwRecordNr,iNumFields,fields);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnIndicateNewArchiv(const CIPCArchivRecord& data)
{
	WK_TRACE(_T("new archiv %s\n"),data.GetName());

	CIPCArchivRecord* pRecord = GetArchive(data.GetID());

	if (!pRecord)
	{
		CIPCDatabaseClient::OnIndicateNewArchiv(data);
		m_pServer->AddNewArchiv(data);
		AfxGetMainWnd()->PostMessage(WM_USER);
		
		if (GetVersion()>2)
		{
			DoRequestSequenceList(data.GetID());
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnIndicateRemoveArchiv(WORD wArchivNr)
{
	WK_TRACE(_T("archiv deleted %d\n"),wArchivNr);
	CIPCDatabaseClient::OnIndicateRemoveArchiv(wArchivNr);
	CViewArchive* pViewArchive = theApp.GetMainFrame()->GetViewArchive();
	if (pViewArchive)
	{
		pViewArchive->PostMessage(WM_USER,
						 MAKELONG(VDH_DEL_ARCHIV,m_pServer->GetID()),
						 wArchivNr);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnRequestVersionInfo(WORD wGroupID)
{
	// Nr 1 is default
	// Nr 2 is Version 3.6 Build 133
	// Nr 3 is new Recherche
	WK_TRACE(_T("OnRequestVersionInfo confirming 3\n"));
	DoConfirmVersionInfo(0,3);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnRequestDisconnect()
{
	if (m_pServer)
	{
		WK_TRACE(_T("disconnect by server %s\n"), m_pServer->GetName());
		m_pServer->ClearSearch();
		DelayedDelete();
		CViewArchive* pAV = theApp.GetMainFrame()->GetViewArchive();
		if (pAV)
		{
			pAV->PostMessage(WM_USER, MAKELONG(WPARAM_DATABASE_DISCONNECTED, m_pServer->GetID()));
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmDrives(int iNumDrives,
								 const CIPCDrive drives[])
{
	if (m_pServer)
	{
		m_pServer->OnConfirmDrives(iNumDrives,drives);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 DWORD dwID)
{
	CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
	if (pViewIdipClient)
	{
		CDlgBackup* pBD = pViewIdipClient->GetDlgBackup();
		if (pBD)
		{
			pBD->PostMessage(WM_USER, ID_BACKUP_CONFIRM,dwID);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmCancelBackup(DWORD dwUserData)
{
	CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
	if (pViewIdipClient)
	{
		CDlgBackup* pBD = pViewIdipClient->GetDlgBackup();
		if (pBD)
		{
			pBD->PostMessage(WM_USER, ID_BACKUP_CANCEL_CONFIRM,dwUserData);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmBackupByTime(WORD wUserData,
													 DWORD dwID,
													 const CSystemTime& stStartLessMaxCDCapacity,
													 const CSystemTime& stEndMoreMaxCDCapacity,
													 WORD wFlags,
													 CIPCInt64 i64Size)
{
	CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
	if (pViewIdipClient)
	{
		CDlgBackup* pBD = pViewIdipClient->GetDlgBackup();
		if (pBD)
		{
			pBD->OnConfirmBackupByTime(wUserData, dwID, stStartLessMaxCDCapacity, stEndMoreMaxCDCapacity, wFlags, i64Size);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseIdipClient::OnConfirmRecords(WORD  wArchivNr, 
											 WORD  wSequenceNr, 
											 CIPCFields fields,
											 CIPCFields records)
{
	// get the matching CWndAlarmList
	CViewIdipClient* pViewIdipClient = theApp.GetMainFrame()->GetViewIdipClient();
	if (pViewIdipClient)
	{
		CWndAlarmList* pALW = pViewIdipClient->GetWndAlarmList(m_pServer->GetID(),wArchivNr,wSequenceNr);
		if (pALW)
		{
			pALW->ConfirmRecords(fields,records);
		}
		else
		{
			WK_TRACE_ERROR(_T("no alarm list window found\n"));
		}
	}
}
