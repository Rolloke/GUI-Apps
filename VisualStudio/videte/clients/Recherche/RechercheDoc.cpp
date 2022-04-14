// RechercheDoc.cpp : implementation of the CRechercheDoc class
//

#include "stdafx.h"
#include "Recherche.h"

#include "RechercheDoc.h"
#include "RechercheView.h"
#include "ObjectView.h"
#include "Mainfrm.h"
#include "SyncCoolBar.h"
#include "IPCControlAudioUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRechercheDoc

IMPLEMENT_DYNCREATE(CRechercheDoc, CDocument)

BEGIN_MESSAGE_MAP(CRechercheDoc, CDocument)
	//{{AFX_MSG_MAP(CRechercheDoc)
	ON_COMMAND(ID_CONNECT, OnConnect)
	ON_COMMAND(ID_CONNECT_LOCAL, OnConnectLocal)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_LOCAL, OnUpdateConnectLocal)
	ON_UPDATE_COMMAND_UI(ID_CONNECT, OnUpdateConnect)
	ON_UPDATE_COMMAND_UI(ID_DISCONNECT, OnUpdateDisconnect)
	ON_COMMAND(ID_CONNECT_MAP, OnConnectMap)
	ON_UPDATE_COMMAND_UI(ID_CONNECT_MAP, OnUpdateConnectMap)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRechercheDoc construction/destruction

CRechercheDoc::CRechercheDoc()
{
	m_dwLastLocalTry = GetTickCount() - 5 * 1000;
	m_bLocalConnection = TRUE;
	m_Servers.SetDocument(this);
	m_pCIPCLocalAudio = NULL;
	
	m_dwLastLocalTryAudio = 0;
}

CRechercheDoc::~CRechercheDoc()
{
	WK_DELETE(m_pCIPCLocalAudio);
}

BOOL CRechercheDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
//#ifndef _DEBUG
	if (GetLocalServer()==NULL)
	{
		OnConnectLocal();
	}
//#endif //!_DEBUG

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CRechercheDoc serialization

void CRechercheDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CRechercheDoc diagnostics

