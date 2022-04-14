// CIPCDatabaseRecherche.cpp: implementation of the CIPCDatabaseRecherche class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "Recherche.h"
#include "RechercheDoc.h"
#include "ObjectView.h"
#include "RechercheView.h"
#include "BackupDialog.h"
#include "Server.h"
#include "CIPCDatabaseRecherche.h"
#include "SyncCoolBar.h"
#include "mainfrm.h"
#include "PlayRealTime.h"
#include "IPCControlAudioUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIPCDatabaseRecherche::CIPCDatabaseRecherche(LPCTSTR shmName, 
											 CServer* pServer, CSecID id)
	: CIPCDatabaseClient(shmName)
{
	m_pServer = pServer;
	m_dwServerVersion = 0; // assume iST 3.0
	m_ID = id;
	m_bGotInfo = FALSE;
	m_bActualizing = FALSE;
	m_dwLastActualizing = GetTickCount();
	StartThread();
}
//////////////////////////////////////////////////////////////////////
CIPCDatabaseRecherche::~CIPCDatabaseRecherche()
{
	StopThread();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::Lock()
{
	m_cs.Lock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::Unlock()
{
	m_cs.Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmConnection()
{
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
void CIPCDatabaseRecherche::OnConfirmGetValue(CSecID id, 
											   const CString &sKey, 
											   const CString &sValue, 
											   DWORD dwServerData)
{
	if (sKey == CSD_IS_DV)
	{
		if (sValue == CSD_ON)
		{
			m_pServer->SetDV();
		}
	}
	else if (0==sKey.CompareNoCase(_T("GetFileVersion")))
	{
		m_pServer->SetVersion(sValue);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmVersionInfo(WORD wGroupID, DWORD dwVersion)
{
	m_dwServerVersion = dwVersion;
	if (m_dwServerVersion>=3)
	{
		DoRequestFieldInfo();
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmAlarmListArchives(int iNumRecords, 
													   const CIPCArchivRecord data[])
{
	for (int i=0;i<iNumRecords;i++)
	{
		// just map the method to one
		// that does everything
		OnIndicateNewArchiv(data[i]);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmInfo(int iNumRecords, const CIPCArchivRecord data[])
{
	Lock();
	CIPCDatabaseClient::OnConfirmInfo(iNumRecords, data);
	m_dwLastActualizing = GetTickCount();
	if (!m_bActualizing)
	{
		m_bGotInfo = TRUE;
		if (m_dwServerVersion<3)
		{
			DoRequestFileList();
			m_pServer->InitFields3x();
		}
		else
		{
			const CIPCArchivRecordArray& a = GetRecords();
			for (int i=0;i<a.GetSize();i++)
			{
				DoRequestSequenceList(a.GetAt(i)->GetID());
			}
			DoRequestDrives();
			if (m_pServer->IsDV())
			{
				DoRequestAlarmListArchives();
			}
		}
	}
	else
	{
		CDataBaseInfoDlg* pDlg = m_pServer->GetInfoDialog();
		if (WK_IS_WINDOW(pDlg))
		{
			pDlg->PostMessage(WM_COMMAND,ID_ARCHIV_INFO);
		}
	}
	m_bActualizing = FALSE;
	Unlock();
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmFieldInfo(int iNumRecords, const CIPCField data[])
{
	m_pServer->OnConfirmFieldInfo(iNumRecords,data);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::ActualizeArchivInfo()
{
	Lock();
	if (!m_bActualizing && (GetTickCount()-m_dwLastActualizing>20000))
	{
		m_bActualizing = TRUE;
		RefreshInfo();
	}
	Unlock();
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnIndicateArchivFile(int iNumRecords, const CIPCArchivFileRecord data[])
{
	int i;

	for (i=0;i<iNumRecords;i++)
	{
		m_pServer->AddArchivFile(data[i]);
	}
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmSequenceList(WORD wArchivNr, int iNumRecords, 
									   const CIPCSequenceRecord data[])
{
	for (int i=0; i<iNumRecords; i++)
	{
		m_pServer->AddNewSequence(data[i]);
	}
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnIndicateRemoveFile(const CIPCArchivFileRecord &data)
{
	m_pServer->AddRemoved(data.GetArchivNr(),data.GetDirNr());
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnIndicateDeleteSequence(WORD wArchivNr, WORD wSequenceNr)
{
	m_pServer->AddRemoved(wArchivNr,wSequenceNr);
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnIndicateNewSequence(const CIPCSequenceRecord& data)
{
	m_pServer->AddNewSequence(data);
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmOpenFile(const CIPCArchivFileRecord &data)
{
	m_pServer->AddToOpenFile(data);
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmRecNo(const CIPCArchivFileRecord &data, const CIPCPictureRecord &pictData)
{
	CRechercheDoc* pDoc;
	pDoc = m_pServer->GetDocument();
	if (pDoc)
	{
		CRechercheView* pRV = pDoc->GetRechercheView();
		if (WK_IS_WINDOW(pRV))
		{
			CDisplayWindow* pDW = pRV->GetDisplayWindow(m_pServer->GetID(),
										CSecID(data.GetArchivNr(),data.GetDirNr()));
			if (WK_IS_WINDOW(pDW))
			{
				pDW->PictureData(data, pictData);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmCameraNames(WORD wArchivNr,
									  DWORD dwUserData,
									  int iNumFields,
									  const CIPCField fields[])
{
	CRechercheDoc* pDoc;
	WORD wSequenceNr = HIWORD(dwUserData);
	pDoc = m_pServer->GetDocument();
	if (pDoc)
	{
		CRechercheView* pRV = pDoc->GetRechercheView();

		if (WK_IS_WINDOW(pRV))
		{
			CDisplayWindow* pDW = pRV->GetDisplayWindow(m_pServer->GetID(),
										CSecID(wArchivNr,wSequenceNr));
			if (WK_IS_WINDOW(pDW))
			{
				pDW->CameraNames(iNumFields,fields);
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmRecordNr(WORD wArchivNr, WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCPictureRecord &pict,
									  int iNumFields,
									  const CIPCField fields[])
{
	TRACE(_T("OnConfirmRecordNr(): dwRecordNr = %u\n"), dwRecordNr);
	CRechercheDoc* pDoc;
	pDoc = m_pServer->GetDocument();
	if (pDoc==NULL)
	{
		return;
	}
	CRechercheView* pRV = pDoc->GetRechercheView();
	CObjectView* pOV = pDoc->GetObjectView();
	CSearchResultDialog* pSRD = NULL;
	
	if (WK_IS_WINDOW(pOV))
	{
		pSRD = pOV->GetSearchResultDialog();
	}

	if (theApp.GetQueryParameter().CopyPictures())
	{
		if (m_pServer->IsQueryResult(wArchivNr,wSequenceNr,dwRecordNr))
		{
			CServer* pServer = m_pServer->GetDocument()->GetLocalServer();
			if (pServer)
			{
				CIPCDatabaseRecherche* pDatabase = pServer->GetDatabase();
				if (pDatabase->GetVersion()>2)
				{
					CIPCFields fieldscopy;
					fieldscopy.FromArray(iNumFields,fields);

					if (WK_IS_WINDOW(pSRD))
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

					pDatabase->DoRequestNewSavePictureForHost(m_pServer->GetHostID(),
															  FALSE,
															  TRUE,
															  pict,
															  fieldscopy);
					if (WK_IS_WINDOW(pSRD))
					{
						pSRD->PostMessage(WM_COMMAND,ID_SEARCH_RESULT_COPY);
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("local database server version wrong\n"));
				}
			}
			m_pServer->CopyNextQueryResult(wArchivNr,wSequenceNr,dwRecordNr);
		}
	}


	if (WK_IS_WINDOW(pRV))
	{
		
		CDisplayWindow* pDW = pRV->GetDisplayWindow(m_pServer->GetID(),
									CSecID(wArchivNr,wSequenceNr));
		if (WK_IS_WINDOW(pDW))
		{

			CPlayRealTime* pPlayRealTime = pDW->GetPlayRealTime();

			if(   !pDW->ContainsAudio() 
			   && theApp.CanPlayRealtime() 
			   && pPlayRealTime)
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
						pDW->PictureData(dwRecordNr,dwNrOfRecords,
										 pict, iNumFields,fields);
						//pDW->RecordNr(wSequenceNr,dwRecordNr,dwNrOfRecords,pict,iNumFields,fields);
					}
					else
					{
	TRACE(_T("OnConfirmRecordNr(): dwRecordNr = %u\n"), dwRecordNr);
					}
				}
				else
				{
					pDW->PictureData(dwRecordNr,dwNrOfRecords,
									 pict, iNumFields,fields);
					//pDW->RecordNr(wSequenceNr,dwRecordNr,dwNrOfRecords,pict,iNumFields,fields);
				}
			}
			else //kein Realtime
			{
				pDW->PictureData(dwRecordNr,dwNrOfRecords,
								 pict, iNumFields,fields);

			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmRecordNr(WORD wArchivNr, 
									  WORD wSequenceNr, 
									  DWORD dwRecordNr,
									  DWORD dwNrOfRecords,
									  const CIPCMediaSampleRecord &media,
									  int iNumFields,
									  const CIPCField fields[])
{

	CRechercheDoc* pDoc;
	pDoc = m_pServer->GetDocument();
	if (pDoc==NULL)
	{
		return;
	}
	CRechercheView* pRV = pDoc->GetRechercheView();
	
	CObjectView* pOV = pDoc->GetObjectView();
	CSearchResultDialog* pSRD = NULL;
	
	if (WK_IS_WINDOW(pOV))
	{
		pSRD = pOV->GetSearchResultDialog();
	}

	if (theApp.GetQueryParameter().CopyPictures())
	{
		if (m_pServer->IsQueryResult(wArchivNr,wSequenceNr,dwRecordNr))
		{
			CServer* pServer = m_pServer->GetDocument()->GetLocalServer();
			if (pServer)
			{
				CIPCDatabaseRecherche* pDatabase = pServer->GetDatabase();
				if (pDatabase->GetVersion()>2)
				{
					CIPCFields fieldscopy;
					fieldscopy.FromArray(iNumFields,fields);

					if (WK_IS_WINDOW(pSRD))
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
					if (WK_IS_WINDOW(pSRD))
					{
						pSRD->PostMessage(WM_COMMAND,ID_SEARCH_RESULT_COPY);
					}
				}
				else
				{
					WK_TRACE_ERROR(_T("local database server version wrong\n"));
				}
			}
			m_pServer->CopyNextQueryResult(wArchivNr,wSequenceNr,dwRecordNr);
		}
	}

	if (WK_IS_WINDOW(pRV))
	{
		CDisplayWindow* pDW = pRV->GetDisplayWindow(m_pServer->GetID(),CSecID(wArchivNr,wSequenceNr));
		if (WK_IS_WINDOW(pDW))
		{
			pDW->AudioData(dwRecordNr, dwNrOfRecords, media, iNumFields, fields);
		}
	}
}
//////////////////////////////////////////////////////////////////////
CIPCArchivRecord* CIPCDatabaseRecherche::GetArchiv(WORD wArchiv)
{
	Lock();
	const CIPCArchivRecordArray& a = GetRecords();
	int i,c;
	CIPCArchivRecord* pRecord = NULL;
	c = a.GetSize();
	for (i=0;i<c;i++)
	{
		if (a.GetAt(i)->GetSubID() == wArchiv)
		{
			pRecord = a.GetAt(i);
			break;
		}
	}
	Unlock();
	return pRecord;
}
//////////////////////////////////////////////////////////////////////
CString	CIPCDatabaseRecherche::GetArchivName(WORD wArchiv)
{
	Lock();
	const CIPCArchivRecordArray& a = GetRecords();
	int i,c;
	CString s;
	c = a.GetSize();
	for (i=0;i<c;i++)
	{
		if (a.GetAt(i)->GetSubID() == wArchiv)
		{
			s = a.GetAt(i)->GetName();
			break;
		}
	}
	Unlock();
	return s;
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnIndicateError(DWORD dwCmd, 
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

		CRechercheDoc* pDoc = m_pServer->GetDocument();
		CObjectView* pOV = pDoc->GetObjectView();
		if (WK_IS_WINDOW(pOV))
		{
			CSearchResultDialog* pSRD = pOV->GetSearchResultDialog();
			if (WK_IS_WINDOW(pSRD))
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
		CRechercheDoc* pDoc = m_pServer->GetDocument();
		CRechercheView* pRV = pDoc->GetRechercheView();
		if (WK_IS_WINDOW(pRV))
		{
			CBackupDialog* pBD = pRV->GetBackupDialog();
			if (WK_IS_WINDOW(pBD))
			{
				pBD->PostMessage(WM_COMMAND,ID_BACKUP_ERROR,iErrorCode);
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
				CRechercheDoc* pDoc = m_pServer->GetDocument();
				CRechercheView* pRV = pDoc->GetRechercheView();
				CDisplayWindow* pDW = pRV->GetDisplayWindow(m_pServer->GetID(),wArchivNr,wSequenceNr);

				if (WK_IS_WINDOW(pDW))
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
				CRechercheDoc* pDoc = m_pServer->GetDocument();
				CRechercheView* pRV = pDoc->GetRechercheView();
				CDisplayWindow* pDW = pRV->GetDisplayWindow(m_pServer->GetID(),wArchivNr,wSequenceNr);

				if (WK_IS_WINDOW(pDW))
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
		case 7: // cannot read dbf
		case 8:	// cannot read dat
			{
				CRechercheDoc* pDoc = m_pServer->GetDocument();
				CRechercheView* pRV = pDoc->GetRechercheView();
				CDisplayWindow* pDW = pRV->GetDisplayWindow(m_pServer->GetID(),
															wArchivNr,
															wSequenceNr);

				if (WK_IS_WINDOW(pDW))
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
			WK_TRACE(_T("error invalid value: %s\n"), (LPCTSTR)sErrorMessage);
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
void CIPCDatabaseRecherche::OnConfirmQuery(DWORD dwUserID)
{
	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CObjectView* pOV = pDoc->GetObjectView();
	CRechercheView* pRV = pDoc->GetRechercheView();
	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;

	if (WK_IS_WINDOW(pRV))
	{
		WORD wa = HIWORD(dwUserID);
		WORD ws = LOWORD(dwUserID);
		CAlarmListWindow* pALW = pRV->GetAlarmListWindow(m_pServer->GetID(),wa,ws);
		if (WK_IS_WINDOW(pALW))
		{
			if (pALW->IsInQuery())
			{
				pALW->ConfirmQuery();
			}
			return;
		}
	}

	CSyncCoolBar* pSCB = pMF->GetSyncCoolBar();
	if (WK_IS_WINDOW(pSCB))
	{
		if (pSCB->IsInQuery(dwUserID))
		{
			pSCB->ConfirmQuery(dwUserID);
			return;
		}
	}
	
	
	WK_TRACE(_T("search <%s> %d finished\n"), m_pServer->GetName(),dwUserID);
	m_pServer->ClearSearch();

	if (WK_IS_WINDOW(pOV))
	{
		CSearchResultDialog* pSRD = pOV->GetSearchResultDialog();
		if (WK_IS_WINDOW(pSRD))
		{
			pSRD->PostMessage(WM_COMMAND,ID_SEARCH_FINISHED);
		}
		else
		{
			WK_TRACE_ERROR(_T("OnConfirmQuery without search result dialog %08lx\n"),dwUserID);
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("OnConfirmQuery without object view\n"));
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnIndicateQueryResult(DWORD dwUserID, const CIPCPictureRecord &pictData)
{
	DoResponseQueryResult(dwUserID);

	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CObjectView* pOV = pDoc->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		CSearchResultDialog* pSRD = pOV->GetSearchResultDialog();
		if (WK_IS_WINDOW(pSRD))
		{
			pSRD->AddResult(m_pServer->GetID(),pictData);
			pSRD->PostMessage(WM_COMMAND,ID_SEARCH_RESULT);
		}
	}


	CServer* pServer = m_pServer->GetDocument()->GetLocalServer();
	if (pServer)
	{
		CIPCDatabaseRecherche* pDatabase = pServer->GetDatabase();
		if (pDatabase->GetVersion()<3)
		{
			pDatabase->DoRequestSavePictureForHost(	m_pServer->GetHostID(),
													pictData,	//&const	CIPCPictureRecord &data,
													FALSE,		//BOOL	bIsAlarmConnection,
													TRUE);		//BOOL	bIsSearchResult,
		}
		else
		{
			CSearchResultDialog* pSRD =NULL;
			CRechercheDoc* pDoc = m_pServer->GetDocument();
			CObjectView* pOV = pDoc->GetObjectView();
			if (WK_IS_WINDOW(pOV))
			{
				pSRD = pOV->GetSearchResultDialog();
				if (WK_IS_WINDOW(pSRD))
				{
					pSRD->PostMessage(WM_COMMAND,ID_SEARCH_RESULT_COPY);
				}
			}

			CIPCFields fields;
			fields.FromString(pictData.GetInfoString());
			fields.SafeAdd(new CIPCField(CIPCField::m_sfStNm,
								     m_pServer->GetName(),32,_T('C')));
			if (WK_IS_WINDOW(pSRD))
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
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnIndicateNewQueryResult(DWORD dwUserID,
													 WORD wArchivNr,
													 WORD wSequenceNr,
													 DWORD dwRecordNr,
												     int iNumFields,
													 const CIPCField fields[])
{
//	TRACE(_T("OnIndicateNewQueryResult %08x %04hx\n"),dwUserID,wArchivNr);

	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CRechercheView* pRV = pDoc->GetRechercheView();
	CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;

	if (WK_IS_WINDOW(pRV))
	{
		WORD wa = HIWORD(dwUserID);
		WORD ws = LOWORD(dwUserID);
		CAlarmListWindow* pALW = pRV->GetAlarmListWindow(m_pServer->GetID(),wa,ws);
		if (WK_IS_WINDOW(pALW))
		{
			if (pALW->IsInQuery())
			{
				pALW->ConfirmQuery(dwUserID,wArchivNr,wSequenceNr,dwRecordNr,iNumFields,fields);
				DoResponseQueryResult(0);
			}
			return;
		}
	}
	CSyncCoolBar* pSCB = pMF->GetSyncCoolBar();
	if (WK_IS_WINDOW(pSCB))
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
		return;
	}

	CObjectView* pOV = pDoc->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		CSearchResultDialog* pSRD = pOV->GetSearchResultDialog();
		if (WK_IS_WINDOW(pSRD))
		{
			pSRD->AddResult(dwUserID,m_pServer->GetID(),wArchivNr,wSequenceNr,dwRecordNr,iNumFields,fields);
			pSRD->PostMessage(WM_COMMAND,ID_SEARCH_RESULT);
		}
		else
		{
			// tritt auf, wenn die suche abgenrochen wird,
			// und der server noch einen moment ergebnisse sendet.
//			WK_TRACE_ERROR(_T("QueryResult without CSearchResultDialog\n"));
		}
	}

	if (theApp.GetQueryParameter().CopyPictures())
	{
		m_pServer->AddQueryResult(dwUserID,wArchivNr,wSequenceNr,dwRecordNr,iNumFields,fields);
	}

}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnIndicateNewArchiv(const CIPCArchivRecord& data)
{
	WK_TRACE(_T("new archiv %s\n"),data.GetName());

	CIPCArchivRecord* pRecord = GetArchiv(data.GetID());

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
void CIPCDatabaseRecherche::OnIndicateRemoveArchiv(WORD wArchivNr)
{
	WK_TRACE(_T("archiv deleted %d\n"),wArchivNr);
	CIPCDatabaseClient::OnIndicateRemoveArchiv(wArchivNr);
	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CObjectView* pOV = pDoc->GetObjectView();
	if (WK_IS_WINDOW(pOV))
	{
		pOV->PostMessage(WM_USER,
						 MAKELONG(VDH_DEL_ARCHIV,m_pServer->GetID()),
						 wArchivNr);
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnRequestVersionInfo(WORD wGroupID)
{
	// Nr 1 is default
	// Nr 2 is Version 3.6 Build 133
	// Nr 3 is new Recherche
	WK_TRACE(_T("OnRequestVersionInfo confirming 3\n"));
	DoConfirmVersionInfo(0,3);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnRequestDisconnect()
{
	WK_TRACE(_T("disconnect by server %s\n"),m_pServer->GetName());
	m_pServer->ClearSearch();
	DelayedDelete();
	AfxGetMainWnd()->PostMessage(WM_USER);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmDrives(int iNumDrives,
								 const CIPCDrive drives[])
{
	m_pServer->OnConfirmDrives(iNumDrives,drives);
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmBackup(DWORD dwUserData,
								 const CString& sDestinationPath,
								 DWORD dwID)
{
	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CRechercheView* pRV = pDoc->GetRechercheView();
	if (WK_IS_WINDOW(pRV))
	{
		CBackupDialog* pBD = pRV->GetBackupDialog();
		if (WK_IS_WINDOW(pBD))
		{
			pBD->PostMessage(WM_COMMAND,ID_BACKUP_CONFIRM,dwID);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmCancelBackup(DWORD dwUserData)
{
	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CRechercheView* pRV = pDoc->GetRechercheView();
	if (WK_IS_WINDOW(pRV))
	{
		CBackupDialog* pBD = pRV->GetBackupDialog();
		if (WK_IS_WINDOW(pBD))
		{
			pBD->PostMessage(WM_COMMAND,ID_BACKUP_CANCEL_CONFIRM,dwUserData);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmBackupByTime(WORD wUserData,
													 DWORD dwID,
													 const CSystemTime& stStartLessMaxCDCapacity,
													 const CSystemTime& stEndMoreMaxCDCapacity,
													 WORD wFlags,
													 CIPCInt64 i64Size)
{
	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CRechercheView* pRV = pDoc->GetRechercheView();
	if (WK_IS_WINDOW(pRV))
	{
		CBackupDialog* pBD = pRV->GetBackupDialog();
		if (WK_IS_WINDOW(pBD))
		{
			pBD->OnConfirmBackupByTime(wUserData, dwID, stStartLessMaxCDCapacity, stEndMoreMaxCDCapacity, wFlags, i64Size);
		}
	}
}
//////////////////////////////////////////////////////////////////////
void CIPCDatabaseRecherche::OnConfirmRecords(WORD  wArchivNr, 
											 WORD  wSequenceNr, 
											 CIPCFields fields,
											 CIPCFields records)
{
	// get the matching CAlarmListWindow
	CRechercheDoc* pDoc = m_pServer->GetDocument();
	CRechercheView* pRV = pDoc->GetRechercheView();
	if (WK_IS_WINDOW(pRV))
	{
		CAlarmListWindow* pALW = pRV->GetAlarmListWindow(m_pServer->GetID(),wArchivNr,wSequenceNr);
		if (WK_IS_WINDOW(pALW))
		{
			pALW->ConfirmRecords(fields,records);
		}
		else
		{
			WK_TRACE_ERROR(_T("no alarm list window found\n"));
		}
	}
}
