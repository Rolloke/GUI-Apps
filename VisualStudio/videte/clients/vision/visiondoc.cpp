// VisionDoc.cpp : implementation of the CVisionDoc class
//

#include "stdafx.h"
#include "Vision.h"

#include "VisionDoc.h"
#include "VisionView.h"
#include "ObjectView.h"
#include "Mainfrm.h"

#include "CIPCDataBaseVision.h"
#include "CIPCServerControlVision.h"
#include "CIPCInputVisionCommData.h"
#include "CIPCOutputVisionDecoder.h"
#include "CIPCControlAudioUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVisionDoc

IMPLEMENT_DYNCREATE(CVisionDoc, CDocument)

BEGIN_MESSAGE_MAP(CVisionDoc, CDocument)
	//{{AFX_MSG_MAP(CVisionDoc)
	ON_COMMAND(ID_CONNECT_LOCAL, OnConnectLocal)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_LOCAL, OnUpdateConnectLocal)
	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_UPDATE_COMMAND_UI(ID_CONNECT, OnUpdateConnect)
	ON_UPDATE_COMMAND_UI(ID_DISCONNECT, OnUpdateDisconnect)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_PANE, OnUpdateSequenzePane)
	ON_UPDATE_COMMAND_UI(ID_DATABASE_PANE, OnUpdateDatabasePane)
	ON_COMMAND(ID_SEQUENZE, OnSequenze)
	ON_COMMAND(ID_SEQUENCE_1, OnSequence1)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_1, OnUpdateSequence1)
	ON_COMMAND(ID_SEQUENCE_10, OnSequence10)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_10, OnUpdateSequence10)
	ON_COMMAND(ID_SEQUENCE_15, OnSequence15)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_15, OnUpdateSequence15)
	ON_COMMAND(ID_SEQUENCE_2, OnSequence2)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_2, OnUpdateSequence2)
	ON_COMMAND(ID_SEQUENCE_20, OnSequence20)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_20, OnUpdateSequence20)
	ON_COMMAND(ID_SEQUENCE_3, OnSequence3)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_3, OnUpdateSequence3)
	ON_COMMAND(ID_SEQUENCE_30, OnSequence30)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_30, OnUpdateSequence30)
	ON_COMMAND(ID_SEQUENCE_4, OnSequence4)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_4, OnUpdateSequence4)
	ON_COMMAND(ID_SEQUENCE_5, OnSequence5)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_5, OnUpdateSequence5)
	ON_COMMAND(ID_VIDEO_SAVE_ALL, OnVideoSaveAll)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SAVE_ALL, OnUpdateVideoSaveAll)
	ON_COMMAND(ID_CONNECT_MAP, OnConnectMap)
	ON_COMMAND(ID_VIDEO_SAVE_ALL_STOP, OnVideoSaveAllStop)
	ON_COMMAND(ID_VIDEO_SAVE_ALL_TOGGLE, OnVideoSaveAllToggle)
	//}}AFX_MSG_MAP
	ON_UPDATE_COMMAND_UI(ID_CONNECT_MAP, OnUpdateConnectMap)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVisionDoc construction/destruction

CVisionDoc::CVisionDoc()
{
	m_pReconnectThread = NULL;
	m_pCIPCDataBaseVision = NULL;
	m_pCIPCServerControlVision = NULL;
	m_pCIPCOutputVisionDecoder = NULL;
	m_pCIPCInputVisionCommData = NULL;
	m_pCIPCLocalAudio = NULL;

	m_bRun = FALSE;
	m_bReconnect = FALSE;
	m_bAutoLogout = FALSE;
	m_bSequence = FALSE;
	m_dwSequenceIntervall = theApp.m_dwSequenceIntervall;
	m_Servers.SetDocument(this);
	m_dwLastBeep = 0;
	m_bStore = FALSE;
	m_dwLastStored = 0;
	
	m_dwLastLocalTryInput = 0;
	m_dwLastLocalTryOutput = 0;
	m_dwLastLocalTryDatabase = 0;
	m_dwLastLocalTryAudio = 0;
	
	m_pConnectionDlg = NULL;

	StartReconnectThread();
}
/////////////////////////////////////////////////////////////////////////////
CVisionDoc::~CVisionDoc()
{
	StopReconnectThread();

	WK_DELETE(m_pConnectionDlg);
	WK_DELETE(m_pReconnectThread);

	m_aConnectionRecords.SafeDeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CVisionDoc serialization

void CVisionDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{

	}
	else
	{

	}
}

/////////////////////////////////////////////////////////////////////////////
// CVisionDoc diagnostics

