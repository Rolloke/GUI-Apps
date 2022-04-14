// IdipClientDoc.cpp : implementation of the CIdipClientDoc class
//

#include "stdafx.h"
#include "IdipClient.h"
#include "IdipClientDoc.h"

#include "ViewIdipClient.h"
#include "ViewCamera.h"
#include "ViewRelais.h"
#include "ViewAlarmList.h"
#include "ViewAudio.h"
#include "ViewArchive.h"

#include "DlgBarIdipClient.h"

#include "Mainfrm.h"

#include "WndLive.h"
#include "CIPCDatabaseIdipClientLocal.h"
#include "CIPCServerControlIdipClient.h"
#include "CIPCInputIdipClientCommData.h"
#include "CIPCOutputIdipClientDecoder.h"
#include "CIPCAudioIdipClient.h"

#include "wkclasses\wk_utilities.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStringHint
IMPLEMENT_DYNAMIC(CStringHint, CObject)
CStringHint::CStringHint(CString sMsg)
{
	m_sMessage = sMsg;
}
/////////////////////////////////////////////////////////////////////////////
CString CStringHint::GetMessage()
{
	return m_sMessage;
}

/////////////////////////////////////////////////////////////////////////////
// CIdipClientDoc

IMPLEMENT_DYNCREATE(CIdipClientDoc, CDocument)

BEGIN_MESSAGE_MAP(CIdipClientDoc, CDocument)
	//{{AFX_MSG_MAP(CIdipClientDoc)
	ON_COMMAND(ID_CONNECT_LOCAL, OnConnectLocal)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_LOCAL, OnUpdateConnectLocal)
	ON_UPDATE_COMMAND_UI_RANGE(ID_CONNECT_LOW, ID_CONNECT_HIGH, OnUpdateConnect)
	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_UPDATE_COMMAND_UI(ID_CONNECT, OnUpdateConnect)
	ON_UPDATE_COMMAND_UI(ID_DISCONNECT, OnUpdateDisconnect)
	ON_UPDATE_COMMAND_UI(ID_SEQUENCE_PANE, OnUpdateSequenzePane)
	ON_UPDATE_COMMAND_UI(IDC_BTN_SEQUENCE, OnUpdateSequenzePane)
	ON_UPDATE_COMMAND_UI(ID_DATABASE_PANE, OnUpdateDatabasePane)
	ON_UPDATE_COMMAND_UI(IDC_BTN_SAVE_STATE, OnUpdateDatabasePane)
	ON_COMMAND(ID_SEQUENZE, OnSequenze)
	ON_COMMAND(ID_VIDEO_SAVE_ALL, OnVideoSaveAll)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SAVE_ALL, OnUpdateVideoSaveAll)
	ON_COMMAND(ID_CONNECT_MAP, OnConnectMap)
	ON_COMMAND(ID_VIDEO_SAVE_ALL_STOP, OnVideoSaveAllStop)
	ON_COMMAND(ID_VIDEO_SAVE_ALL_TOGGLE, OnVideoSaveAllToggle)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_MAP, OnUpdateConnectMap)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_FILE_OPEN, OnUpdateFileOpen)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_SAVE_ALL_STOP, OnUpdateVideoSaveAllStop)
	ON_COMMAND(ID_FILE_PRINT, OnFilePrint)
	ON_COMMAND_RANGE(ID_SEQUENCE_1, ID_SEQUENCE_30, OnSequenceX)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SEQUENCE_1, ID_SEQUENCE_30, OnUpdateSequenceX)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CIdipClientDoc construction/destruction

CIdipClientDoc::CIdipClientDoc()
{
	m_bRun = FALSE;

	m_pCIPCServerControlIdipClient = NULL;

	m_pReconnectThread = NULL;
	m_bReconnectFinished = FALSE;
	m_pCIPCDatabaseLocal = NULL;
	m_pCIPCInputIdipClientCommData = NULL;
	m_pCIPCLocalAudio = NULL;
	m_pDataCarrier = NULL;
	m_dwLastReconnect = GetTickCount();
	m_dwReconnectIntervall = 2*1000;
	
	m_pFetchResultLocalInput = NULL;
	m_pFetchResultLocalDatabase = NULL;
	m_pFetchResultLocalAudio = NULL;
	m_pFetchResultLocalDataCarrier = NULL;

	m_bAutoLogout = theApp.m_AutoLogout.GetAutoApplication();
	m_bSequence = FALSE;
	m_dwSequenceIntervall = theApp.m_dwSequenceIntervall;
	m_Servers.SetDocument(this);
	m_dwLastBeep = 0;
	m_bStore = FALSE;
	m_dwLastStored = 0;
	
	m_pConnectionDlg = NULL;
}
/////////////////////////////////////////////////////////////////////////////
CIdipClientDoc::~CIdipClientDoc()
{
	StopReconnectThread();

	WK_DELETE(m_pConnectionDlg);
	WK_DELETE(m_pCIPCServerControlIdipClient);

	WK_DELETE(m_pCIPCLocalAudio);
	WK_DELETE(m_pCIPCDatabaseLocal);
	WK_DELETE(m_pCIPCInputIdipClientCommData);
	WK_DELETE(m_pDataCarrier);

	WK_DELETE(m_pFetchResultLocalAudio);
	WK_DELETE(m_pFetchResultLocalDatabase);
	WK_DELETE(m_pFetchResultLocalInput);
	WK_DELETE(m_pFetchResultLocalDataCarrier);

	m_aConnectionRecords.SafeDeleteAll();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CIdipClientDoc serialization

void CIdipClientDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{

	}
	else
	{

	}
}

/////////////////////////////////////////////////////////////////////////////
// CIdipClientDoc diagnostics