#ifdef _DEBUG
void CRechercheDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CRechercheDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CRechercheDoc commands
CObjectView* CRechercheDoc::GetObjectView()
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
CRechercheView* CRechercheDoc::GetRechercheView()
{
	CView* pView;
	POSITION pos = GetFirstViewPosition();
	while (pos) 
	{
		pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CRechercheView))) 
		{
			return (CRechercheView*)pView;
			break;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::UpdateMyViews(CView* pSender, LPARAM lHint, CObject* pHint)
	// walk through all views
{
	CObjectView* pOV = GetObjectView();
	ASSERT_VALID(pOV);
	if (pOV != pSender)
		pOV->OnUpdate(pSender, lHint, pHint);
	CRechercheView* pRV = GetRechercheView();
	ASSERT_VALID(pRV);
	if (pRV != pSender)
		pRV->OnUpdate(pSender, lHint, pHint);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::OnIdle()
{
	m_Servers.OnIdle();

	AutoLogout();

	Sleep(0);

//#ifndef _DEBUG
	if (GetLocalServer()==NULL)
	{
		OnConnectLocal();
	}
//#endif //!_DEBUG

	/*
		CObjectView* pOV = GetObjectView();
		if (WK_IS_WINDOW(pOV))
		{
			pOV->Redraw();
		}
	*/

	CRechercheView* pRV = GetRechercheView();
	if (WK_IS_WINDOW(pRV))
	{
		pRV->OnIdle();
	}

	if (m_pCIPCLocalAudio!=NULL && (m_pCIPCLocalAudio->GetIPCState() == CIPC_STATE_DISCONNECTED))
	{
		WK_DELETE(m_pCIPCLocalAudio);
	}
	else
	{
		ConnectToAudio();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::OnConnectLocal() 
{
	if (m_Servers.GetLocalServer())
	{
		// already connected
		return;
	}
	if (WK_IS_RUNNING(WK_APP_NAME_DB_SERVER)==FALSE) 
	{
		// no chance without dbs.exe running
		return; 
	}

	DWORD dwTick = GetTickCount();

	if ((dwTick - m_dwLastLocalTry) < 7500)
	{
		// try again in 7,5 seconds
		AfxGetMainWnd()->PostMessage(WM_USER);
		return;
	}

	m_dwLastLocalTry = dwTick;

	CServer* pServer;
    CHostArray& ha = theApp.GetHosts();

	CSecID idHost = ha.GetLocalHost()->GetID();

	pServer = m_Servers.GetServer(idHost);
	if (pServer==NULL)
	{
		pServer = m_Servers.AddServer();
		CString sLocal;
		sLocal.LoadString(IDS_LOCAL_SERVER);
		sLocal += _T(" ") + ha.GetLocalHost()->GetName();
		pServer->Connect(sLocal,LOCAL_LOOP_BACK,idHost);
		m_dwLastLocalTry = GetTickCount();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::OnUpdateConnectLocal(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Servers.GetLocalServer()==NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::Connect(CSecID idHostID)
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
			if (theApp.m_pUser)
			{
				WK_TRACE_USER(_T("%s baut Verbindung zu %s auf\n"),
					theApp.m_pUser->GetName(),pHost->GetName());
			}
			pServer = m_Servers.AddServer();
			pServer->Connect(pHost->GetName(),
							 pHost->GetNumber(),
							 pHost->GetID());
		}
	}
	else
	{
		WK_TRACE_ERROR(_T("connect unknown host\n"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::OnConnect() 
{
	COEMConnectionDialog dlg(theApp.GetHosts(),AfxGetMainWnd());

	dlg.m_sFilterTypes = _T("MINI B3,MINI B6,SMS,FAX,E-Mail,TeleObserver");

	if (IDOK==dlg.DoModal())
	{
		CServer* pServer = m_Servers.GetServer(dlg.m_idHost);
		if (pServer==NULL)
		{
			pServer = m_Servers.AddServer();
			pServer->Connect(dlg.m_sHost,dlg.m_sTelefon,dlg.m_idHost);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::OnConnectMap() 
{
	CRechercheView* pRV = GetRechercheView();
	if (WK_IS_WINDOW(pRV))
	{
		pRV->ShowMapWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::OnUpdateConnectMap(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!theApp.GetMapURL().IsEmpty());
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::OnUpdateConnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(theApp.m_bRunAnyLinkUnit 
				  && (GetRechercheView()->GetBackupDialog()==NULL));
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::Disconnect(WORD wServerID)
{
	if (wServerID!=0)
	{
		CMainFrame*	pMF = (CMainFrame*)theApp.GetMainWnd();
		if (WK_IS_WINDOW(pMF))
		{
			CSyncCoolBar* pSCB = pMF->GetSyncCoolBar();
			if (WK_IS_WINDOW(pSCB))
			{
				pSCB->DeleteArchive(wServerID,0,FALSE);
			}
		}
		UpdateMyViews(NULL,VDH_REMOVE_SERVER,(CObject*)wServerID);
		m_Servers.Disconnect(wServerID);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::DisconnectAll()
{
	CServer* pServer;
	WORD wServerID;

	for (int i=0;i<m_Servers.GetSize();i++)
	{
		pServer = m_Servers.GetAt(i);
		wServerID = pServer->GetID();
		m_Servers.Disconnect(wServerID);
		UpdateMyViews(NULL,VDH_REMOVE_SERVER,(CObject*)wServerID);
	}

	WK_DELETE(m_pCIPCLocalAudio);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::DisconnectRemote()
{
	CServer* pServer;
	WORD wServerID;

	for (int i=0;i<m_Servers.GetSize();i++)
	{
		pServer = m_Servers.GetAt(i);
		if (!pServer->IsLocal())
		{
			wServerID = pServer->GetID();
			m_Servers.Disconnect(wServerID);
			UpdateMyViews(NULL,VDH_REMOVE_SERVER,(CObject*)wServerID);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::DeleteArchivFiles()
{
	int i,c;
	CServer* pServer;

	c = m_Servers.GetSize();
	for (i=0;i<c;i++)
	{
		pServer = m_Servers.GetAt(i);
		if (pServer && pServer->IsConnected())
		{
			pServer->DeleteArchivFiles();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::StartSearch()
{
	int i,c;
	CServer* pServer;

	c = m_Servers.GetSize();
	for (i=0;i<c;i++)
	{
		pServer = m_Servers.GetAt(i);
		if (pServer && pServer->IsConnected())
		{
			pServer->StartSearch();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::StopSearch()
{
	int i,c;
	CServer* pServer;

	c = m_Servers.GetSize();
	for (i=0;i<c;i++)
	{
		pServer = m_Servers.GetAt(i);
		if (pServer && pServer->IsConnected())
		{
			pServer->StopSearch();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::StopSearchCopy()
{
	int i,c;
	CServer* pServer;

	c = m_Servers.GetSize();
	for (i=0;i<c;i++)
	{
		pServer = m_Servers.GetAt(i);
		if (pServer && pServer->IsConnected())
		{
			pServer->StopSearchCopy();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::Remove(WORD wServerID)
{
	UpdateMyViews(NULL,VDH_REMOVE_SERVER,(CObject*)wServerID);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::OnUpdateDisconnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_Servers.GetSize()
					&& (GetRechercheView()->GetBackupDialog()==NULL));
}
/////////////////////////////////////////////////////////////////////////////
CServer* CRechercheDoc::GetServer(WORD wServerID)
{
	return m_Servers.GetServer(wServerID);
}
/////////////////////////////////////////////////////////////////////////////
CServer* CRechercheDoc::GetLocalServer()
{
	int i,c;
	CServer* pServer;

	c = m_Servers.GetSize();
	for (i=0;i<c;i++)
	{
		pServer = m_Servers.GetAt(i);
		if (pServer && pServer->IsLocal() && pServer->IsConnected())
		{
			return pServer;
		}
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::AutoLogout()
{
	// Attention called from OnIdle
	if (theApp.m_AutoLogout.IsApplicationOver())
	{
		if (theApp.IsDemo())
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
BOOL CRechercheDoc::CanCloseFrame(CFrameWnd* pFrame) 
{
	// TODO: Add your specialized code here and/or call the base class
	// return TRUE to close the window
	// return FALSE if not

	m_Servers.DisconnectAll();
//	WK_DELETE(m_pCIPCLocalAudio);
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::ActualizeDatabase(CIPCDatabaseRecherche* pDatabase)
{
	UpdateMyViews(NULL,VDH_DATABASE,(CObject*)pDatabase);
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::NoLocalConnection()
{
	m_bLocalConnection = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CRechercheDoc::ResetConnectionAutoLogout()
{
	m_Servers.ResetConnectionAutoLogout();
}

/////////////////////////////////////////////////////////////////////////////
// Local Connection to Audio Unit !!
void CRechercheDoc::ConnectToAudio()
{
	if (m_pCIPCLocalAudio)
	{
		return;
	}
	if (IsAudioConnected())
	{
		return;	// already connected
	}
	if (NULL == theApp.m_pUser)
	{
		return;	// no chance without user
	}

	if (WK_IS_RUNNING(WK_APP_NAME_AUDIOUNIT1)==FALSE) 
	{
		return; // no chance without the AudioUnit
	}

#ifdef _DTS
	CString str;
	str.Format(SM_AudioUnitMedia, 1);
	m_pCIPCLocalAudio = new CIPCAudioRecherche(str, this, TRUE);
	Sleep(10);
	return ; // nothing to do anymore, because no server on backup medium
#else

	if (theApp.m_bReadOnlyModus)
	{
		CString str;
		str.Format(SM_AudioUnitMedia, 1);
		m_pCIPCLocalAudio = new CIPCAudioRecherche(str, this, TRUE);
		Sleep(10);
		return ; // nothing to do anymore, because no server on backup medium
	}
	else
	{
		CWK_Dongle dongle;
		if(!dongle.RunAudioUnit())
		{
			return;	// not allowed
		}
	}

	DWORD dwTick = GetTickCount();
	if (dwTick - m_dwLastLocalTryAudio < 10 * 1000)
	{
		AfxGetMainWnd()->PostMessage(WM_USER); // Force OnIdle() !!
		return;
	}
	m_dwLastLocalTryAudio = dwTick;

	CString sPass = theApp.m_pPermission->GetPassword();
	CConnectionRecord c(LOCAL_LOOP_BACK,
		*theApp.m_pUser,
		theApp.m_pPermission->GetName(),
		theApp.m_pPermission->GetPassword()
		);
	c.SetTimeout(2000);

	CString sSmName;
	CIPCFetch fetch;
	CIPCFetchResult fr = fetch.FetchAudio(c);
	sSmName = fr.GetShmName();
	if (!sSmName.IsEmpty())
	{
		m_pCIPCLocalAudio = new CIPCAudioRecherche(sSmName, this);
	}
	Sleep(10);
#endif
}
//////////////////////////////////////////////////////////////////////////
BOOL CRechercheDoc::IsAudioConnected()
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
CIPCAudioRecherche* CRechercheDoc::GetLocalAudio()
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