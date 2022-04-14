// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "Vision.h"

#include "MainFrm.h"

#include "VisionDoc.h"
#include "VisionView.h"
#include "ObjectView.h"
#include "CIPCOutputVisionDecoder.h"
#include "CIPCControlAudioUnit.h"

#include "ExternalFrameDlg.h"

#include <regstr.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_WM_MOVE()
	ON_COMMAND(ID_VIEW_FULLSCREEN, OnViewFullscreen)
	ON_UPDATE_COMMAND_UI(ID_VIEW_FULLSCREEN, OnUpdateViewFullscreen)
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATEAPP()
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_WM_ENTERMENULOOP()
	ON_WM_EXITMENULOOP()
   ON_COMMAND_RANGE(ID_CONNECT_LOW, ID_CONNECT_HIGH, OnConnect)
   ON_COMMAND_RANGE(ID_DISCONNECT_LOW, ID_DISCONNECT_HIGH, OnDisconnect)
   ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropDown)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
	ON_MESSAGE(WM_USER, OnUser)
	ON_UPDATE_COMMAND_UI_RANGE(ID_VIEW_MONITOR2, ID_VIEW_MONITOR1+MAX_MONITORS, OnUpdateViewMonitor)
   ON_COMMAND_RANGE(ID_VIEW_MONITOR2, ID_VIEW_MONITOR1+MAX_MONITORS, OnViewMonitor)
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_DATABASE_TEXT,//1
	ID_DATABASE_PANE,
	ID_SEQUENCE_TEXT,//3
	ID_SEQUENCE_PANE,
};
/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
/////////////////////////////////////////////////////////////////////////////
CMainFrame::CMainFrame()
{
	m_pVisionView = NULL;
	m_pObjectView = NULL;
	m_bActive = TRUE;
	m_bFullScreen = FALSE;
	m_hMenu = NULL;
	memset(&m_wp,0,sizeof(m_wp));
	m_wp.length = sizeof(m_wp);
	m_iObjectWidth = 0;
}
/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
	while(m_ExternalFrames.GetCount())
	{
		CExternalFrameDlg *pExt = (CExternalFrameDlg*) m_ExternalFrames.RemoveHead();
		WK_DELETE(pExt);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CMainFrame::KillScreenSaverFunc(HWND hwnd, LPARAM lParam)
{
   if(::IsWindowVisible(hwnd))
	{
		WK_TRACE(_T("ForegroundWindow found on Other Desktop\n"));
		::PostMessage(hwnd, WM_CLOSE, 0, 0);
		*((BOOL*)lParam) = FALSE;
	}
   return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (COemGuiApi::GetComCtl32Version()==COMCTL32_471)
	{
		if (!m_wndCoolBar.Create(this,
			WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
				RBS_TOOLTIPS|RBS_BANDBORDERS|RBS_VARHEIGHT|CBRS_FLYBY)) {
			TRACE(_T("Failed to create cool bar\n"));
			return -1;      // fail to create
		}
   }
	else
	{
		if (!m_wndToolBar.Create(this) ||
			!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
			TRACE(_T("Failed to create tool bar\n"));
			return -1;      // fail to create
		}
		m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
			CBRS_ALIGN_TOP | CBRS_TOOLTIPS);
		m_wndToolBar.SetHeight(40);
		TBBUTTON tb;
		CString s;
		int i,c;
		c = m_wndToolBar.GetToolBarCtrl().GetButtonCount();
		for (i=0;i<c;i++)
		{
			if (m_wndToolBar.GetToolBarCtrl().GetButton(i,&tb))
			{
				if (tb.idCommand != ID_SEPARATOR)
				{
					s.LoadString(tb.idCommand);
					s = s.Mid(s.Find('\n')+1);
					m_wndToolBar.SetButtonText(i,s);
				}
			}
		}
		m_wndToolBar.GetToolBarCtrl().SetButtonSize(CSize(72,42));
	}
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE(_T("Failed to create status bar\n"));
		return -1;      // fail to create
	}

	m_wndStatusBar.SetPaneStyle(1,SBPS_NOBORDERS);
	m_wndStatusBar.SetPaneStyle(3,SBPS_NOBORDERS);

