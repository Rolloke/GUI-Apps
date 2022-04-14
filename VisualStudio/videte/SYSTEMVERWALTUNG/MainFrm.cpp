// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "AFXPRIV.H"
#include "oemgui\oemgui.h"

#include "MainFrm.h"
#include "SVDoc.h"
#include "SVView.h"
#include "SVPropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame
IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
    ON_COMMAND_RANGE(ID_CONNECT_LOW, ID_CONNECT_HIGH, OnConnect)
    ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnDropDown)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
	ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_KEYBOARD_CHANGED, OnKeyboardChanged)
	ON_COMMAND(ID_VIEW_PROFESSIONAL, OnViewProfessional)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/*
	// Global help commands
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
*/
/////////////////////////////////////////////////////////////////////////////
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
	ID_INDICATOR_EXT,
};
/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	m_nTimer = 0;
}
/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
}
/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	WK_TRACE(_T("ComCtl32Version: %d\n"), COemGuiApi::GetComCtl32Version());
	if (COemGuiApi::GetComCtl32Version()==COMCTL32_471 || theApp.IsVista())
	{
		if (!m_wndCoolBar.Create(this,
			WS_CHILD|WS_VISIBLE|WS_BORDER|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
				RBS_TOOLTIPS|RBS_BANDBORDERS|RBS_VARHEIGHT)) {
			TRACE0("Failed to create cool bar\n");
			return -1;      // fail to create
		}
	}
	else
	{
		if (!m_wndToolBar.Create(this) ||
			!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
		{
			TRACE0("Failed to create tool bar\n");
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
					s = s.Left(s.Find('\n'));
					m_wndToolBar.SetButtonText(i,s);
				}
			}
		}
		m_wndToolBar.GetToolBarCtrl().SetButtonSize(CSize(72,42));
	}

	int nIndicators = sizeof(indicators)/sizeof(UINT);
	if (!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, nIndicators))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
/* 
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);
*/ 

// RKE: CoolMenuManager funktioniert nicht im MFC 7.x
#if _MFC_VER < 0x0700
	m_CoolMenuManager.m_bAutoAccel = FALSE;
	m_CoolMenuManager.Install(this);
	m_CoolMenuManager.LoadToolbar(IDR_MAINFRAME);
