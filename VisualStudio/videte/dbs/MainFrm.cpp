// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DBS.h"

#include "MainFrm.h"

#include "OscopeView.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ID_VIEW_OSCOPE (AFX_IDW_PANE_FIRST+1)
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_WM_TIMER()
	ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_MESSAGE(WM_USER,OnUser)
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)	
	ON_COMMAND_RANGE(AFX_IDW_PANE_FIRST, AFX_IDW_PANE_LAST, OnView)
	ON_UPDATE_COMMAND_UI_RANGE(AFX_IDW_PANE_FIRST, AFX_IDW_PANE_LAST, OnUpdateView)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#define INDEX_NO_OF_BUBBLES 1
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_NUM,		// indicates number of bubbles
};
/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction
CMainFrame::CMainFrame()
{
	m_uTimerID = 0;
	m_pScopeView = NULL;
}

CMainFrame::~CMainFrame()
{
}
/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_DATABASE_SERVER, 0);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	CWK_Profile wkp;
	if (wkp.GetInt(SECTION_DBS, _T("CreateScopeView"), 0))
	{
		CreateScopeView();
	}

	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND|MF_GRAYED);
	// Icon ins Systemtray
	NOTIFYICONDATA tnd;

	CString sTip = COemGuiApi::GetApplicationName(WAI_DATABASE_SERVER);

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYCLICKED;
	tnd.hIcon		= theApp.LoadIcon(IDR_MAINFRAME);

	lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
	Shell_NotifyIcon(NIM_ADD, &tnd);

	m_uTimerID = SetTimer(1,5*1000,NULL);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL ret =  CFrameWnd::PreCreateWindow(cs);

	cs.style = WS_OVERLAPPED | WS_CAPTION 
		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	cs.lpszClass = WK_APP_NAME_DB_SERVER;
	cs.lpszName = AfxGetApp()->m_pszAppName;

	return ret;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDestroy() 
{
	CFrameWnd::OnDestroy();
	
	// Nur erforderlich, wenn Icon in der Systemtaskleiste
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);

	KillTimer(m_uTimerID);
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