// TODO gf CoolMenuManager funzt nicht mit MFC 7.1
#if _MFC_VER < 0x0070
	m_CoolMenuManager.m_bAutoAccel = FALSE;
	m_CoolMenuManager.Install(this);
	m_CoolMenuManager.LoadToolbar(IDR_TOOLBARMENU);
#endif


#ifdef _DTS
	CMenu* pMenu = GetMenu();
	if (pMenu)
	{
		// Melder
		pMenu->DeleteMenu(1,MF_BYPOSITION);
		// Relais
		pMenu->DeleteMenu(2,MF_BYPOSITION);
		// Akustische Alarmmeldung
		CMenu* pPopupConnection = pMenu->GetSubMenu(0);
		pPopupConnection->DeleteMenu(ID_CONNECTION_ANIMATION,MF_BYCOMMAND);
		pPopupConnection->DeleteMenu(ID_EDIT_CREATE_HTML_HOST_PAGE, MF_BYCOMMAND);
		pPopupConnection->DeleteMenu(ID_CONNECT_MAP,MF_BYCOMMAND);
		CMenu* pPopupCamera = pMenu->GetSubMenu(1);
		pPopupCamera->DeleteMenu(ID_VIDEO_SAVE_REFERENCE,MF_BYCOMMAND);
		pPopupCamera->DeleteMenu(ID_VIDEO_SHOW_REFERENCE,MF_BYCOMMAND);
		pPopupCamera->DeleteMenu(ID_HOST_CAMERA_MAP,MF_BYCOMMAND);
		pPopupCamera->DeleteMenu(ID_EDIT_CREATE_HTML_CAMERA_PAGE, MF_BYCOMMAND);
		pPopupCamera->DeleteMenu(ID_VIDEO_FEIN,MF_BYCOMMAND);
		pPopupCamera->DeleteMenu(ID_VIDEO_GROB,MF_BYCOMMAND);
		pPopupCamera->DeleteMenu(ID_VIDEO_ALARM_SWITCH,MF_BYCOMMAND);
		pPopupCamera->DeleteMenu(ID_VIDEO_CSB,MF_BYCOMMAND);
		pPopupCamera->DeleteMenu(5,MF_BYPOSITION);
		pPopupCamera->DeleteMenu(5,MF_BYPOSITION);

		CMenu* pPopupSwitch = NULL; 
		
		//
		for (DWORD i=0;i<pPopupCamera->GetMenuItemCount();i++)
		{ 
			pPopupSwitch = pPopupCamera->GetSubMenu(i);
			if (pPopupSwitch)
			{
				break;
			}
		}

		if (pPopupSwitch)
		{
			pPopupSwitch->DeleteMenu(ID_HOST_LOAD_SEQUENCE,MF_BYCOMMAND);
			pPopupSwitch->DeleteMenu(ID_HOST_SAVE_SEQUENCE,MF_BYCOMMAND);
			pPopupSwitch->DeleteMenu(ID_SEQUENCE_1,MF_BYCOMMAND);
			pPopupSwitch->DeleteMenu(ID_SEQUENCE_2,MF_BYCOMMAND);
			pPopupSwitch->DeleteMenu(ID_SEQUENCE_3,MF_BYCOMMAND);
		}

		CMenu* pPopupView = pMenu->GetSubMenu(2);
		if (pPopupView)
		{
			UINT nCount = pPopupView->GetMenuItemCount();
			for (i = nCount-1;i>=0;i--)
			{
				UINT nID = pPopupView->GetMenuItemID(i);
				if (nID == ID_SEPARATOR)
				{
					pPopupView->DeleteMenu(i,MF_BYPOSITION);
					break;
				}
			}

			pPopupView->DeleteMenu(ID_VIEW_MONITOR2, MF_BYCOMMAND);
		}
	}
#else
	if (!theApp.m_pDongle->RunAudioUnit())
	{
		CMenu* pMenu = GetMenu();
		if (pMenu)
		{
			pMenu->DeleteMenu(4, MF_BYPOSITION);
		}
	}
