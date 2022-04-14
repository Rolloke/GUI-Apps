// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "DVStorage.h"

#include "MainFrm.h"
#include ".\mainfrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	IDS_PANE_CLIENTS,
	IDS_PANE_PROCESS,
	IDS_PANE_FPS,
};
/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SYSCOMMAND()
	ON_WM_TIMECHANGE()
	//}}AFX_MSG_MAP
	ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_USER,OnUser)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
	ON_COMMAND(ID_SETTINGS, OnSettings)
	ON_UPDATE_COMMAND_UI(ID_SETTINGS, OnUpdateSettings)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// add member initialization code here
	
}

CMainFrame::~CMainFrame()
{
}

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

	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND|MF_GRAYED);

	// Icon ins Systemtray
	NOTIFYICONDATA tnd;

	CString sTip;
	
	GetWindowText(sTip);

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYCLICKED;
	tnd.hIcon		= theApp.LoadIcon(IDR_MAINFRAME);

	lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
	Shell_NotifyIcon(NIM_ADD, &tnd);

#ifdef _DEBUG
	SetTimer(1,30*1000,NULL);
#endif

	return 0;
}

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


void CMainFrame::OnTimer(UINT nIDEvent) 
{
#ifdef _DEBUG
	RedrawWindow();
//	PostMessage(WM_COMMAND,ID_FILE_FAST_RESET);
#endif
	
	CFrameWnd::OnTimer(nIDEvent);
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
					HWND hClient = ::FindWindow(_T("DVClient"), NULL);
					if (hClient != NULL && IsWindow(hClient))
					{
						WK_TRACE(_T("TKR ---- WM_CLOSE send to Client\n"));
						::PostMessage(hClient, WM_CLOSE, 0, 0);	
					}
					WK_TRACE(_T("TKR ----- WMClose send to Storage\n"));
					PostMessage(WM_CLOSE, 0, 0);
				}
			}
			else
			{
				theApp.OnDeviceRemoved(pHdr);
			}

		}
		else
		{
			theApp.OnDeviceRemoved(pHdr);
		}
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
	{
		::PostMessage(hWnd, WM_SELFCHECK, WAI_DATABASE_SERVER, 0);
	}

	return 0;
}
/////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnUser(WPARAM wParam, LPARAM lParam)
{
	if ((wParam != 0) || (lParam != 0))
	{
		WORD wCause	  = LOWORD(wParam);
//		WORD wInfo = HIWORD(wParam);
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
		case WPARAM_BACKUP_SCAN_FINISHED:
			theApp.OnBackupScanFinished();
			break;
		case WPARAM_VERIFY_FINISHED:
			theApp.OnVerifyFinished();
			break;
		}
	}
	return 0L;
}

void CMainFrame::OnDestroy() 
{
	if (theApp.m_Dlg.m_hWnd)
	{
		theApp.m_Dlg.SendMessage(WM_COMMAND, IDCANCEL);
	}
	CFrameWnd::OnDestroy();
	
	NOTIFYICONDATA tnd;

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);
	
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CMainFrame::OnTrayClicked(WPARAM wParam, LPARAM lParam)
{
	switch (lParam)
	{
		case WM_LBUTTONDBLCLK:
			ShowWindow(SW_RESTORE); //SW_SHOWNORMAL);
			break;
		case WM_RBUTTONDOWN:
		{
			CMenu menu;
			CMenu* pM;
			CPoint pt;

			GetCursorPos(&pt);
			menu.LoadMenu(IDR_MAINFRAME);
			pM = menu.GetSubMenu(0);

			SetForegroundWindow();		// Siehe ID: Q135788 
			pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);
			PostMessage(WM_NULL, 0, 0); // Siehe ID: Q135788
			break;
		}
	}

	return 0;
}

void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{
	CFrameWnd::OnSysCommand(nID, lParam);
	if (nID == SC_MINIMIZE)
		ShowWindow(SW_HIDE);
}

void CMainFrame::OnTimeChange() 
{
	CFrameWnd::OnTimeChange();
	
	WK_STAT_PEAK(_T("Server"),1,_T("WM_TIMECHANGE"));
}

void CMainFrame::OnSettings()
{
	if (!theApp.m_Dlg.m_hWnd)
	{
		theApp.m_Dlg.Create(CDlgSettings::IDD, this);
	}
}

void CMainFrame::OnUpdateSettings(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(theApp.m_Dlg.m_hWnd == NULL);
}