#ifdef _DEBUG
void CIdipClientDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CIdipClientDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientDoc::IsLocalDatabaseConnected()
{
	BOOL bRet = FALSE;
	if (m_pCIPCDatabaseLocal)
	{
		bRet = (m_pCIPCDatabaseLocal->GetIPCState() == CIPC_STATE_CONNECTED);
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CIPCDatabaseIdipClientLocal* CIdipClientDoc::GetLocalDatabase()
{
	if (IsLocalDatabaseConnected())
	{
		return m_pCIPCDatabaseLocal;
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
// Local Connection to Database !!
void CIdipClientDoc::ConnectToLocalDatabase()
{
	if (WK_IS_RUNNING(WK_APP_NAME_DB_SERVER) == FALSE) 
	{
		return; // no chance without DatabaseServer.exe
	}

	if (NULL == theApp.m_pUser)
	{
		return; // no chance without user
	}

	if (IsLocalDatabaseConnected())
	{
		return;	// no need to connect twice
	}

	if (!theApp.IsDataBasePresent())
	{
		return;	// no chance without data base
	}

	CConnectionRecord c(LOCAL_LOOP_BACK,
		*theApp.m_pUser,
		theApp.m_pPermission->GetName(),
		theApp.m_pPermission->GetPassword()
		);
	CString sCodePage;
	sCodePage.Format(_T("%d"), CODEPAGE_UNICODE);
	c.SetFieldValue(CRF_CODEPAGE, sCodePage);

	CString sSmName;
	CIPCFetch fetch;
	TRACE(_T("connect to local database\n"));
	CIPCFetchResult fr = fetch.FetchDatabase(c);
	TRACE(_T("connected to local database\n"));
	m_pFetchResultLocalDatabase = new CIPCFetchResult(fr);
}
//////////////////////////////////////////////////////////////////////
void CIdipClientDoc::ConnectToDataCarrier()
{
	if (IsDataCarrierConnected())
	{
		return;
	}
	if (NULL == theApp.m_pUser)
	{
		return;	// no chance without user
	}
	if (theApp.m_pDongle == NULL)
	{
		return;	// no dongle
	}
	if (   !theApp.IsReadOnlyModus()
		&& !theApp.m_pDongle->RunCDRWriter())
	{
		return;	// not allowed
	}
	if (WK_IS_RUNNING(WK_APP_NAME_ACDC)==FALSE) 
	{
		return; // no chance without the AC/DC
	}

	CConnectionRecord c(LOCAL_LOOP_BACK,
		*theApp.m_pUser,
		theApp.m_pPermission->GetName(),
		theApp.m_pPermission->GetPassword());
	CString sCodePage;
	sCodePage.Format(_T("%d"), CODEPAGE_UNICODE);
	c.SetFieldValue(CRF_CODEPAGE, sCodePage);

	CString sSmName;
	CIPCFetch fetch;
	TRACE(_T("connect to data carrier\n"));
	CIPCFetchResult fr = fetch.FetchDataCarrier(c);
	TRACE(_T("connected to data carrier\n"));

	m_pFetchResultLocalDataCarrier = new CIPCFetchResult(fr);
}
/////////////////////////////////////////////////////////////////////////////
// Local Connection to Audio Unit !!
void CIdipClientDoc::ConnectToAudio()
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
	if (   !theApp.IsReadOnlyModus()
		&& !theApp.m_pDongle->RunAudioUnit())
	{
		return;	// not allowed
	}
	if (WK_IS_RUNNING(WK_APP_NAME_AUDIOUNIT1)==FALSE) 
	{
		return; // no chance without the AudioUnit
	}

	CConnectionRecord c(LOCAL_LOOP_BACK,
						*theApp.m_pUser,
						theApp.m_pPermission->GetName(),
						theApp.m_pPermission->GetPassword());
	CString sCodePage;
	sCodePage.Format(_T("%d"), CODEPAGE_UNICODE);
	c.SetFieldValue(CRF_CODEPAGE, sCodePage);

	CString sSmName;
	CIPCFetch fetch;
	TRACE(_T("connect to audio\n"));
	CIPCFetchResult fr = fetch.FetchAudio(c);
	TRACE(_T("connected to audio\n"));

	m_pFetchResultLocalAudio = new CIPCFetchResult(fr);

//#endif
}
//////////////////////////////////////////////////////////////////////////
BOOL CIdipClientDoc::IsAudioConnected()
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
//////////////////////////////////////////////////////////////////////////
BOOL CIdipClientDoc::IsDataCarrierConnected()
{
	BOOL bRet = FALSE;
	if (m_pDataCarrier)
	{
		bRet = (m_pDataCarrier->GetIPCState() == CIPC_STATE_CONNECTED);
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CIPCDataCarrierIdipClient* CIdipClientDoc::GetDataCarrier()
{
	if (IsDataCarrierConnected())
	{
		return m_pDataCarrier;
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
CIPCAudioIdipClient* CIdipClientDoc::GetLocalAudio()
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
// Local Connection !!
void CIdipClientDoc::ConnectToCommData()
{
	if (   WK_IS_RUNNING(WK_APP_NAME_SERVER)==FALSE
		|| NULL==theApp.m_pUser
		|| !theApp.m_bRS232Transparent
		|| IsCommDataConnected()) 
	{
		return; // no chance 
	}

	WK_TRACE(_T("connecting to local input for comm data\n"));
	CConnectionRecord c(LOCAL_LOOP_BACK,
		*theApp.m_pUser,
		theApp.m_pPermission->GetName(),
		theApp.m_pPermission->GetPassword()
		);
	CString sCodePage;
	sCodePage.Format(_T("%d"), CODEPAGE_UNICODE);
	c.SetFieldValue(CRF_CODEPAGE, sCodePage);

	CString sSmName;
	CIPCFetch fetch;
	TRACE(_T("connect to comm data\n"));
	CIPCFetchResult fr = fetch.FetchInput(c);
	TRACE(_T("connected to comm data\n"));
	m_pFetchResultLocalInput = new CIPCFetchResult(fr);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientDoc::IsCommDataConnected()
{
	BOOL bRet = FALSE;
	if (m_pCIPCInputIdipClientCommData)
	{
		bRet = (m_pCIPCInputIdipClientCommData->GetIPCState() == CIPC_STATE_CONNECTED);
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
CIPCInputIdipClientCommData* CIdipClientDoc::GetCIPCInputIdipClientCommData()
{
	if (IsCommDataConnected())
	{
		return m_pCIPCInputIdipClientCommData;
	}
	else
	{
		return NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
CIPCServerControlIdipClient* CIdipClientDoc::GetCIPCServerControlIdipClient()
{
	return m_pCIPCServerControlIdipClient;
}
/*
/////////////////////////////////////////////////////////////////////////////
CViewIdipClient* CIdipClientDoc::GetViewIdipClient()
{
	CMainFrame*pMF = theApp.GetMainFrame();
	if (pMF)
	{
		return pMF->GetViewIdipClient();
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CViewCamera* CIdipClientDoc::GetViewCamera()
{
	CMainFrame*pMF = theApp.GetMainFrame();
	if (pMF)
	{
		return pMF->GetViewCamera();
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CViewAudio* CIdipClientDoc::GetViewAudio()
{
	CMainFrame*pMF = theApp.GetMainFrame();
	if (pMF)
	{
		return pMF->GetViewAudio();
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CViewRelais* CIdipClientDoc::GetViewRelais()
{
	CMainFrame*pMF = theApp.GetMainFrame();
	if (pMF)
	{
		return pMF->GetViewRelais();
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CViewAlarmList* CIdipClientDoc::GetViewAlarmList()
{
	CMainFrame*pMF = theApp.GetMainFrame();
	if (pMF)
	{
		return pMF->GetViewAlarmList();
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CViewArchive* CIdipClientDoc::GetViewArchive()
{
	CMainFrame*pMF = theApp.GetMainFrame();
	if (pMF)
	{
		return pMF->GetViewArchive();
	}
	return NULL;
}
*/
/////////////////////////////////////////////////////////////////////////////
// CIdipClientDoc commands
void CIdipClientDoc::OnIdle()
{
	m_Servers.OnIdle();

	Reconnect();

	if (m_bAutoLogout)
	{
		theApp.AutoLogout();
	}
	if (m_aConnectionRecords.GetCount())
	{
		OnConnectionRecord();
	}
	if (    m_pCIPCInputIdipClientCommData!=NULL 
		&& (m_pCIPCInputIdipClientCommData->GetIPCState() == CIPC_STATE_DISCONNECTED))
	{
		WK_DELETE(m_pCIPCInputIdipClientCommData);
	}
	if (    m_pCIPCLocalAudio!=NULL 
		&& (m_pCIPCLocalAudio->GetIPCState() == CIPC_STATE_DISCONNECTED))
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
		if (theApp.IsUserAlarm())
		{
			CMainFrame* pMainFrame = theApp.GetMainFrame();
			if (pMainFrame)
			{
				pMainFrame->PostCommand(ID_FORCE_NEW_LOGIN);
			}
		}
	}

// in Recherche the local server is always opened
// at least the Read-Only version has to connect automatically
	if (theApp.IsReadOnlyModus())
	{
		if (GetLocalServer() == NULL)
		{
			OnConnectLocal();
		}
		if (m_pCIPCLocalAudio == NULL && WK_IS_RUNNING(WK_APP_NAME_AUDIOUNIT1))
		{
			CString str;
			str.Format(SM_AudioUnitMedia, 1);
			m_pCIPCLocalAudio = new CIPCAudioIdipClient(str, this, TRUE);
		}
	}
	else
	{
		// check for disconnected data carrier
		OnLocalDataCarrierDisconnected();
	}


	Sleep(0);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::AlarmAnimation()
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
		if (GetTimeSpanSigned(m_dwLastBeep, dwTick) > (10 * 1000))
		{
			m_dwLastBeep = dwTick;
			bBeep = TRUE;
		}
	}
	if (bBeep)
	{
		MessageBeep(MB_ICONEXCLAMATION);	// Hinweis
// MessageBeep will only be played on sound card, Beep plays always on speaker
		Beep(1000, 100);
		Beep( 440, 100);
		Beep(1000, 100);

	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnConnectLocal() 
{
	if (m_Servers.GetLocalServer())
	{
		return;
	}
	if (   (WK_IS_RUNNING(WK_APP_NAME_SERVER)==FALSE) 
		&& (WK_IS_RUNNING(WK_APP_NAME_DB_SERVER)==FALSE)
		)
	{
		return; // no chance without any server
	}

	if (theApp.m_pUser == NULL)
	{
		// no user no connection
		return;
	}


	CServer* pServer;
	CHostArray& ha = theApp.GetHosts();

	CSecID idHost = ha.GetLocalHost()->GetID();

	pServer = m_Servers.GetServer(idHost);
	if (pServer==NULL)
	{
		pServer = m_Servers.AddServer();
		pServer->Connect(ha.GetLocalHost()->GetName(),LOCAL_LOOP_BACK, idHost, 10*1000);
		if (m_pCIPCLocalAudio)
		{
			m_pCIPCLocalAudio->SetServerID(pServer->GetID());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnUpdateConnectLocal(CCmdUI* pCmdUI) 
{
	BOOL bEnable = (   (m_Servers.GetLocalServer()==NULL)
					&& (WK_IS_RUNNING(WK_APP_NAME_SERVER))
					&& theApp.IsValidUserLoggedIn()
					);
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientDoc::Connect(CSecID idHostID)
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
		CHostArray& ha = theApp.GetHosts();
		CHost* pHost;
		pHost = ha.GetHost(idHostID);
		if (pHost)
		{
			CServer* pServer;
			pServer = m_Servers.GetServer(idHostID);
			if (pServer==NULL)
			{
				if (pHost->IsISDN())
				{
					HWND hWnd = FindWindow(WK_APP_NAME_LAUNCHER, NULL);
					if (hWnd)
					{
						::PostMessage(hWnd, LAUNCHER_APPLICATION, WAI_ISDN_UNIT, MAKELONG(WM_ISDN_B_CHANNELS, 0));
					}
				}
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
void CIdipClientDoc::OnConnect() 
{
	CHostArray &Hosts = theApp.GetHosts();
	if (theApp.GetIntValue(_T("ConnectMenu"), TRUE))
	{
		CSkinMenu menu;

		int i, c, n;
		CHost* pHost;
		c = Hosts.GetSize();

		for (i=0, n=0; i<c; i++)	// count the real number of hosts
		{
			pHost = Hosts.GetAtFast(i);
			if (!pHost->IsSMS() && !pHost->IsEMail() && !pHost->IsFAX())
			{
				n++;
			}
		}

		if (IsBetween(n, 1, 16))
		{
			CString sText;
			CString sLocal;
			CRect rc;
			CPoint pt;
			CWnd *pBtn = theApp.GetMainFrame()->GetDlgBarIdipClient()->CWnd::GetDlgItem(ID_CONNECT);
			GetCursorPos(&pt);
			if (pBtn && pBtn->IsWindowVisible())
			{
				pBtn->GetWindowRect(&rc);
				if (rc.PtInRect(pt))
				{
					pt = rc.TopLeft();
				}
			}

			menu.CreatePopupMenu();

			if (!theApp.IsReceiver())
			{
				sLocal.LoadString(IDS_LOCAL_SERVER);
				pHost = Hosts.GetLocalHost();
				sText.Format(_T("&%s %s"), sLocal, pHost->GetName());
				menu.AppendMenu(MF_STRING, ID_CONNECT_LOCAL, sText);
			}

			for (i=0, n=1; i<c; i++)
			{
				pHost = Hosts.GetAtFast(i);
				if (!pHost->IsSMS() && !pHost->IsEMail() && !pHost->IsFAX())
				{
					sText.Format(_T("&%X %s (%s)"), n++, pHost->GetName(),pHost->GetTyp());
					menu.AppendMenu(MF_STRING, pHost->GetID().GetSubID()+ID_CONNECT_LOW, sText);
				}
			}

			menu.ConvertMenu(TRUE);
			menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_BOTTOMALIGN | TPM_LEFTBUTTON, pt.x, pt.y, theApp.GetMainFrame());
			menu.ConvertMenu(FALSE);
			return;
		}
	}

	if (m_pConnectionDlg == NULL)
	{
		m_pConnectionDlg = new COEMConnectionDialog(Hosts, AfxGetMainWnd());
	}

	m_pConnectionDlg->m_sFilterTypes = _T("MINI B3,MINI B6,SMS,FAX,E-Mail,TeleObserver");
	m_pConnectionDlg->m_sHost.Empty();
	m_pConnectionDlg->m_sTelefon.Empty();
	m_pConnectionDlg->m_idHost.Set(SECID_NO_ID);

	INT_PTR nResult = m_pConnectionDlg->DoModal();

	if (IDOK==nResult)
	{
		CServer* pServer = m_Servers.GetServer(m_pConnectionDlg->m_idHost);
		if (pServer==NULL)
		{
			pServer = m_Servers.AddServer();
			pServer->Connect(m_pConnectionDlg->m_sHost, m_pConnectionDlg->m_sTelefon, m_pConnectionDlg->m_idHost,0);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::CloseConnectionDlg()
{
	if (   m_pConnectionDlg
		&& m_pConnectionDlg->m_hWnd)
	{
		TRACE(_T("CloseConnectionDlg\n"));
		m_pConnectionDlg->PostMessage(WM_COMMAND, IDCANCEL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnConnectMap() 
{
	CViewIdipClient* pVV = theApp.GetMainFrame()->GetViewIdipClient();
	if (pVV)
	{
		pVV->ShowMapWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnUpdateConnectMap(CCmdUI* pCmdUI) 
{
	BOOL bEnable = (   !theApp.IsReadOnlyModus()
					&&  theApp.AllowHTMLmaps()
					&&  theApp.IsValidUserLoggedIn()
					&& !theApp.GetMapURL().IsEmpty());
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnUpdateConnect(CCmdUI* pCmdUI) 
{
	BOOL bEnable = FALSE;
	if (pCmdUI->m_nID == ID_CONNECT)
	{
		bEnable = (   (theApp.IsValidUserLoggedIn())
					&& theApp.m_bRunAnyLinkUnit
					&& !m_Servers.IsAnyServerFetching()
					&& (theApp.GetMainFrame()->GetViewIdipClient()->GetDlgBackup() == NULL)
					);
	}
	else
	{
		CSecID idHost(SECID_GROUP_HOST,(WORD)(pCmdUI->m_nID-ID_CONNECT_LOW));
		bEnable = m_Servers.GetServer(idHost) == NULL;
	}
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::ActualizeInput(CIPCInputIdipClient* pInput)
{
	UpdateAllViews(NULL, VDH_INPUT, pInput);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::ActualizeOutput(CIPCOutputIdipClient* pOutput)
{
	UpdateAllViews(NULL, VDH_OUTPUT, pOutput);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::ActualizeAudio(CIPCAudioIdipClient* pAudio)
{
	UpdateAllViews(NULL, VDH_AUDIO, pAudio);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::ActualizeDatabase(CIPCDatabaseIdipClient* pDatabase)
{
	UpdateAllViews(NULL, VDH_DATABASE, pDatabase);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::Disconnect(WORD wServerID, BOOL bAllowRequest/*=TRUE*/)
{
	if (bAllowRequest && theApp.m_bConfirmDisconnect)
	{
		CServer *pServer = GetServer(wServerID);
		if (pServer)
		{
			CString sDisconnect;
			sDisconnect.Format(IDS_REQUEST_DISCONNECT, pServer->GetName());
			if (IDYES != AfxMessageBox(sDisconnect, MB_YESNO|MB_ICONQUESTION))
			{
				return;
			}
		}
	}
	UpdateAllViews(NULL, MAKELONG(VDH_DEACTIVATE_CAMERAS,wServerID), NULL);
	UpdateAllViews(NULL, MAKELONG(VDH_REMOVE_SERVER, wServerID), NULL);
	if (wServerID!=0)
	{
		Sleep(50);
		m_Servers.Disconnect(wServerID);
	}

	CMainFrame* pMF = theApp.GetMainFrame();
	if (pMF && theApp.m_dwClosing == 0)
	{
		pMF->PostMessage(WM_USER, INIT_FIELDS);
		if (m_Servers.GetSize() == 0)
		{
			pMF->ChangeFullScreenModus(FALSE|FULLSCREEN_FORCE);
			CSyncCoolBar* pSCB = pMF->GetSyncCoolBar();
			if (pSCB)
			{
				pSCB->DeleteArchive(wServerID,0,FALSE);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientDoc::DisconnectAll(BOOL bExceptAlarm, BOOL bAsk/*=FALSE*/)
{
	BOOL bAllDisconnected = FALSE;

	int i,c;
	CServer* pServer;

	if (bAsk)
	{
		pServer = m_Servers.GetLocalServer();
		if (pServer)
		{
			CIPCOutputIdipClient *pOutput = pServer->GetOutput();
			if (   pOutput 
				&& !pOutput->ReferenceComplete()
				&& IDNO==AfxMessageBox(IDP_NOT_ALL_REFERENCE,MB_YESNO))
			{
				return FALSE;
			}
		}
	}

	c = m_Servers.GetSize();
	for (i=c-1;i>=0;i--)
	{
		pServer = m_Servers.GetAtFast(i);
		if (bExceptAlarm && pServer->IsAlarm())
		{
			// ignore, do not disconnect
			continue;
		}
		else
		{
			Disconnect(pServer->GetID(), FALSE);
		}
	}

	bAllDisconnected = (m_Servers.GetSize() == 0);

	return bAllDisconnected;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::DisconnectRemote()
{
	CServer* pServer;

	for (int i=0;i<m_Servers.GetSize();i++)
	{
		pServer = m_Servers.GetAtFast(i);
		if (!pServer->IsLocal())
		{
			Disconnect(pServer->GetID(), FALSE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::DeleteArchivFiles()
{
	int i,c;
	CServer* pServer;

	c = m_Servers.GetSize();
	for (i=0;i<c;i++)
	{
		pServer = m_Servers.GetAtFast(i);
		if (pServer && pServer->IsConnectedPlay())
		{
			pServer->DeleteArchivFiles();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::StartSearch()
{
	int i,c;
	CServer* pServer;

	c = m_Servers.GetSize();
	for (i=0;i<c;i++)
	{
		pServer = m_Servers.GetAtFast(i);
		if (pServer && pServer->IsConnectedPlay())
		{
			pServer->StartSearch();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::StopSearch()
{
	int i,c;
	CServer* pServer;

	c = m_Servers.GetSize();
	for (i=0;i<c;i++)
	{
		pServer = m_Servers.GetAtFast(i);
		if (pServer && pServer->IsConnectedPlay())
		{
			pServer->StopSearch();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::StopSearchCopy()
{
	int i,c;
	CServer* pServer;

	c = m_Servers.GetSize();
	for (i=0;i<c;i++)
	{
		pServer = m_Servers.GetAtFast(i);
		if (pServer && pServer->IsConnectedPlay())
		{
			pServer->StopSearchCopy();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::Remove(WORD wServerID)
{
	// Recherche calls UpdateMyViews!
	UpdateAllViews(NULL, MAKELONG(VDH_REMOVE_SERVER, wServerID), NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnUpdateDisconnect(CCmdUI* pCmdUI) 
{
	BOOL bEnable = (  /* theApp.IsValidUserLoggedIn()
					&& */(theApp.IsReadOnlyModus() == FALSE)
					&& (m_Servers.GetSize() > 0)
					&& (theApp.GetMainFrame()->GetViewIdipClient()->GetDlgBackup() == NULL)
					);
	pCmdUI->Enable(bEnable);
}
/////////////////////////////////////////////////////////////////////////////
CServer* CIdipClientDoc::GetServer(WORD wServerID)
{
	return m_Servers.GetServer(wServerID);
}
//////////////////////////////////////////////////////////////////////////
CServer* CIdipClientDoc::GetServerFromIndex(int nIndex)
{
	if (IsBetween(nIndex, 0, m_Servers.GetSize()-1))
	{
		return m_Servers.GetAtFast(nIndex);
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
WORD CIdipClientDoc::GetServerIDbyHostID(WORD wHostID, BOOL bConnect)
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
CServer* CIdipClientDoc::GetLocalServer()
{
	CServer*pServer = m_Servers.GetLocalServer();
	return pServer;
}
/////////////////////////////////////////////////////////////////////////////
CServer* CIdipClientDoc::GetActiveServer()
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
		CViewIdipClient* pView = theApp.GetMainFrame()->GetViewIdipClient();
		if (pView)
		{
			pView->LockSmallWindows(_T(__FUNCTION__));
			CWndLive* pLive = pView->GetWndLiveCmdActive();
			CServer *pActiveServer = NULL;
			if (pLive)
			{
				pActiveServer = pLive->GetServer();
			}
			pView->UnlockSmallWindows();
			return pActiveServer;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
UINT CIdipClientDoc::ReconnectThreadProc(LPVOID pParam)
{
	CIdipClientDoc* pDoc = (CIdipClientDoc*)pParam;
	XTIB::SetThreadName(_T("CIdipClientDoc::ReconnectThreadProc"));

	TRACE(_T("ReconnectThreadProc start\n"));
	pDoc->DoReconnect();
	TRACE(_T("ReconnectThreadProc ende\n"));

	return 0x05EC;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::DoReconnect()
{
	if (   m_pCIPCDatabaseLocal == NULL
		&& m_bRun)
	{
		ConnectToLocalDatabase();
	}
	if (   m_pCIPCInputIdipClientCommData == NULL
		&& m_bRun)
	{
		ConnectToCommData();
	}
	if (   m_pCIPCLocalAudio == NULL
		&& m_bRun)
	{
		ConnectToAudio();
	}
	if (   m_pDataCarrier == NULL
		&& m_bRun)
	{
		ConnectToDataCarrier();
	}
	m_bReconnectFinished = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::StartReconnectThread()
{
	m_bRun = TRUE;
	m_pReconnectThread = AfxBeginThread(ReconnectThreadProc,this);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::StopReconnectThread()
{
	m_bRun = FALSE;

	if (m_pReconnectThread)
	{
		Sleep(100);
		m_pReconnectThread = NULL;
	}
	m_bReconnectFinished = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::ResetReconnect()
{
	// Attention called from Sequence Thread
	m_bAutoLogout = TRUE;
	theApp.GetMainFrame()->PostMessage(WM_USER);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::Reconnect()
{
	if (  m_pCIPCServerControlIdipClient==NULL	&& !theApp.IsReadOnlyModus())
	{
		m_pCIPCServerControlIdipClient = new CIPCServerControlIdipClient(WK_SMClientControl,this);
	}

	// Attention called from OnIdle
	if (   theApp.m_pUser
		&& theApp.m_pDongle
		&& !theApp.IsReadOnlyModus()
		&& GetTimeSpanSigned(m_dwLastReconnect) > (int)m_dwReconnectIntervall)
	{
		// alle 15 Sekunden checken ob wir mal reconnecten müssen
		m_dwReconnectIntervall = 15*1000;

		if (   (m_pCIPCDatabaseLocal == NULL && WK_IS_RUNNING(WK_APP_NAME_DB_SERVER))
			|| (m_pCIPCInputIdipClientCommData == NULL && theApp.m_bRS232Transparent && WK_IS_RUNNING(WK_APP_NAME_COMMUNIT))
			|| (m_pCIPCLocalAudio == NULL && WK_IS_RUNNING(WK_APP_NAME_AUDIOUNIT1))
			|| (m_pDataCarrier == NULL && WK_IS_RUNNING(WK_APP_NAME_ACDC))
			)
		{
			if (m_pReconnectThread == NULL)
			{
				m_bReconnectFinished = FALSE;
				StartReconnectThread();
			}
		}
		m_dwLastReconnect = GetTickCount();
	}
	if (m_pFetchResultLocalAudio)
	{
		if (m_pFetchResultLocalAudio->IsOkay())
		{
			m_pCIPCLocalAudio = new CIPCAudioIdipClient(m_pFetchResultLocalAudio->GetShmName(), this);
			CServer *pServer = GetLocalServer();
			if (pServer)
			{
				m_pCIPCLocalAudio->SetServerID(pServer->GetID());
			}
		}
		Sleep(0);
		WK_DELETE(m_pFetchResultLocalAudio);
	}
	if (m_pFetchResultLocalDatabase)
	{
		if (m_pFetchResultLocalDatabase->IsOkay())
		{
			m_pCIPCDatabaseLocal = new CIPCDatabaseIdipClientLocal(this, m_pFetchResultLocalDatabase->GetShmName());
		}
		Sleep(0);
		WK_DELETE(m_pFetchResultLocalDatabase);
	}
	if (m_pFetchResultLocalInput)
	{
		if (m_pFetchResultLocalInput->IsOkay())
		{
			m_pCIPCInputIdipClientCommData = new CIPCInputIdipClientCommData(m_pFetchResultLocalInput->GetShmName(),this);
			m_pCIPCInputIdipClientCommData->SetCodePage(m_pFetchResultLocalInput->GetCodePage());
		}
		Sleep(0);
		WK_DELETE(m_pFetchResultLocalInput);
	}
	if (m_pFetchResultLocalDataCarrier)
	{
		if (m_pFetchResultLocalDataCarrier->IsOkay())
		{
			m_pDataCarrier = new CIPCDataCarrierIdipClient(m_pFetchResultLocalDataCarrier->GetShmName(),this,m_pFetchResultLocalDataCarrier->GetAssignedID());
			m_pDataCarrier->SetCodePage(m_pFetchResultLocalDataCarrier->GetCodePage());
		}
		Sleep(0);
		WK_DELETE(m_pFetchResultLocalDataCarrier);
	}

	if (m_bReconnectFinished)
	{
		StopReconnectThread();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::AddConnectionRecord(const CConnectionRecord &c)
{
	m_aConnectionRecords.SafeAddTail(new CConnectionRecord(c));
	// Post and OnIdle will do the same, react on incoming ConnectionRecord
	CViewCamera *pView = theApp.GetMainFrame()->GetViewCamera();
	if (pView)
	{
		pView->PostMessage(WM_USER,WPARAM_CONNECTION_RECORD);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnConnectionRecord()
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
				bFetching = m_Servers.GetAtFast(i)->IsFetching();
			}
			
			if (!bFetching)
			{
				// gerade kein Fetch
				if (theApp.m_pPermission == NULL)	// no permission: no user
				{
					if (InitAlarmUser())
					{
						const CString &sPermission = pCR->GetPermission();

						CWK_Profile wkp;
						CPermissionArray pms;
						CPermission* pControlPermission = NULL;
						pms.Load(wkp);
						if (!sPermission.IsEmpty())
						{
							pControlPermission = pms.GetPermission(sPermission);
						}
						if (!pControlPermission)
						{
							pControlPermission = pms.GetPermission(theApp.m_pUser->GetPermissionID());
						}
						if (pControlPermission)
						{
							theApp.m_pPermission = new CPermission(*pControlPermission);
							if (theApp.m_pDlgLogin)
							{
								theApp.m_pDlgLogin->DestroyWindow();
								theApp.m_pDlgLogin = NULL;
							}
						}
					}
				}
				if (theApp.m_pPermission)
				{
					ControlConnection(*pCR);
				}
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
void CIdipClientDoc::AlarmConnection(const CConnectionRecord& c)
{
	if (theApp.m_pPermission == NULL)
	{
//			theApp.m_pDlgLogin->SendMessage(WM_USER_LOGIN_ALARM, NULL, NULL);

		CPermissionArray pms;
		CPermission* pAlarmPermission = NULL;
		CWK_Profile wkp;
		pms.Load(wkp);
		
		pAlarmPermission = pms.GetPermission(c.GetPermission());

		if (pAlarmPermission == NULL)
		{
			//check for alarmcall from DTS (SpecialReceiver)
			CPermission* permSpecialReceiver = pms.CheckPermission(c);
			if (   permSpecialReceiver
				&& permSpecialReceiver->IsSpecialReceiver())
			{
				pAlarmPermission = permSpecialReceiver;
			}
		}

		if (pAlarmPermission)
		{
			theApp.m_pPermission = new CPermission(*pAlarmPermission);
			InitAlarmUser();
		}
	}

	if (   theApp.m_pPermission
		&& theApp.m_pUser)
	{
		if (theApp.m_pDlgLogin)
		{
			theApp.m_pDlgLogin->DestroyWindow();
			theApp.m_pDlgLogin = NULL;
		}

		// gf: If it is a second call (called before the first confirmation arrived at host)
		// we may got the non-alarm server and add the alarm server again a second time...
		// .. so get the real alarm server - or not
	//	CServer* pServer = m_Servers.GetServer(c.GetSourceHost());
		CServer* pServer = m_Servers.GetAlarmServer(c.GetSourceHost());
		if (pServer)
		{
			// Ignore it...
			// TODO GF: But what about camID and other parameters, may differ from first call...?
	/*
			if (   m_pServer->IsControl()
				&& (m_pServer->GetAlarmID().GetSubID() == c.GetCamID().GetSubID())
				)
			{
				pServer->SetAlarmID(c.GetCamID());
			}
	*/
			DWORD dwOptions = SCO_WANT_RELAYS | SCO_WANT_CAMERAS_COLOR | SCO_NO_STREAM | SCO_JPEG_AS_H263 | SCO_MULTI_CAMERA;
			if (theApp.GetIntValue(_T("CanGOP"), 1))
			{
				dwOptions |= SCO_CAN_GOP;
			}
			GetCIPCServerControlIdipClient()->DoConfirmAlarmConnection(c.GetInputShm(), 
																		c.GetOutputShm(),
																		_T(""),
																		_T(""),
																		dwOptions
	#ifdef _UNICODE
																, MAKELONG(CODEPAGE_UNICODE, 0)
	#endif
																);
			Sleep(50);
		}
		else
		{
			pServer = m_Servers.AddServer();
			pServer->AlarmConnection(c,m_pCIPCServerControlIdipClient);
			theApp.AlarmConnection();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::ControlConnection(const CConnectionRecord& c)
{
	// immer den bestehenden Server verwenden
	CServer* pServer = m_Servers.GetServer(c.GetSourceHost());
	if (pServer)
	{
		if (pServer->IsConnectedLive())
		{
			// already connected!
			if (!pServer->IsControl())
			{
				if (pServer->ControlConnection(c))
				{
					GetCIPCServerControlIdipClient()->DoConfirmAlarmConnection(c.GetInputShm(), 
																		   c.GetOutputShm(),
                                                                           _T(""),
																		   _T(""),
                                                                           0
#ifdef _UNICODE
																		   , MAKELONG(CODEPAGE_UNICODE, 0)
#endif
																		   );
					CViewCamera* pOV = theApp.GetMainFrame()->GetViewCamera();

					if (pOV)
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
					m_pCIPCServerControlIdipClient->DoIndicateError(0, SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT, 0);
				}
			}
			else
			{
				WK_TRACE(_T("control connection of controlled host failed %s\n"),c.GetSourceHost());
				m_pCIPCServerControlIdipClient->DoIndicateError(0, SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT, 1);
			}
		}
		else
		{
			WK_TRACE(_T("control connection of not yet connected host failed %s\n"),c.GetSourceHost());
			m_pCIPCServerControlIdipClient->DoIndicateError(0, SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT, 2);
		}
	}
	else
	{
		CString sValue;
		if (c.GetFieldValue(CRF_EXCLUSIVE,sValue))
		{
			if (sValue == CSD_ON)
			{
				DisconnectAll(FALSE);
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
			m_pCIPCServerControlIdipClient->DoIndicateError(0, SECID_NO_ID,CIPC_ERROR_UNABLE_TO_CONNECT, 3);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	// return TRUE to close the window
	// return FALSE if not

	BOOL bAsk = TRUE;
	if (theApp.m_dwClosing & LAUNCHER_IS_CLOSING)
	{
		bAsk = FALSE;
	}
	
	if (!DisconnectAll(FALSE, bAsk))
	{
		return FALSE;
	}

	WK_DELETE(m_pCIPCInputIdipClientCommData);
	WK_DELETE(m_pCIPCDatabaseLocal);
	WK_DELETE(m_pCIPCServerControlIdipClient);
	WK_DELETE(m_pCIPCLocalAudio);
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnSequenze() 
{
	m_bSequence = !m_bSequence;
	if (!m_bSequence)
	{
		// Bei DV-Server über ISDN muss die Bildanforderung
		// für die aktuell aktive Kamera erneut angestossen werden
		CServer* pServer;
		CIPCOutputIdipClient* pOutput;
		//changes for VisualStudio 2005
		int i = 0;
		for (i=0 ; i<m_Servers.GetSize() ; i++)
		{
			pServer = m_Servers.GetAtFast(i);
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
void CIdipClientDoc::OnUpdateSequenzePane(CCmdUI* pCmdUI) 
{
	CString sL;
	CString sF;
	// TODO! RKE: new Sequencer modus to be implemented
	if (m_bSequence)
	{
		if (pCmdUI->m_nID == IDC_BTN_SEQUENCE)
		{
			sF.Format(_T("%ds"), m_dwSequenceIntervall);
		}
		else
		{
			sL.LoadString(ID_SEQUENCE_PANE);
			sF.Format(sL,m_dwSequenceIntervall);
		}
	}
	else
	{
		sF.LoadString(IDS_SEQUENCE_OFF);
	}
	pCmdUI->SetText(sF);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::PictureStored()
{
	m_dwLastStored = GetTickCount();
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnUpdateDatabasePane(CCmdUI* pCmdUI) 
{
	CString sL;
	if (pCmdUI->m_pOther)
	{
		if (IsLocalDatabaseConnected())
		{
			int nIndex = 0;
			if (m_dwLastStored != 0 && GetTimeSpan(m_dwLastStored) < 200)
			{
				sL.LoadString(IDS_STORING);
				nIndex = 1+(m_dwLastStored&1);
			}
			else
			{
				sL.LoadString(IDS_POSSIBLE);
			}
			if (pCmdUI->m_nID == IDC_BTN_SAVE_STATE)
			{
				CWnd *pWnd = CWnd::FromHandle(pCmdUI->m_pOther->m_hWnd);
				ASSERT_KINDOF(CSkinButton, pWnd);
				((CSkinButton*)pWnd)->SetImageIndex(nIndex);
				if (!pWnd->IsWindowVisible())
					pWnd->ShowWindow(SW_SHOW);
			}
		}
		else
		{
			sL.LoadString(ID_DATABASE_PANE);
			if (pCmdUI->m_nID == IDC_BTN_SAVE_STATE)
			{
				if (pCmdUI->m_pOther->IsWindowVisible())
					pCmdUI->m_pOther->ShowWindow(SW_HIDE);
			}
		}
	}
	pCmdUI->SetText(sL);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::SetSequence(BOOL bOn)
{
	m_bSequence = bOn;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::SetSequenceTime(DWORD dwSeconds)
{
	m_dwSequenceIntervall = dwSeconds;
	theApp.m_dwSequenceIntervall = m_dwSequenceIntervall;
	m_bSequence  = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnSequenceX(UINT nID) 
{
	switch (nID)
	{
		case ID_SEQUENCE_1: nID = 1; break;
		case ID_SEQUENCE_2: nID = 2; break;
		case ID_SEQUENCE_3: nID = 3; break;
		case ID_SEQUENCE_4: nID = 4; break;
		case ID_SEQUENCE_5: nID = 5; break;
		case ID_SEQUENCE_10: nID = 10; break;
		case ID_SEQUENCE_15: nID = 15; break;
		case ID_SEQUENCE_20: nID = 20; break;
		case ID_SEQUENCE_30: nID = 30; break;
		default: nID = 2;
	}
	SetSequenceTime(nID);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnUpdateSequenceX(CCmdUI* pCmdUI) 
{
	UINT nID;
	switch (pCmdUI->m_nID)
	{
		case ID_SEQUENCE_1: nID = 1; break;
		case ID_SEQUENCE_2: nID = 2; break;
		case ID_SEQUENCE_3: nID = 3; break;
		case ID_SEQUENCE_4: nID = 4; break;
		case ID_SEQUENCE_5: nID = 5; break;
		case ID_SEQUENCE_10: nID = 10; break;
		case ID_SEQUENCE_15: nID = 15; break;
		case ID_SEQUENCE_20: nID = 20; break;
		case ID_SEQUENCE_30: nID = 30; break;
		default: nID = 2;
	}
	pCmdUI->Enable(   m_Servers.GetSize()
//				   && (!m_Servers.IsAnyB3Connection())
				   && m_bSequence);

	pCmdUI->SetCheck(m_bSequence && (m_dwSequenceIntervall == nID));
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientDoc::GetStore()
{
	return m_bStore;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientDoc::GetSequence()
{
	return m_bSequence;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CIdipClientDoc::GetSequenceIntervall()
{
	return m_dwSequenceIntervall;
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnVideoSaveAll() 
{
	OnVideoSaveAllToggle();

/*
// TKR wird nun nur noch über einen einzigen Menüeintrag getoggled
	m_bStore = TRUE;
	CViewIdipClient* pView = GetViewIdipClient();
	if (pView)
	{
		pView->SetStore(m_bStore);
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnVideoSaveAllStop() 
{
	OnVideoSaveAllToggle();
/*
// TKR wird nun nur noch über einen einzigen Menüeintrag getoggled
	m_bStore = FALSE;
	CViewIdipClient* pView = GetViewIdipClient();
	if (pView)
	{
		pView->SetStore(m_bStore);
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnVideoSaveAllToggle() 
{
	m_bStore = !m_bStore;
	CViewIdipClient* pView = theApp.GetMainFrame()->GetViewIdipClient();
	if (pView)
	{
		pView->SetStore(m_bStore);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnUpdateVideoSaveAll(CCmdUI* pCmdUI) 
{
	BOOL bEnable = (   theApp.IsValidUserLoggedIn()
					&& IsLocalDatabaseConnected()
					&& (theApp.IsReadOnlyModus() == FALSE)
					);

	pCmdUI->SetCheck(m_bStore);
	pCmdUI->Enable(bEnable);
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::ResetConnectionAutoLogout() 
{
	m_Servers.ResetConnectionAutoLogout();
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnLocalDatabaseConnected()
{
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnLocalDatabaseDisconnected()
{
	if (   m_pCIPCDatabaseLocal
		&& m_pCIPCDatabaseLocal->GetIsMarkedForDelete())
	{
		WK_DELETE(m_pCIPCDatabaseLocal);
	}
}
////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnLocalDataCarrierDisconnected()
{
	if (   m_pDataCarrier
		&& m_pDataCarrier->GetIsMarkedForDelete())
	{
		WK_DELETE(m_pDataCarrier);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnFileOpen()
{
	CViewIdipClient* pView = theApp.GetMainFrame()->GetViewIdipClient();
	if (pView)
	{
		pView->OpenFile();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnUpdateFileOpen(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.IsValidUserLoggedIn());
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnUpdateVideoSaveAllStop(CCmdUI *pCmdUI)
{
	// TKR wird nun nur noch über einen einzigen Menüeintrag getoggled
	//deshalb wird Button "SaveAllStop" nicht mehr benutzt
/*
	BOOL bEnable = (   theApp.IsValidUserLoggedIn()
					&& IsLocalDatabaseConnected()
					&& (theApp.IsReadOnlyModus() == FALSE)
					);


	pCmdUI->Enable(bEnable);
*/
	pCmdUI->Enable(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::OnFilePrint()
{
	CViewIdipClient* pView = theApp.GetMainFrame()->GetViewIdipClient();
	if (pView)
	{
		pView->OnFilePrint();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CIdipClientDoc::UpdateAllViewsAsync(LPARAM lHint, CObject* pHint)
{
	ASSERT(pHint == NULL || _CrtIsValidHeapPointer(pHint));				// Has to be allocated with new
	ASSERT(pHint == NULL || pHint->IsKindOf(RUNTIME_CLASS(CObject)));	// and derived from CObject!
	theApp.GetMainFrame()->PostMessage(WM_UPDATE_ALL_VIEWS, (WPARAM)pHint, lHint);
}
/////////////////////////////////////////////////////////////////////////////
DWORD CIdipClientDoc::UpdateMapElement(CSecID ServerID, CString &s, DWORD& dwUpdate)
{
	DWORD dwResult = SECID_NO_ID;
	if (dwUpdate & (SEARCH_IN_CAMERAS|SEARCH_IN_HOSTS))
	{
		CViewCamera *pView = theApp.GetMainFrame()->GetViewCamera();
		if (pView)
		{
			dwResult = pView->UpdateMapElement(ServerID, s, dwUpdate);
		}
	}

	if (dwUpdate & SEARCH_IN_RELAYS && dwResult == SECID_NO_ID)
	{
		CViewRelais*pView = theApp.GetMainFrame()->GetViewRelais();
		if (pView)
		{
			dwResult = pView->UpdateMapElement(ServerID, s, dwUpdate);
		}
	}

	if (dwUpdate & SEARCH_IN_INPUT && dwResult == SECID_NO_ID)
	{
		CViewAlarmList*pView = theApp.GetMainFrame()->GetViewAlarmList();
		if (pView)
		{
			dwResult = pView->UpdateMapElement(ServerID, s, dwUpdate);
		}
	}

	if (dwUpdate & SEARCH_IN_MEDIA && dwResult == SECID_NO_ID)
	{
		CViewAudio*pView = theApp.GetMainFrame()->GetViewAudio();
		if (pView)
		{
			dwResult = pView->UpdateMapElement(ServerID, s, dwUpdate);
		}
	}
	return dwResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CIdipClientDoc::InitAlarmUser()
{
	CWK_Profile wkp;
	CUserArray users;
	users.Load(wkp);
	
	BOOL bFound=FALSE;
	//changes for VisualStudio 2005
	int i = 0;
	for (i=0 ; bFound==FALSE && i<users.GetSize() ; i++) 
	{
		CUser* pUser = users[i];
		if (pUser->IsUserAlarm())
		{
			WK_DELETE(theApp.m_pUser);
			theApp.m_pUser = new CUser(*pUser);
			return TRUE;
		}
	}// end of loop over users
	return FALSE;
}