void CMainFrame::OnAppAbout() 
{
	COemGuiApi::AboutDialog(this);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::OnDeviceChange(UINT nEventType, DWORD dwData)
{
	DEV_BROADCAST_HDR* pHdr = (DEV_BROADCAST_HDR*)dwData;

	if (pHdr)
	{
		if (nEventType == DBT_DEVICEARRIVAL)
		{
			theApp.OnDeviceArrived(pHdr);
		}
		else if(nEventType == DBT_DEVICEREMOVECOMPLETE)
		{
			if(theApp.IsReadOnly())
			{
				if (pHdr->dbch_devicetype == DBT_DEVTYP_VOLUME)
				{
					HWND hClient = NULL;
					if (theApp.IsDTS())
					{
						hClient = ::FindWindow(WK_APP_NAME_DVCLIENT, NULL);
					}
					else
					{
						hClient = ::FindWindow(WK_APP_NAME_IDIP_CLIENT, NULL);
					}
					if (hClient != NULL && IsWindow(hClient))
					{
						::PostMessage(hClient, WM_CLOSE, 0, 0);	
					}
					PostMessage(WM_CLOSE, 0, 0);
				}
			}
			else
			{
				theApp.OnDeviceRemoved(pHdr);
			}
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_LBUTTONDBLCLK:
			ShowWindow(SW_RESTORE); //SW_SHOWNORMAL);
			PostMessage(WM_COMMAND,ID_FILE_ACTUALIZE);
			break;
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MAINFRAME);
			pM = menu.GetSubMenu(0);
			CString sAbout;
			menu.GetMenuString(ID_APP_ABOUT, sAbout, MF_BYCOMMAND);
			pM->InsertMenu(0, MF_STRING, ID_APP_ABOUT, (LPCTSTR)sAbout);

			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	CFrameWnd::OnSysCommand(nID, lParam);

	if (nID == SC_MINIMIZE)
		ShowWindow(SW_HIDE);
}
/////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnUser(WPARAM wParam, LPARAM lParam)
{
	if ((wParam != 0) || (lParam != 0))
	{
		WORD wCause	  = LOWORD(wParam);
		WORD wInfo = HIWORD(wParam);
		CSecID id = (DWORD)(lParam);

		switch (wCause)
		{
		case WPARAM_REQUEST_BACKUP:
			theApp.OnRequestBackup();
			break;
		case WPARAM_BACKUP_FINISHED:
			theApp.OnBackupFinished();
			break;
		case WPARAM_REQUEST_QUERY:
			theApp.OnRequestQuery();
			break;
		case WPARAM_QUERY_FINISHED:
			theApp.OnQueryFinished();
			break;
		case WPARAM_CONNECTION_RECORD:
			theApp.OnConnectionRecord();
			break;
		case WPARAM_INITIALIZE_THREAD:
			theApp.CheckInitializeThreadFinished();
			break;
		case WPARAM_SEND_ALARM_FUEL:
			theApp.SendAlarmFuel();
			break;
		case WPARAM_DRIVE_CHANGED:
			theApp.OnDriveChanged(wInfo, (_TCHAR)LOWORD(lParam));
			break;
		}
	}
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (m_uTimerID == nIDEvent)
	{
		theApp.OnTimer();
	}
	
	CFrameWnd::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnIdle()
{
	_TCHAR szBubbles[16];
	_itot(CIPCExtraMemory::GetCreatedBubbles(), szBubbles, 10);
	m_wndStatusBar.SetPaneText(INDEX_NO_OF_BUBBLES, szBubbles);
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnLanguageChanged(WPARAM wParam, LPARAM lParam)
{
#if _MFC_VER >= 0x0710
	return theApp.SetLanguageParameters((UINT)wParam, (DWORD)lParam);
#else
	return 0;
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnView(UINT nView)
{
	CWnd*pWnd = GetDlgItem(nView);
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CView)))
	{
		CView *pSetActive = (CView*)pWnd;
		CView *pActive = GetActiveView();
		if (pSetActive != pActive)
		{
			pActive->ShowWindow(SW_HIDE);
			CRect rc;
			GetClientRect(&rc);
			SetActiveView(pSetActive);
			OnSize(0, rc.Width(), rc.Height());
			pSetActive->ShowWindow(SW_SHOW);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateView(CCmdUI *pCmdUI)
{
	CWnd*pWnd = GetDlgItem(pCmdUI->m_nID);
	pCmdUI->Enable(pWnd != NULL);
	pCmdUI->SetCheck(m_pViewActive == pWnd);
}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::RecalcLayout(BOOL bNotify)
{
	if (m_bInRecalcLayout)
		return;

	m_bInRecalcLayout = TRUE;
	int nIDPane = AFX_IDW_PANE_FIRST;
	CView *pView = GetActiveView();
	if (pView)
	{
		nIDPane = pView->GetDlgCtrlID();
	}
	// clear idle flags for recalc layout if called elsewhere
	if (m_nIdleFlags & idleNotify)
		bNotify = TRUE;
	m_nIdleFlags &= ~(idleLayout|idleNotify);

	// reposition all the child windows (regardless of ID)
	if (GetStyle() & FWS_SNAPTOBARS)
	{
		CRect rect(0, 0, 32767, 32767);
		RepositionBars(0, 0xffff, nIDPane, reposQuery,
			&rect, &rect, FALSE);
		RepositionBars(0, 0xffff, nIDPane, reposExtra,
			&m_rectBorder, &rect, TRUE);
		CalcWindowRect(&rect);
		SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(),
			SWP_NOACTIVATE|SWP_NOMOVE|SWP_NOZORDER);
	}
	else
		RepositionBars(0, 0xffff, nIDPane, reposExtra, &m_rectBorder);
	m_bInRecalcLayout = FALSE;

}
/////////////////////////////////////////////////////////////////////////////
void CMainFrame::CreateScopeView()
{
	CWnd*pWnd = GetDlgItem(ID_VIEW_OSCOPE);
	if (pWnd == NULL)
	{
		CMenu*pMenu = GetMenu();

		CMenu *pSubView = pMenu->GetSubMenu(1);
		CString sName;
		pSubView->AppendMenu(MF_BYPOSITION|MF_SEPARATOR);
		sName = _T("List");
		pSubView->AppendMenu(MF_BYPOSITION, AFX_IDW_PANE_FIRST, sName);
		sName = _T("Scope");
		pSubView->AppendMenu(MF_BYPOSITION, ID_VIEW_OSCOPE, sName);

		CCreateContext cc;
		cc.m_pNewViewClass = RUNTIME_CLASS(COscopeView);
		pWnd = CreateView(&cc, ID_VIEW_OSCOPE);
		if (pWnd && pWnd->IsKindOf(cc.m_pNewViewClass))
		{
			m_pScopeView = (COscopeView*)pWnd;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
COscopeView* CMainFrame::GetOscopeView()
{
	return 	m_pScopeView;
}
/////////////////////////////////////////////////////////////////////////////