#ifdef _DEBUG
void CVisionDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CVisionDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
BOOL CVisionDoc::IsDatabaseConnected()
{
	BOOL bRet = FALSE;
	if (m_pCIPCDataBaseVision)
	{
		bRet = (m_pCIPCDataBaseVision->GetIPCState() == CIPC_STATE_CONNECTED);
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CIPCDataBaseVision*	CVisionDoc::GetDatabase()
{
	if (IsDatabaseConnected())
	{
		return m_pCIPCDataBaseVision;
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
// Local Connection to Database !!
void CVisionDoc::ConnectToDatabase()
{
	if (WK_IS_RUNNING(WK_APP_NAME_DB_SERVER)==FALSE) 
	{
		return; // no chance without DatabaseServer.exe
	}

	if (NULL==theApp.m_pUser)
	{
		return; // no chance without user
	}

	if (IsDatabaseConnected())
	{
		return;
	}

	DWORD dwTick = GetTickCount();

	if (dwTick - m_dwLastLocalTryDatabase < 10 * 1000)
	{
		AfxGetMainWnd()->PostMessage(WM_USER);
		return;
	}

	m_dwLastLocalTryDatabase = dwTick;

	CConnectionRecord c(LOCAL_LOOP_BACK,
		*theApp.m_pUser,
		theApp.m_pPermission->GetName(),
		theApp.m_pPermission->GetPassword()
		);
	CString sSmName;
	CIPCFetch fetch;
	CIPCFetchResult fr = fetch.FetchDatabase(c);
	sSmName = fr.GetShmName();

	if (!sSmName.IsEmpty())
	{
		m_pCIPCDataBaseVision = new CIPCDataBaseVision(this,sSmName);
	}
	Sleep(10);
}
/////////////////////////////////////////////////////////////////////////////
// Local Connection to Audio Unit !!
void CVisionDoc::ConnectToAudio()
{
	if (IsAudioConnected())
	{
		return;	// already connected
	}
	if (NULL == theApp.m_pUser)
	{
		return;	// no chance without user
	}
	if (theApp.m_pDongle == NULL)
	{
		return;	// no dongle
	}
	if (!theApp.m_pDongle->RunAudioUnit())
	{
		return;	// not allowed
	}
	if (WK_IS_RUNNING(WK_APP_NAME_AUDIOUNIT1)==FALSE) 
	{
		return; // no chance without the AudioUnit
	}

#ifdef _DTS
	HWND hwnd = ::FindWindow(NULL, WK_APP_NAME_AUDIOUNIT1);
	if (hwnd)
	{
		DWORD dwResult;
		SetLastError(0);
		::SendMessageTimeout(hwnd, WK_WINTHREAD_STOP_THREAD, TRUE, NULL, SMTO_BLOCK, 5000, &dwResult);
		DWORD dwError = GetLastError();
		if ((dwError == 0) && (dwResult != 0))
		{
			CString str;
			str.Format(SM_AudioUnitMedia, 1);
			str += _T("Control");
			m_pCIPCLocalAudio = new CIPCAudioVision(str, this, TRUE);
		}
	}
#else

	DWORD dwTick = GetTickCount();
	if (dwTick - m_dwLastLocalTryAudio < 10 * 1000)
	{
		AfxGetMainWnd()->PostMessage(WM_USER); // Force OnIdle() !!
		return;
	}
	m_dwLastLocalTryAudio = dwTick;

	CConnectionRecord c(LOCAL_LOOP_BACK,
		*theApp.m_pUser,
		theApp.m_pPermission->GetName(),
		theApp.m_pPermission->GetPassword()
		);

	CString sSmName;
	CIPCFetch fetch;
	CIPCFetchResult fr = fetch.FetchAudio(c);
	sSmName = fr.GetShmName();

	if (!sSmName.IsEmpty())
	{
		m_pCIPCLocalAudio = new CIPCAudioVision(sSmName, this);
	}
	Sleep(10);
#endif
}
//////////////////////////////////////////////////////////////////////////
BOOL CVisionDoc::IsAudioConnected()
{
	BOOL bRet = FALSE;
	if (m_pCIPCLocalAudio)
	{
		bRet = (m_pCIPCLocalAudio->GetIPCState() == CIPC_STATE_CONNECTED);
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CIPCAudioVision* CVisionDoc::GetLocalAudio()
{
	if (IsAudioConnected())
	{
		return m_pCIPCLocalAudio;
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
// Local Connection to Decoder !!
void CVisionDoc::ConnectToDecoder()
{
	if (theApp.m_bCoCoSoft)
	{
		return;
	}

	if (WK_IS_RUNNING(WK_APP_NAME_SERVER)==FALSE) 
	{
		return; // no chance without Sec3.exe
	}

	if (NULL==theApp.m_pUser)
	{
		return; // no chance without user
	}

	if (IsDecoderConnected())
	{
		return;
	}

	DWORD dwTick = GetTickCount();

	if (dwTick - m_dwLastLocalTryOutput < 2 * 1000)
	{
		AfxGetMainWnd()->PostMessage(WM_USER);
		return;
	}

	m_dwLastLocalTryOutput = dwTick;

	CConnectionRecord c(LOCAL_LOOP_BACK,
		*theApp.m_pUser,
		theApp.m_pPermission->GetName(),
		theApp.m_pPermission->GetPassword()
		);
	c.SetOptions(SCO_WANT_DECOMPRESS_COLOR);

	CString sSmName;
	CIPCFetch fetch;
	CIPCFetchResult fr = fetch.FetchOutput(c);
	sSmName = fr.GetShmName();

	if (!sSmName.IsEmpty())
	{
		m_pCIPCOutputVisionDecoder = new CIPCOutputVisionDecoder(sSmName,this);
	}
	Sleep(100);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionDoc::IsDecoderConnected()
{
	BOOL bRet = FALSE;
	if (m_pCIPCOutputVisionDecoder)
	{
		bRet = (m_pCIPCOutputVisionDecoder->GetIPCState() == CIPC_STATE_CONNECTED);
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CIPCOutputVisionDecoder* CVisionDoc::GetCIPCOutputVisionDecoder()
{
	if (IsDecoderConnected())
	{
		return m_pCIPCOutputVisionDecoder;
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
// Local Connection !!
void CVisionDoc::ConnectToCommData()
{
	if (WK_IS_RUNNING(WK_APP_NAME_SERVER)==FALSE) 
	{
		return; // no chance without Sec3.exe
	}

	if (NULL==theApp.m_pUser)
	{
		return; // no chance without user
	}

	if (!theApp.m_bRS232Transparent)
	{
		return;
	}

	if (IsCommDataConnected())
	{
		return;
	}

	DWORD dwTick = GetTickCount();

	if (dwTick - m_dwLastLocalTryInput < 2 * 1000)
	{
		AfxGetMainWnd()->PostMessage(WM_USER);
		return;
	}

	m_dwLastLocalTryInput = dwTick;

	WK_TRACE(_T("connecting to local input for comm data\n"));
	CConnectionRecord c(LOCAL_LOOP_BACK,
		*theApp.m_pUser,
		theApp.m_pPermission->GetName(),
		theApp.m_pPermission->GetPassword()
		);
	CString sSmName;
	CIPCFetch fetch;
	CIPCFetchResult fr = fetch.FetchInput(c);
	sSmName = fr.GetShmName();

	if (!sSmName.IsEmpty())
	{
		m_pCIPCInputVisionCommData = new CIPCInputVisionCommData(sSmName,this);
	}
	Sleep(100);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionDoc::IsCommDataConnected()
{
	BOOL bRet = FALSE;
	if (m_pCIPCInputVisionCommData)
	{
		bRet = (m_pCIPCInputVisionCommData->GetIPCState() == CIPC_STATE_CONNECTED);
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CIPCInputVisionCommData* CVisionDoc::GetCIPCInputVisionCommData()
{
	if (IsCommDataConnected())
	{
		return m_pCIPCInputVisionCommData;
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
CIPCServerControlVision* CVisionDoc::GetCIPCServerControlVision()
{
	return m_pCIPCServerControlVision;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::UpdateViewRects(BOOL bActive)
{
#ifndef _DTS
	CVisionView* pView = GetVisionView();
	if (WK_IS_WINDOW(pView))
	{
		if (IsDecoderConnected())
		{
			if (bActive)
			{
				pView->SetDisplayOutputWindow(SDOW_DISPLAY|SDOW_REACTIVATE);
			}
			else
			{
				// don't clear display window
				// because no coco settings possible
			}
		}
	}
#endif
}
/////////////////////////////////////////////////////////////////////////////
CObjectView* CVisionDoc::GetObjectView()
{
	CView* pView;
	POSITION pos = GetFirstViewPosition();
	while (pos) 
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CObjectView))) 
		{
			return (CObjectView*)pView;
			break;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CVisionView* CVisionDoc::GetVisionView()
{
	CView* pView;
	POSITION pos = GetFirstViewPosition();
	while (pos) 
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CVisionView))) 
		{
			return (CVisionView*)pView;
			break;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
// CVisionDoc commands
void CVisionDoc::OnIdle()
{
	m_Servers.OnIdle();

	if (m_bReconnect)
	{
		Reconnect();
	}
	if (m_bAutoLogout)
	{
		AutoLogout();
	}
	if (m_aConnectionRecords.GetCount())
	{
		OnConnectionRecord();
	}
	if (m_pCIPCOutputVisionDecoder!=NULL && (m_pCIPCOutputVisionDecoder->GetIPCState() == CIPC_STATE_DISCONNECTED))
	{
		WK_DELETE(m_pCIPCOutputVisionDecoder);
	}
	if (m_pCIPCInputVisionCommData!=NULL && (m_pCIPCInputVisionCommData->GetIPCState() == CIPC_STATE_DISCONNECTED))
	{
		WK_DELETE(m_pCIPCInputVisionCommData);
	}
	if (m_pCIPCLocalAudio!=NULL && (m_pCIPCLocalAudio->GetIPCState() == CIPC_STATE_DISCONNECTED))
	{
		WK_DELETE(m_pCIPCLocalAudio);
	}
	
	if (theApp.m_bAlarmAnimation)
	{
		AlarmAnimation();
	}
	if (m_Servers.GetSize() == 0)
	{
		m_bSequence = FALSE;
	}
	Sleep(0);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::AlarmAnimation()
{
	BOOL bBeep = FALSE;
	if (m_dwLastBeep==0)
	{
		m_dwLastBeep = GetTickCount();
		bBeep = TRUE;
	}
	else
	{
		DWORD dwTick = GetTickCount();
		if ((dwTick - m_dwLastBeep) > (10 * 1000))
		{
			m_dwLastBeep = dwTick;
			bBeep = TRUE;
		}
	}
	if (bBeep)
	{
		// Only the last MessageBeep will be played, no multi-tone possible 
//		MessageBeep(MB_OK);					// Standardton-Warnsignal
//		MessageBeep(MB_ICONASTERISK);		// Stern
		MessageBeep(MB_ICONEXCLAMATION);	// Hinweis
//		MessageBeep(MB_ICONHAND);			// 
//		MessageBeep(MB_ICONQUESTION);		// Frage
// MessageBeep will only be played on sound card, Beep plays always on speaker
		Beep(1000, 100);
		Beep( 440, 100);
		Beep(1000, 100);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnConnectLocal() 
{
	if (m_Servers.GetLocalServer())
	{
		return;
	}
	if (WK_IS_RUNNING(WK_APP_NAME_SERVER)==FALSE) 
	{
		return; // no chance without sec3.exe
	}

	CServer* pServer;
	CHostArray& ha = theApp.GetHosts();

	CSecID idHost = ha.GetLocalHost()->GetID();

	pServer = m_Servers.GetServer(idHost);
	if (pServer==NULL)
	{
		pServer = m_Servers.AddServer();
		pServer->Connect(ha.GetLocalHost()->GetName(),LOCAL_LOOP_BACK,idHost,10*1000);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateConnectLocal(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Servers.GetLocalServer()==NULL);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionDoc::Connect(CSecID idHostID)
{
	if (idHostID.GetSubID() == SECSUBID_LOCAL_HOST)
	{
		if (GetLocalServer() == NULL)
		{
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_CONNECT_LOCAL, 0);
			return TRUE;
		}
		else
		{
			return 2;//  already connected
		}
	}
	else
	{
// 	CWK_Profile wkp;
		CHostArray& ha = theApp.GetHosts();
		CHost* pHost;
		pHost = ha.GetHost(idHostID);
		if (pHost)
		{
			CServer* pServer;
			pServer = m_Servers.GetServer(idHostID);
			if (pServer==NULL)
			{
				pServer = m_Servers.AddServer();
				pServer->Connect(pHost->GetName(),
								 pHost->GetCleanedUpNumber(),
								 pHost->GetID(),0);
				return TRUE;
			}
			else 
			{
				return 2;//  already connected
			}
		}
		else
		{
			return FALSE;
	//		WK_TRACE_ERROR(_T("connect unknown host\n"));
		}
	}

}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::CSConnectionDlgLockFunction(BOOL bLock)
{
	if (bLock)
	{
		m_csConnectionDlg.Lock();
	}
	else
	{
		m_csConnectionDlg.Unlock();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnConnect() 
{
	CSConnectionDlgLockFunction(TRUE);
	m_pConnectionDlg = new COEMConnectionDialog(theApp.GetHosts(),AfxGetMainWnd());
	CSConnectionDlgLockFunction(FALSE);

	m_pConnectionDlg->m_sFilterTypes = _T("SMS,FAX,E-Mail");

	if (IDOK==m_pConnectionDlg->DoModal())
	{
		CServer* pServer = m_Servers.GetServer(m_pConnectionDlg->m_idHost);
		if (pServer==NULL)
		{
			pServer = m_Servers.AddServer();
			pServer->Connect(m_pConnectionDlg->m_sHost, m_pConnectionDlg->m_sTelefon, m_pConnectionDlg->m_idHost,0);
		}
	}
	CSConnectionDlgLockFunction(TRUE);
	WK_DELETE(m_pConnectionDlg);
	CSConnectionDlgLockFunction(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnConnectMap() 
{
	CVisionView* pVV = GetVisionView();
	if (WK_IS_WINDOW(pVV))
	{
		pVV->ShowMapWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateConnectMap(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!theApp.GetMapURL().IsEmpty());
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateConnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.m_bRunAnyLinkUnit && !m_Servers.IsAnyServerFetching());
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::ActualizeInput(CIPCInputVision* pInput)
{
	UpdateAllViews(NULL, VDH_INPUT, (CObject*)pInput);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::ActualizeOutput(CIPCOutputVision* pOutput)
{
	UpdateAllViews(NULL, VDH_OUTPUT, (CObject*)pOutput);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::ActualizeAudio(CIPCAudioVision* pAudio)
{
	UpdateAllViews(NULL, VDH_AUDIO, (CObject*)pAudio);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::Disconnect(WORD wServerID)
{
	UpdateAllViews(NULL,VDH_DEACTIVATE_CAMERAS,(CObject*)wServerID);
	if (wServerID!=0)
	{
		Sleep(50);
		m_Servers.Disconnect(wServerID);
	}
	UpdateAllViews(NULL,VDH_REMOVE_SERVER,(CObject*)wServerID);
	if (m_Servers.GetSize() == 0)
	{
		CMainFrame* pMF = (CMainFrame*)theApp.GetMainWnd();
		if (WK_IS_WINDOW(pMF))
		{
			if (pMF->IsFullScreen())
			{
				pMF->ChangeFullScreenModus();
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::DisconnectAll()
{
	int i,c;
	CServer* pServer;
	WORD wServerID;

	c = m_Servers.GetSize();
	for (i=c-1;i>=0;i--)
	{
		pServer = m_Servers.GetAt(i);
		wServerID = pServer->GetID();
		UpdateAllViews(NULL,VDH_DEACTIVATE_CAMERAS,(CObject*)wServerID);
		Sleep(50);
		m_Servers.Disconnect(wServerID);
		UpdateAllViews(NULL,VDH_REMOVE_SERVER,(CObject*)wServerID);
	}
	WK_DELETE(m_pCIPCOutputVisionDecoder);
	WK_DELETE(m_pCIPCInputVisionCommData);
	WK_DELETE(m_pCIPCDataBaseVision);
	WK_DELETE(m_pCIPCServerControlVision);
	WK_DELETE(m_pCIPCLocalAudio);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::Remove(WORD wServerID)
{
	UpdateAllViews(NULL,VDH_REMOVE_SERVER,(CObject*)wServerID);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateDisconnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Servers.GetSize());
}
/////////////////////////////////////////////////////////////////////////////
CServer* CVisionDoc::GetServer(WORD wServerID)
{
	return m_Servers.GetServer(wServerID);
}
//////////////////////////////////////////////////////////////////////////
CServer* CVisionDoc::GetServerFromIndex(int nIndex)
{
	if (IsBetween(nIndex, 0, m_Servers.GetSize()-1))
	{
		return m_Servers.GetAt(nIndex);
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
WORD CVisionDoc::GetServerIDbyHostID(WORD wHostID, BOOL bConnect)
{
	CSecID idHost(SECID_GROUP_HOST, wHostID);
	CServer*pServer = m_Servers.GetServer(idHost);
	if (pServer)
	{
		return pServer->GetID();
	}
	else if (bConnect)
	{
		Connect(idHost);
	}
	return SECID_NO_SUBID;
}
/////////////////////////////////////////////////////////////////////////////
CServer* CVisionDoc::GetLocalServer()
{
	CServer*pServer = m_Servers.GetLocalServer();
	return pServer;
}
/////////////////////////////////////////////////////////////////////////////
CServer* CVisionDoc::GetActiveServer()
{
	int iRemote;
	iRemote = m_Servers.GetNrOfRemoteServers();

	if (iRemote==0)
	{
		return NULL;
	}
	else if (iRemote==1)
	{
		return m_Servers.GetRemoteServer();
	}
	else
	{
		CVisionView* pView = GetVisionView();
		if (WK_IS_WINDOW(pView))
		{
			// GF todo m_SmallWindows.Lock;
			CDisplayWindow* pDW = pView->GetCmdActiveDisplayWindow();
			if (WK_IS_WINDOW(pDW))
			{
				return pDW->GetServer();
			}
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
UINT CVisionDoc::ReconnectThreadProc(LPVOID pParam)
{
	CVisionDoc* pDoc = (CVisionDoc*)pParam;
	DWORD	dwR;

	while (pDoc->m_bRun)
	{
		dwR = WaitForSingleObject(pDoc->m_evClose,100);

		if (dwR == WAIT_TIMEOUT)
		{
			if (pDoc->m_bRun)
			{
				pDoc->ResetReconnect();
			}
		}
	}
	return 0x05EC;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::StartReconnectThread()
{
	m_bRun = TRUE;
	m_pReconnectThread = AfxBeginThread(ReconnectThreadProc,(LPVOID)this);
	m_pReconnectThread->m_bAutoDelete = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::StopReconnectThread()
{
	m_bRun = FALSE;

	if (m_pReconnectThread)
	{
		m_evClose.SetEvent();
		if (WAIT_TIMEOUT == WaitForSingleObject(m_pReconnectThread->m_hThread,2000))
		{
			WK_TRACE_ERROR(_T("time out waiting for end of Reconnect Thread\n"));
		}
		WK_DELETE(m_pReconnectThread);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::PictureData(WORD wServerID, const CIPCPictureRecord &pict, CSecID id)
{
	CVisionView* pView = GetVisionView();
	if (WK_IS_WINDOW(pView))
	{
        pView->PictureData(wServerID,pict,id);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::ResetReconnect()
{
	// Attention called from Sequence Thread
//	m_Servers.Sequence();
	m_bReconnect  = TRUE;
	m_bAutoLogout = TRUE;
	CWnd * pWnd = theApp.m_pMainWnd;
	if (WK_IS_WINDOW(pWnd))
	{
		pWnd->PostMessage(WM_USER);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::Reconnect()
{
	// Attention called from OnIdle
	m_bReconnect = FALSE;
	if (m_pCIPCDataBaseVision==NULL)
	{
		ConnectToDatabase();
	}
	if (m_pCIPCServerControlVision==NULL)
	{
		m_pCIPCServerControlVision = new CIPCServerControlVision(WK_SMClientControl,this);
	}
	if (m_pCIPCOutputVisionDecoder==NULL)
	{
		ConnectToDecoder();
	}
	if (m_pCIPCInputVisionCommData==NULL)
	{
		ConnectToCommData();
	}
	if (m_pCIPCLocalAudio == NULL && theApp.m_pDongle->RunAudioUnit())
	{
		ConnectToAudio();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::AddConnectionRecord(const CConnectionRecord &c)
{
	m_aConnectionRecords.SafeAddTail(new CConnectionRecord(c));
	// Post and OnIdle will do the same, react on incoming ConnectionRecord
	GetObjectView()->PostMessage(WM_USER,WPARAM_CONNECTION_RECORD);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnConnectionRecord()
{
	CConnectionRecord* pCR = m_aConnectionRecords.SafeGetAndRemoveHead();

	if (pCR)
	{
		// maybe in future other connectionrecords than
		// alarm
		if (!pCR->GetInputShm().IsEmpty() &&
			!pCR->GetOutputShm().IsEmpty())
		{
			AlarmConnection(*pCR);
		}
		else if (!pCR->GetInputShm().IsEmpty())
		{
			BOOL bFetching = FALSE;

			for (int i=0;i<m_Servers.GetSize() && !bFetching;i++)
			{
				bFetching = m_Servers.GetAt(i)->IsFetching();
			}
			
			if (!bFetching)
			{
				// gerade kein Fetch
				ControlConnection(*pCR);
			}
			else
			{
				// Connectionrecord einfach wieder hinten ran
				m_aConnectionRecords.SafeAddTail(new CConnectionRecord(*pCR));
			}
		}

		WK_DELETE(pCR);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::AlarmConnection(const CConnectionRecord& c)
{
	// gf: If it is a second call (called before the first confirmation arrived at host)
	// we may got the non-alarm server and add the alarm server again a second time...
	// .. so get the real alarm server - or not
//	CServer* pServer = m_Servers.GetServer(c.GetSourceHost());
	CServer* pServer = m_Servers.GetAlarmServer(c.GetSourceHost());
	if (pServer)
	{
		// Ignore it...
		// gf todo: But what about camID and other parameters, may differ from first call...?
/*
		if (   m_pServer->IsControl()
			&& (m_pServer->GetAlarmID().GetSubID() == c.GetCamID().GetSubID())
			)
		{
			pServer->SetAlarmID(c.GetCamID());
		}
*/
		GetCIPCServerControlVision()->DoConfirmAlarmConnection(c.GetInputShm(), 
															   c.GetOutputShm(),
															   _T(""),
															   _T(""),
															   0
#ifdef _UNICODE
															   , MAKELONG(CODEPAGE_UNICODE, 0)
#endif
															   );
		Sleep(50);
	}
	else
	{
		pServer = m_Servers.AddServer();
		pServer->AlarmConnection(c,m_pCIPCServerControlVision);
		theApp.AlarmConnection();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::ControlConnection(const CConnectionRecord& c)
{
	// immer den bestehenden Server verwenden
	CServer* pServer = m_Servers.GetServer(c.GetSourceHost());
	if (pServer)
	{
		if (pServer->IsConnected())
		{
			// already connected!
			if (!pServer->IsControl())
			{
				if (pServer->ControlConnection(c))
				{
					GetCIPCServerControlVision()->DoConfirmAlarmConnection(c.GetInputShm(), 
																		   c.GetOutputShm(),
                                                                           _T(""),
																		   _T(""),
                                                                           0
#ifdef _UNICODE
																		   , MAKELONG(CODEPAGE_UNICODE, 0)
#endif
																		   );
					CObjectView* pOV = GetObjectView();

					if (WK_IS_WINDOW(pOV))
					{
						CSecID camID;
						camID = pServer->GetOutput()->GetCamID(c.GetCamID().GetSubID());

						if (camID.IsOutputID())
						{
							pOV->SendMessage(WM_USER,
											   MAKELONG(WPARAM_CONTROL_SELECT_CAM,pServer->GetID()),
											   camID.GetID());
						}
					}
				}
				else
				{
					WK_TRACE(_T("control connection of already connected host failed %s\n"),c.GetSourceHost());
					m_pCIPCServerControlVision->DoIndicateError(0, SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT, 0);
				}
			}
			else
			{
				WK_TRACE(_T("control connection of controlled host failed %s\n"),c.GetSourceHost());
				m_pCIPCServerControlVision->DoIndicateError(0, SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT, 1);
			}
		}
		else
		{
			WK_TRACE(_T("control connection of not yet connected host failed %s\n"),c.GetSourceHost());
			m_pCIPCServerControlVision->DoIndicateError(0, SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT, 2);
		}
	}
	else
	{
		CString sValue;
		if (c.GetFieldValue(CRF_EXCLUSIVE,sValue))
		{
			if (sValue == CSD_ON)
			{
				DisconnectAll();
			}
		}
		pServer = m_Servers.AddServer();
		if (pServer->ControlConnection(c))
		{
			theApp.AlarmConnection();
		}
		else
		{
			WK_TRACE(_T("control connection of not connected host failed %s\n"),c.GetSourceHost());;
			m_pCIPCServerControlVision->DoIndicateError(0, SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT, 3);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::AutoLogout()
{
	// Attention called from OnIdle
	if (theApp.m_AutoLogout.IsApplicationOver())
	{
		if (theApp.m_pDongle->IsDemo())
		{
			theApp.m_pMainWnd->PostMessage(WM_COMMAND,ID_APP_EXIT,0L);
		}
		else
		{
			COemGuiApi::AutoExitDialog(theApp.m_pMainWnd,&theApp.m_AutoLogout,WM_COMMAND,ID_APP_EXIT,0L);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	// return TRUE to close the window
	// return FALSE if not
	if (IsDecoderConnected())
	{
		m_pCIPCOutputVisionDecoder->DoIndicateClientActive(FALSE,SECID_NO_GROUPID);
		Sleep(10);
	}

	if (!m_Servers.DisconnectAll())
	{
		return FALSE;
	}
	WK_DELETE(m_pCIPCOutputVisionDecoder);
	WK_DELETE(m_pCIPCInputVisionCommData);
	WK_DELETE(m_pCIPCDataBaseVision);
	WK_DELETE(m_pCIPCServerControlVision);
	WK_DELETE(m_pCIPCLocalAudio);
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnSequenze() 
{
	m_bSequence = !m_bSequence;
	if (!m_bSequence)
	{
		// Bei DV-Server über ISDN muss die Bildanforderung
		// für die aktuell aktive Kamera erneut angestossen werden
		CServer* pServer;
		CIPCOutputVision* pOutput;
		for (int i=0 ; i<m_Servers.GetSize() ; i++)
		{
			pServer = m_Servers.GetAt(i);
			if (   pServer->IsDTS()
				&& pServer->IsISDN()
				)
			{
				pOutput = pServer->GetOutput();
				if (pOutput)
				{
					pOutput->RestartJpegEncoding();
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateSequenzePane(CCmdUI* pCmdUI) 
{
	CString sL;
	CString sF;
	if (m_bSequence)
	{
		sL.LoadString(ID_SEQUENCE_PANE);
		sF.Format(sL,m_dwSequenceIntervall);
	}
	else
	{
		sF.LoadString(IDS_SEQUENCE_OFF);
	}
	pCmdUI->SetText(sF);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::PictureStored()
{
	m_dwLastStored = GetTickCount();
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateDatabasePane(CCmdUI* pCmdUI) 
{
	CString sL;
	if (IsDatabaseConnected())
	{
		if (   (m_dwLastStored!=0)
			&& (GetTimeSpan(m_dwLastStored)<200)
			)
		{
			sL.LoadString(IDS_STORING);
		}
		else
		{
			sL.LoadString(IDS_POSSIBLE);
		}
	}
	else
	{
		sL.LoadString(ID_DATABASE_PANE);
	}
	pCmdUI->SetText(sL);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::SetSequence(BOOL bOn)
{
	m_bSequence = bOn;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::SetSequenceTime(DWORD dwSeconds)
{
	m_dwSequenceIntervall = dwSeconds;
	theApp.m_dwSequenceIntervall = m_dwSequenceIntervall;
	m_bSequence  = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnSequence1() 
{
	SetSequenceTime(1);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateSequence1(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_Servers.GetSize()
				   && (!m_Servers.IsAnyB3Connection())
				   && m_bSequence);
	pCmdUI->SetCheck(m_bSequence && (m_dwSequenceIntervall == 1));
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnSequence10() 
{
	SetSequenceTime(10);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateSequence10(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Servers.GetSize() && m_bSequence);
	pCmdUI->SetCheck(m_bSequence && (m_dwSequenceIntervall == 10));
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnSequence15() 
{
	SetSequenceTime(15);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateSequence15(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Servers.GetSize() && m_bSequence);
	pCmdUI->SetCheck(m_bSequence && (m_dwSequenceIntervall == 15));
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnSequence2() 
{
	SetSequenceTime(2);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateSequence2(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_Servers.GetSize()
				   && (!m_Servers.IsAnyB3Connection())
				   && m_bSequence);
	pCmdUI->SetCheck(m_bSequence && (m_dwSequenceIntervall == 2));
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnSequence20() 
{
	SetSequenceTime(20);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateSequence20(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Servers.GetSize() && m_bSequence);
	pCmdUI->SetCheck(m_bSequence && (m_dwSequenceIntervall == 20));
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnSequence3() 
{
	SetSequenceTime(3);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateSequence3(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_Servers.GetSize()
				   && (!m_Servers.IsAnyB3Connection())
				   && m_bSequence);
	pCmdUI->SetCheck(m_bSequence && (m_dwSequenceIntervall == 3));
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnSequence30() 
{
	SetSequenceTime(30);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateSequence30(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Servers.GetSize() && m_bSequence);
	pCmdUI->SetCheck(m_bSequence && (m_dwSequenceIntervall == 30));
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnSequence4() 
{
	SetSequenceTime(4);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateSequence4(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(   m_Servers.GetSize()
				   && (!m_Servers.IsAnyB3Connection())
				   && m_bSequence);
	pCmdUI->SetCheck(m_bSequence && (m_dwSequenceIntervall == 4));
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnSequence5() 
{
	SetSequenceTime(5);
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateSequence5(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Servers.GetSize() && m_bSequence);
	pCmdUI->SetCheck(m_bSequence && (m_dwSequenceIntervall == 5));
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionDoc::GetStore()
{
	return m_bStore;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVisionDoc::GetSequence()
{
	return m_bSequence;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CVisionDoc::GetSequenceIntervall()
{
	return m_dwSequenceIntervall;
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnVideoSaveAll() 
{
	m_bStore = TRUE;
	CVisionView* pView = GetVisionView();
	if (pView)
	{
		pView->SetStore(m_bStore);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnVideoSaveAllStop() 
{
	m_bStore = FALSE;
	CVisionView* pView = GetVisionView();
	if (pView)
	{
		pView->SetStore(m_bStore);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnVideoSaveAllToggle() 
{
	m_bStore = !m_bStore;
	CVisionView* pView = GetVisionView();
	if (pView)
	{
		pView->SetStore(m_bStore);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnUpdateVideoSaveAll(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(IsDatabaseConnected());
}
////////////////////////////////////////////////////////////////////////////
void CVisionDoc::ResetConnectionAutoLogout() 
{
	m_Servers.ResetConnectionAutoLogout();
}
////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnDatabaseConnected()
{
}
////////////////////////////////////////////////////////////////////////////
void CVisionDoc::OnDatabaseDisconnected()
{
	if (   m_pCIPCDataBaseVision
		&& m_pCIPCDataBaseVision->GetIsMarkedForDelete())
	{
		WK_DELETE(m_pCIPCDataBaseVision);
	}
}