#endif
	
	BOOL bActive = FALSE;
	// WINVER >= 0x0500
	// #define SPI_GETSCREENSAVERRUNNING 114 
 	SystemParametersInfo(114, FALSE, &bActive, SPIF_SENDWININICHANGE);
	WK_TRACE(_T("ScreenSaverRunning = %d\n"), bActive);
	
	if (bActive)
	{
		HKEY hKey = NULL;
		BOOL bSecure = TRUE;
		if (theApp.IsWinXP())
		{
			LRESULT lResult = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Control Panel\\Desktop"), 0, KEY_READ, &hKey);
			if (lResult == ERROR_SUCCESS)
			{
				_TCHAR szReg[32];
				DWORD dwType, dwCount = sizeof(szReg);
				lResult = RegQueryValueEx(hKey, _T("ScreenSaverIsSecure"), NULL, &dwType, (LPBYTE)szReg, &dwCount);
				if (lResult == ERROR_SUCCESS && dwType == REG_SZ && dwCount != 0)
				{
					bSecure = _ttoi(szReg);
				}
			}
			if (hKey)
			{
				RegCloseKey(hKey);
			}
		}
		if (bSecure)
		{
			WK_TRACE(_T("ScreenSaver is Secure\n"));
			// TODO! RKE: Letztes Alarmbild speichern und Daueralarmton starten, bis
			// der User sich einloggt.
			Beep(1000, 1000);
		}
		else
		{
			WK_TRACE(_T("Find WindowsScreenSaverClass\n"));
			HWND hwnd = ::FindWindow(_T("WindowsScreenSaverClass"), NULL);
			if (hwnd)
			{
				::PostMessage(hwnd, WM_CLOSE, 0, 0);
				WK_TRACE(_T("WindowsScreenSaverClass found\n"));
				bActive = FALSE;
			}
			else
			{

/*
				hwnd = ::GetForegroundWindow();
				if (hwnd)
				{
					::PostMessage(hwnd, WM_CLOSE, 0, 0);
					WK_TRACE(_T("ForegroundWindow found\n"));
					bActive = FALSE;
				}
*/
				WK_TRACE(_T("trying Screen-saver Desktop\n"));
				HDESK hdesk = OpenDesktop(_T("Screen-saver"),
										  0,
										  FALSE,
										  DESKTOP_READOBJECTS | DESKTOP_WRITEOBJECTS);
				if (hdesk)
				{
					EnumDesktopWindows(hdesk, CMainFrame::KillScreenSaverFunc, (LPARAM)&bActive);
					CloseDesktop(hdesk);
					if (!bActive)
					{
						WK_TRACE(_T("WindowsScreenSaver found\n"));
					}
				} 
			}
		}
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCreateClient( LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	if (!m_wndSplitter.CreateStatic( this, 1, 2))
	{
        TRACE(_T("Failed to CreateStaticSplitter\n"));
        return FALSE;
	}

    // add the first splitter pane - the default view in column 0
    if (!m_wndSplitter.CreateView(0, 0,
        RUNTIME_CLASS(CObjectView), CSize(200, 0), pContext))
    {
        TRACE(_T("Failed to create first pane\n"));
        return FALSE;
    }

    if (!m_wndSplitter.CreateView(0, 1,
        RUNTIME_CLASS(CVisionView), CSize(200, 0), pContext))
    {
        TRACE(_T("Failed to create second pane\n"));
        return FALSE;
    }
	m_pObjectView = (CObjectView*)m_wndSplitter.GetPane(0,0);
	m_pVisionView = (CVisionView*)m_wndSplitter.GetPane(0,1);

    // activate the input view
   SetActiveView((CView*)m_wndSplitter.GetPane(0, 0));

	CMenu*pMainMenu = GetMenu();
	CMenu*pSub = pMainMenu->GetSubMenu(4);
	int nPos = -1;
	if (pSub)
	{
		int i, nCount = pSub->GetMenuItemCount();
		for (i=0; i<nCount; i++)
		{
			if (pSub->GetMenuItemID(i) == ID_VIEW_MONITOR2)
				break;
		}
		if (i<nCount) nPos = i+1;
	}

#ifndef _DTS
	int iNrOfMonitors = m_MonitorInfo.GetNrOfMonitors();

	if (iNrOfMonitors > 1)
	{
		CVisionApp *pApp = (CVisionApp*)AfxGetApp();
		int i;
		CString str;
		DWORD dwMonitorFlag;
		for (i=1; i<iNrOfMonitors; i++)
		{
			dwMonitorFlag = (1 << i);
			if ((pApp->m_dwMonitorFlags & dwMonitorFlag) && (pApp->m_dwMonitorFlags & USE_MONITOR_1))
			{
				AddExternalFrame(i);
			}
			if (i>1)
			{
				str.Format(IDS_MONITOR, i+1);
				pSub->InsertMenu(nPos, MF_BYPOSITION, ID_VIEW_MONITOR1+i, str);
			}
		}
		if (!(pApp->m_dwMonitorFlags & USE_MONITOR_1))
		{
			SetWindowMonitor(true);
		}
	}
#endif

	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnUser(WPARAM wParam, LPARAM lParam)
{
	WORD wLow = LOWORD(wParam);
	if (wLow == WM_CLOSE)
	{
		POSITION pPosOld, pos = m_ExternalFrames.GetHeadPosition();
		CExternalFrameDlg *pExt;
		while (pos)
		{
			pPosOld = pos;
			pExt = (CExternalFrameDlg*)m_ExternalFrames.GetNext(pos);
			if (lParam == (LPARAM)pExt)
			{
				CVisionApp *pApp = (CVisionApp*)AfxGetApp();
				CRect rc;
				m_ExternalFrames.RemoveAt(pPosOld);
				pApp->m_dwMonitorFlags &= ~pExt->GetMonitorFlag();
				m_pVisionView->GetClientRect(&rc);
				m_pVisionView->SendMessage(WM_SIZE, 0, MAKELONG(rc.right, rc.bottom));
				break;
			}
		}
	}
	else if (wLow == GET_MONITOR_RECT)
	{
		int nMonitor = HIWORD(wParam);
		CRect rc;
		if (m_MonitorInfo.GetMonitorRect(nMonitor, rc, true))
		{
			*((CRect*)lParam) = rc;
		}
		return 1;
	}
	else if (wLow == REJECT_ANNOUNCEMENT)
	{
		AfxMessageBox(IDS_REJECT_ANNOUNCEMENT);
	}
	else if (wLow == NO_MEDIA_RECEIVE_CONFIRM)
	{
		WORD wServer = HIWORD(wParam);
		CSecID id((DWORD)lParam);
		int nResult = AfxMessageBox(IDS_NO_MEDIA_RECEIVE_CONFIRM, MB_YESNOCANCEL);
		if (nResult == IDYES)
		{
			CVisionDoc*pDoc = GetDocument();
			if (pDoc)
			{
				CIPCAudioVision *pAudio = NULL;
				if (wServer == 0)
				{
					pAudio = pDoc->GetLocalAudio();
				}
				else
				{
					CServer *pServer = pDoc->GetServer(wServer);
					if (pServer)
					{
						pAudio = pServer->GetAudio();
					}
				}
				if (pAudio)
				{
					pAudio->DoRequestSetValue(id, _T("ReleaseFilters"),_T(""));
					pAudio->ResetSentSamples();
				}
			}
		}
		else if (nResult == IDCANCEL)
		{
			PostMessage(WM_COMMAND, ID_AU_STOP_ANY);
		}
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::AddExternalFrame(int nMonitor)
{
	CExternalFrameDlg *pExternalFrame;
	pExternalFrame = new CExternalFrameDlg(this, m_pVisionView, nMonitor);
	pExternalFrame->Create(IDD_EXTERNAL_FRAME, this);
	m_ExternalFrames.AddHead(pExternalFrame);
}
//////////////////////////////////////////////////////////////////////////
bool CMainFrame::RemoveSWfromExtFrames(CSmallWindow *pSW)
{
	POSITION pos = m_ExternalFrames.GetHeadPosition();
	CExternalFrameDlg *pExt;
	while (pos)
	{
		pExt = (CExternalFrameDlg*)m_ExternalFrames.GetNext(pos);
		if (pExt->RemoveSmallWindow(pSW))
		{
			return true;
		}
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
int CMainFrame::GetNoOfExternalFrames()
{
	return m_ExternalFrames.GetCount();
}
//////////////////////////////////////////////////////////////////////////
CExternalFrameDlg* CMainFrame::GetExternalFrame(int nFrame)
{
	if (nFrame < m_ExternalFrames.GetCount())
	{
		POSITION pos = m_ExternalFrames.FindIndex(nFrame);
		if (pos)
		{
			return (CExternalFrameDlg*) m_ExternalFrames.GetAt(pos);
		}
	}
	return NULL;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewMonitor(UINT nID)
{
	CVisionApp *pApp          = (CVisionApp*)AfxGetApp();
	int         nMonitor      = nID - ID_VIEW_MONITOR2+1;
	DWORD       dwMonitorFlag = 1 << nMonitor;

	if (!(pApp->m_dwMonitorFlags & USE_MONITOR_1))
		return;

	if (dwMonitorFlag & pApp->m_dwMonitorFlags)
	{
		pApp->m_dwMonitorFlags &= ~dwMonitorFlag;
		for (int i=0; i<GetNoOfExternalFrames(); i++)
		{
			CExternalFrameDlg *pExtFrame = GetExternalFrame(i);
			if (pExtFrame->GetMonitorFlag() == dwMonitorFlag)
			{
				pExtFrame->PostMessage(WM_CLOSE, 0, 0);
			}
		}
	}
	else
	{
		pApp->m_dwMonitorFlags |= dwMonitorFlag;
		AddExternalFrame(nMonitor);
		PostMessage(WM_EXITSIZEMOVE); 
	}

	m_pVisionView->PostMessage(WM_COMMAND, ID_VIEW_MONITOR16, 0);
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewMonitor(CCmdUI* pCmdUI)
{
	CVisionApp *pApp = (CVisionApp*)AfxGetApp();
	int nMonitor = pCmdUI->m_nID - ID_VIEW_MONITOR2+1;
	DWORD dwMonitorFlag = 1 << nMonitor;
	if (!(pApp->m_dwMonitorFlags & USE_MONITOR_1))
	{
		dwMonitorFlag = 0;
		nMonitor = m_MonitorInfo.GetNrOfMonitors();
	}
	pCmdUI->Enable((nMonitor < m_MonitorInfo.GetNrOfMonitors()) ? TRUE : FALSE);
	pCmdUI->SetCheck((dwMonitorFlag & pApp->m_dwMonitorFlags) ? 1 : 0);
}
//////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL ret;
	
	ret =  CFrameWnd::PreCreateWindow(cs);

	cs.lpszClass = WK_APP_NAME_EXPLORER;
	cs.style &= ~FWS_ADDTOTITLE;
	cs.style |= WS_MAXIMIZE;

	return ret;
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
/////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
CVisionDoc* CMainFrame::GetDocument()
{
	if (WK_IS_WINDOW(m_pVisionView))
	{
		return m_pVisionView->GetDocument();
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnAppAbout() 
{
	COemGuiApi::AboutDialog(this);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDropDown(NMHDR* pNotifyStruct, LRESULT* pResult)
{
   // this function handles the dropdown menus from the toolbar
   NMTOOLBAR* pNMToolBar = (NMTOOLBAR*)pNotifyStruct;
   CRect rect;

    // translate the current toolbar item rectangle into screen coordinates
    // so that we'll know where to pop up the menu
	m_wndCoolBar.m_wndToolBar.SendMessage(TB_GETRECT, pNMToolBar->iItem, (LPARAM)&rect);
   m_wndCoolBar.m_wndToolBar.ClientToScreen(rect);

	if (pNMToolBar->iItem == ID_CONNECT)
	{
	   CMenu menu;

		int i,c;
		CHostArray& ha = theApp.GetHosts();
		CHost* pHost;
		CString sText;
		CString sLocal;
		c = ha.GetSize();

		menu.CreatePopupMenu();

		if (theApp.m_pPermission->IsSpecialReceiver() == FALSE)
		{
			sLocal.LoadString(IDS_LOCAL_SERVER);
			pHost = ha.GetLocalHost();
			sText.Format(_T("&%s %s"),sLocal,pHost->GetName());
			menu.AppendMenu(MF_STRING,
							ID_CONNECT_LOCAL,
							sText);
		}

		for (i=0;i<c && i<9;i++)
		{
			pHost = ha.GetAt(i);
			if (   pHost->GetTyp()!=_T("SMS")
				&& pHost->GetTyp()!=_T("E-Mail")
				&& pHost->GetTyp()!=_T("FAX"))
			{
				sText.Format(_T("&%x %s (%s)"),i+1,pHost->GetName(),pHost->GetTyp());
				menu.AppendMenu(MF_STRING,
								pHost->GetID().GetSubID()+ID_CONNECT_LOW,
								sText);
			}
		}
		if (c>9)
		{
			sText.LoadString(IDS_CONNECT);
			menu.AppendMenu(MF_STRING,
							ID_CONNECT,
							sText);
		}

		  // the font popup is stored in a resource
		  menu.TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left, rect.bottom, this);
	}
	else if (pNMToolBar->iItem == ID_DISCONNECT)
	{
		if (WK_IS_WINDOW(m_pObjectView))
		{
			m_pObjectView->PopupDisconnectMenu(CPoint(rect.left, rect.bottom));
		}
	}
	else if (pNMToolBar->iItem == ID_SEQUENZE)
	 {
		CMenu menu;
		CMenu* pPopup;

		menu.LoadMenu(IDR_CONTEXT);
		// Sequenzer Popup menu
		pPopup = menu.GetSubMenu(4);
#ifdef _DTS
		// Sequenzer erst ab 4 s
		pPopup->DeleteMenu(ID_SEQUENCE_1,MF_BYCOMMAND);
		pPopup->DeleteMenu(ID_SEQUENCE_2,MF_BYCOMMAND);
		pPopup->DeleteMenu(ID_SEQUENCE_3,MF_BYCOMMAND);
#endif
//      pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left, rect.bottom, this);
      pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON, rect.left, rect.bottom, AfxGetMainWnd());
	}

    *pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnConnect(UINT nID)
{
	CVisionDoc* pDoc = GetDocument();
	CSecID idHost(SECID_GROUP_HOST,(WORD)(nID-ID_CONNECT_LOW));
	pDoc->Connect(idHost);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDisconnect(UINT nID)
{
	if (WK_IS_WINDOW(m_pObjectView))
	{
		m_pObjectView->DisconnectID(nID-ID_DISCONNECT_LOW);
	}
}
/////////////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0700
void CMainFrame::OnActivateApp(BOOL bActive, DWORD dwThreadID) 
{
	CFrameWnd::OnActivateApp(bActive, dwThreadID);
#else
void CMainFrame::OnActivateApp(BOOL bActive, HTASK hTask) 
{
	CFrameWnd::OnActivateApp(bActive, hTask);
#endif // _MFC_VER >= 0x0700
	
	m_bActive = bActive;

	CVisionDoc* pDoc = GetDocument();
	if (pDoc)
	{
		if (pDoc->IsDecoderConnected())
		{
			pDoc->GetCIPCOutputVisionDecoder()->DoIndicateClientActive(bActive,SECID_NO_GROUPID);
		}

		// force redraw
		pDoc->UpdateViewRects(bActive);
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnMove(int x, int y) 
{
	CFrameWnd::OnMove(x, y);
	CVisionDoc* pDoc = GetDocument();
	if (pDoc)
	{
		pDoc->UpdateViewRects(TRUE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetStore(BOOL bOn)
{
	if (WK_IS_WINDOW(&m_wndCoolBar))
	{
		m_wndCoolBar.SetStore(bOn);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::UpdateToolbar()
{
/*	if (WK_IS_WINDOW(&m_wndCoolBar))
	{
		m_wndCoolBar.DestroyWindow();
		if (!m_wndCoolBar.Create(this,
			WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
				RBS_TOOLTIPS|RBS_BANDBORDERS|RBS_VARHEIGHT|CBRS_FLYBY)) {
			TRACE(_T("Failed to create cool bar\n"));
			// fail to create
			// gf todo send RTE
		}
	}
*/
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::ChangeFullScreenModus()
{
	OnViewFullscreen();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewFullscreen() 
{
	m_bFullScreen = !m_bFullScreen;
	if (m_bFullScreen)
	{
		m_hMenu = ::GetMenu(m_hWnd);
		::SetMenu(m_hWnd,NULL);
		GetWindowPlacement(&m_wp);
		CRect rect;

		m_pObjectView->GetClientRect(rect);
		m_iObjectWidth = rect.Width();
		m_wndSplitter.SetColumnInfo(0,0,0);

		ShowWindow(SW_RESTORE);
		ModifyStyle(WS_OVERLAPPEDWINDOW,0);
		CRect workingRect;
		BOOL bPrimary = TRUE;
		if (m_MonitorInfo.GetNrOfMonitors() > 1)
		{
			HMONITOR hMon = m_MonitorInfo.GetMonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST);
			int      iMon;
			if (m_MonitorInfo.GetMonitorFromHandle(hMon, iMon) && (iMon != 0))
			{
				bPrimary = !m_MonitorInfo.GetMonitorRect(iMon, workingRect, true);
			}
		}
		if (bPrimary)
		{
			SystemParametersInfo(SPI_GETWORKAREA,0,&workingRect,0);
		}

		SetWindowPos(&wndTop,
					workingRect.left,
					workingRect.top,
					workingRect.Width(),
					workingRect.Height(),
					0);

		ShowControlBar(&m_wndStatusBar,FALSE,FALSE);
		if (WK_IS_WINDOW(&m_wndCoolBar))
		{
			ShowControlBar(&m_wndCoolBar,FALSE,FALSE);
		}
		if (WK_IS_WINDOW(&m_wndToolBar))
		{
			ShowControlBar(&m_wndToolBar,FALSE,FALSE);
		}
		GetClientRect(rect);
		m_pVisionView->MoveWindow(rect);
	}
	else
	{
		::SetMenu(m_hWnd,m_hMenu);
		ModifyStyle(0,WS_OVERLAPPEDWINDOW);
		SetWindowPlacement(&m_wp);
		ShowControlBar(&m_wndStatusBar,TRUE,FALSE);
		if (WK_IS_WINDOW(&m_wndCoolBar))
		{
			ShowControlBar(&m_wndCoolBar,TRUE,FALSE);
		}
		if (WK_IS_WINDOW(&m_wndToolBar))
		{
			ShowControlBar(&m_wndToolBar,TRUE,FALSE);
		}
		m_wndSplitter.SetColumnInfo(0,m_iObjectWidth,m_iObjectWidth);
		m_wndSplitter.RecalcLayout();
		UpdateWindow();
	}
	for (int i=0; i<GetNoOfExternalFrames(); i++)
	{
		GetExternalFrame(i)->OnViewFullscreen(m_bFullScreen?true:false);
	}
	
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateViewFullscreen(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_pVisionView->GetNrOfDisplayWindows() || m_bFullScreen);
	pCmdUI->SetCheck(m_bFullScreen);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo) 
{
	if (WK_IS_WINDOW(m_pObjectView))
	{
		if (m_pObjectView->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
		{
			return TRUE;
		}
	}
	if (WK_IS_WINDOW(m_pVisionView))
	{
		if (m_pVisionView->OnCmdMsg(nID,nCode,pExtra,pHandlerInfo))
		{
			return TRUE;
		}
	}
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_EXPLORER, 0);

	return 0;
}
///////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
   LRESULT lResult;
	CString str;
   if (AFX_IDW_TOOLBAR == wParam)
   {
      wParam = m_wndCoolBar.HitTest();
   }
	else if ((ID_VIEW_MONITOR1 <= wParam) && (wParam <= ID_VIEW_MONITOR16))
	{
		str.Format(ID_VIEW_MONITOR2, wParam - ID_VIEW_MONITOR1 + 1);
      lParam = (LPARAM)LPCTSTR(str);
		wParam = 0;
	}

   lResult = CFrameWnd::OnSetMessageString(wParam, lParam);
   return lResult;
}
///////////////////////////////////////////////////////////////////
void CMainFrame::OnEnterMenuLoop(BOOL bIsTrackPopupMenu)
{
//	TRACE(_T("CMainFrame Enter menu\n"));
	CDisplayWindow* pDW = m_pVisionView->GetCmdActiveDisplayWindow();
	if (pDW)
	{
		pDW->SendMessage(WM_ENTERMENULOOP, (WPARAM)bIsTrackPopupMenu, NULL);
	}
}
///////////////////////////////////////////////////////////////////
void CMainFrame::OnExitMenuLoop(BOOL bIsTrackPopupMenu)
{
//	TRACE(_T("CMainFrame Exit menu\n"));
	CDisplayWindow* pDW = m_pVisionView->GetCmdActiveDisplayWindow();
	if (pDW)
	{
		pDW->SendMessage(WM_EXITMENULOOP, (WPARAM)bIsTrackPopupMenu, NULL);
	}
}
///////////////////////////////////////////////////////////////////
void CMainFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CFrameWnd::OnShowWindow(bShow, nStatus);
	
	if (bShow)
	{
	}
}
///////////////////////////////////////////////////////////////////
void CMainFrame::SetWindowMonitor(bool bMove)
{
	CVisionApp *pApp = (CVisionApp*)AfxGetApp();
	DWORD dwFlags;
	int   nMon;
	for (nMon=0, dwFlags = 1; nMon < m_MonitorInfo.GetNrOfMonitors(); nMon++, dwFlags<<=1)
	{
		if (pApp->m_dwMonitorFlags & dwFlags)
		{
			int nCurrent = -1;;
			m_MonitorInfo.GetMonitorFromHandle(m_MonitorInfo.GetMonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), nCurrent);
			if (nCurrent != nMon)
			{
				CRect rc;
				if (bMove)
				{
					CRect rcMon, rcCurr;
					m_MonitorInfo.GetMonitorRect(nMon, rcMon);
					m_MonitorInfo.GetMonitorRect(nCurrent, rcCurr);
					GetWindowRect(&rc);
					rc.OffsetRect(rcMon.TopLeft()-rcCurr.TopLeft());
				}
				else
				{
					m_MonitorInfo.GetMonitorRect(nMon, rc, true);
				}
				SetWindowPos(&CWnd::wndTop, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOOWNERZORDER);
			}
			break;
		}
	}
}
///////////////////////////////////////////////////////////////////
LRESULT CMainFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (WM_IDLEUPDATECMDUI == message)
	{
		int i, nFrames = GetNoOfExternalFrames();
		for (i=0; i<nFrames; i++)
		{
			CExternalFrameDlg *pWnd = GetExternalFrame(i);
			if (WK_IS_WINDOW(pWnd))
			{
				pWnd->WindowProc(message, wParam, lParam);
			}
		}	
	}
	else if (WM_EXITSIZEMOVE  == message)
	{
		if (GetNoOfExternalFrames())
		{
			int iMonitor;
			if (m_MonitorInfo.GetMonitorFromHandle(m_MonitorInfo.GetMonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), iMonitor))
			{
				if (iMonitor != 0)
				{
					WINDOWPLACEMENT wpl = {0};
					bool bMove = true;
					wpl.length = sizeof(WINDOWPLACEMENT);
					GetWindowPlacement(&wpl);
					if (wpl.showCmd == SW_SHOWMAXIMIZED)
					{
						bMove = false;
						ShowWindow(SW_RESTORE);
					}
					SetWindowMonitor(bMove);
				}
			}
		}
	}
	else if (WM_DISPLAYCHANGE == message)
	{
		m_MonitorInfo.Init();
		SetWindowMonitor(false);
		int i, nFrames = GetNoOfExternalFrames();
		for (i=0; i<nFrames; i++)
		{
			CExternalFrameDlg *pWnd = GetExternalFrame(i);
			if (WK_IS_WINDOW(pWnd))
			{
				pWnd->SetWindowSize();
			}
		}	
	}
	else if (WM_SYSCOMMAND == message)
	{
      if((wParam&0xfff0)==SC_SCREENSAVE)
		{
			TRACE(_T("SC_SCREENSAVE\n"));
		}

	}
		
	return CFrameWnd::WindowProc(message, wParam, lParam);
}