#endif

   // Vorläufig das Wizardmenü nur im Debugmodus
	/*
	if (theApp.m_bProfessional == FALSE)
	{
		CMenu *pMenu = GetMenu();
		if (pMenu)
		{
			pMenu->DeleteMenu(5, MF_BYPOSITION);
		}
	}*/
	SetMonitor();
	OnKeyboardChanged(0,0);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL ret;
	
	ret =  CFrameWnd::PreCreateWindow(cs);

	cs.lpszClass = WK_APP_NAME_SUPERVISOR;
	cs.style &= ~FWS_ADDTOTITLE;
	cs.style |= WS_MAXIMIZE;

	return ret;
}
/////////////////////////////////////////////////////////////////////////////
CWnd* CMainFrame::GetToolBar()
{
	if (m_wndCoolBar.m_hWnd)
	{
		return &m_wndCoolBar;
	}
	if (m_wndToolBar.m_hWnd)
	{
		return &m_wndToolBar;
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics
#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG
/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
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
		CWK_Profile wkp;	// ML 1.9.99 Sollte nicht lieber GetProfile() benutzt werden?
		CHostArray ha;
		CHost* pHost;
		CString sText;
		CString sLocal;
		ha.Load(wkp);
		c = ha.GetSize();

		menu.CreatePopupMenu();

		for (i=0;i<c && i<9;i++)
		{
			pHost = ha.GetAt(i);
			if (   (pHost->GetTyp()!=_T("SMS")) 
				&& (pHost->GetTyp()!=_T("MINI B3"))
				&& (pHost->GetTyp()!=_T("TeleObserver"))
				&& (pHost->GetTyp()!=_T("FAX"))
				&& (pHost->GetTyp()!=_T("E-Mail"))
				&& (pHost->GetTyp()!=_T("MINI B6")))
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

    *pResult = 0;
}
/////////////////////////////////////////////////////////////////////////////
CSVDoc* CMainFrame::GetDocument()
{
	return (CSVDoc*)GetActiveDocument();
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnConnect(UINT nID)
{
	CSVDoc* pDoc = GetDocument();
	CSecID idHost(SECID_GROUP_HOST, (WORD)(nID-ID_CONNECT_LOW));
	pDoc->Connect(idHost);
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_SUPERVISOR, 0);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	return CFrameWnd::OnNotify(wParam, lParam, pResult);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
   LRESULT lResult;
   bool bToolBar = false;
   if (AFX_IDW_TOOLBAR == wParam)
   {
      wParam = m_wndCoolBar.HitTest();
      bToolBar = true;
   }
   lResult = CFrameWnd::OnSetMessageString(wParam, lParam);
/*
   if (bToolBar)
   {
   	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
      TRACE(_T("LastStatus %x\n"), pThreadState->m_nLastStatus);
   }
*/
   return lResult;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CMainFrame::WizardCloseProc(CFrameWnd *pFrameWnd)
{
	CView *pView = pFrameWnd->GetActiveView();
	ASSERT_KINDOF(CSVView, pView);
	((CSVView*)pView)->GetSVWizard()->SendMessage(WM_COMMAND, IDCANCEL);
	return false;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetWizardCloseProc(bool bSet)
{
	if (bSet)
	{
		if (!m_lpfnCloseProc)
		{
			m_lpfnCloseProc = WizardCloseProc;
		}
	}
	else
	{
		if (m_lpfnCloseProc == WizardCloseProc)
		{
			m_lpfnCloseProc = NULL;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::SetMonitor()
{
	if (theApp.GetDongle()->AllowMultiMonitor() && theApp.m_nCmdShow != SW_HIDE)
	{
		DWORD dwFlags, dwMonitorFlags;
		CWK_Profile wkp;
		CMonitorInfo MonitorInfo;
		dwMonitorFlags = wkp.GetInt(REG_KEY_SUPERVISOR, REG_KEY_USE_MONITOR, 1);
		ShowWindow(SW_HIDE);
		int   nMon, nMonitors = MonitorInfo.GetNrOfMonitors();
		for (nMon=0, dwFlags = 1; nMon < nMonitors; nMon++, dwFlags<<=1)
		{
			if (dwMonitorFlags & dwFlags)
			{
				int nCurrent = -1;;
				MonitorInfo.GetMonitorFromHandle(MonitorInfo.GetMonitorFromWindow(m_hWnd, MONITOR_DEFAULTTONEAREST), nCurrent);
				if (nCurrent != nMon)
				{
					CRect rc;
					WINDOWPLACEMENT wpl = {0};
					wpl.length = sizeof(WINDOWPLACEMENT);
					GetWindowPlacement(&wpl);
					ShowWindow(SW_RESTORE);

					CRect rcMon, rcCurr;
					MonitorInfo.GetMonitorRect(nMon, rcMon);
					MonitorInfo.GetMonitorRect(nCurrent, rcCurr);
					rc = wpl.rcNormalPosition;
					if (rc.IsRectEmpty())
					{
						rc = rcCurr;
					}
					else
					{
						rc.OffsetRect(rcMon.TopLeft()-rcCurr.TopLeft());
					}

					SetWindowPos(&CWnd::wndTop, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOOWNERZORDER);
					PostMessage(WM_SYSCOMMAND, wpl.showCmd == SW_SHOWMAXIMIZED ? SC_MAXIMIZE : SC_RESTORE);
				}
				break;
			}
		}
		ShowWindow(SW_SHOW);
	}
}
//////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnDeviceChange( UINT nEventType, DWORD dwData )
{
	WK_TRACE(_T("OnDeviceChange\n"));
	DEV_BROADCAST_HDR* pHdr = (DEV_BROADCAST_HDR*)dwData;
	if (pHdr)
	{
		if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
		{
			DEV_BROADCAST_VOLUME *pDBV  = (DEV_BROADCAST_VOLUME*)pHdr;
			_TCHAR  szDrive[] = _T("A:");
			int   nDrive;
			DWORD dwUnitMask = pDBV->dbcv_unitmask;
			for (nDrive=-1; nDrive<32 && dwUnitMask!=0; nDrive++) dwUnitMask >>= 1;
			szDrive[0] = (_TCHAR)(_T('A') + nDrive);
			UINT nType = GetDriveType(szDrive);
			if (DRIVE_REMOVABLE == nType)
			{
				if (nEventType == DBT_DEVICEARRIVAL)
				{
					theApp.m_sOpenDir = szDrive;
				}
				else
				{
					theApp.ReadOpenDirPath();
				}
			}
			else
			{

			}
			if (GetDocument()->IsLocal())
			{
				GetDocument()->LoadDrives(TRUE);
			}
		}
	}

	if (m_nTimer == 0)
	{
		m_nTimer = SetTimer(0x0815, 2000, NULL);
	}
	return 1;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nTimer)
	{
		WK_TRACE(_T("OnTimer\n"));
		KillTimer(m_nTimer);
		m_nTimer = 0;
		CView *pView = GetActiveView();
		if (pView)
		{
			pView->SendMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, 1<<IMAGE_HARDWARE);
		}
	}
	CFrameWnd::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnKeyboardChanged(WPARAM wParam, LPARAM lParam)
{
	int i, n = sizeof(indicators) / sizeof(indicators[0]);
	for (i=n-1; i>=0; i--)
	{
		if (ID_INDICATOR_EXT == indicators[i])
			break;
	}
	if (wParam)
	{
		CKBInput*pActive = (CKBInput*)wParam;
		GetStatusBar()->SetPaneText(i, pActive->GetShortName());
	}
	else
	{
		GetStatusBar()->SetPaneText(i, _T(""));
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////
void CMainFrame::OnViewProfessional()
{
	theApp.m_bProfessional = !theApp.m_bProfessional;
}
