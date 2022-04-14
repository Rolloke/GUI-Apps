// CommWindow.cpp : implementation file
//

#include "stdafx.h"
#include "CommUnit.h"
#include "CommWindow.h"
#include "wk_names.h"
#include "CameraCommandRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//extern CCommUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CCommWindow
CCommWindow::CCommWindow()
{
	CreateEx(WS_EX_OVERLAPPEDWINDOW,WK_APP_NAME_COMMUNIT,_T("Comm - Unit"),
			 WS_OVERLAPPEDWINDOW,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 CW_USEDEFAULT, CW_USEDEFAULT,
             NULL, NULL, NULL);

		
	ShowWindow(SW_HIDE);

	NOTIFYICONDATA tnd;
	HICON	hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CString sTip = AfxGetAppName();

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_USER;
	tnd.hIcon		= hIcon;

	lstrcpyn(tnd.szTip, sTip.GetBuffer(0), sTip.GetLength()+1);
	sTip.ReleaseBuffer();
	Shell_NotifyIcon(NIM_ADD, &tnd);
}

CCommWindow::~CCommWindow()
{
}

BEGIN_MESSAGE_MAP(CCommWindow, CWnd)
	//{{AFX_MSG_MAP(CCommWindow)
	ON_WM_DESTROY()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_ABOUT, OnAbout)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_LANGUAGE_CHANGED, OnLanguageChanged)	
	ON_COMMAND(ID_VIEW_DEBUGINFO, OnViewDebuginfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CCommWindow::SetToolTip(const CString& sComs)
{
	NOTIFYICONDATA tnd;
	CString sTip = AfxGetAppName();

	sTip += sComs;
	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;

	tnd.uFlags		= NIF_TIP;

	lstrcpyn(tnd.szTip, sTip.GetBuffer(0), sTip.GetLength()+1);
	sTip.ReleaseBuffer();
	Shell_NotifyIcon(NIM_MODIFY, &tnd);
}
/////////////////////////////////////////////////////////////////////////////
// CCommWindow message handlers
void CCommWindow::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CCommWindow::OnDestroy() 
{
	NOTIFYICONDATA tnd;
	HICON hIcon = NULL;
	hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CString sTip = _T("GA - Unit");

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);

	CWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CCommWindow::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message==WM_USER)
		{
		switch (lParam)
			{
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
				}
				break;
			}
		}
	
	return CWnd::DefWindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CCommWindow::OnAppExit() 
{
	if (m_Dlg.m_hWnd)
	{
		m_Dlg.SendMessage(WM_COMMAND, IDCANCEL);
	}
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CCommWindow::OnAbout() 
{
	COemGuiApi::AboutDialog(this);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CCommWindow::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if (wParam==ID_WRITE_DATA)
	{
		theApp.OnWriteData();
		return TRUE;
	}

	if (wParam==ID_CAMERA_COMMAND)
	{
		CCameraCommandRecord* pCCommandR = (CCameraCommandRecord*)lParam;
		theApp.OnCameraCommand(pCCommandR);
		WK_DELETE(pCCommandR);
		return TRUE;
	}

	if (wParam==ID_RESET)
	{
		theApp.OnReset();
		return TRUE;
	}

/*	if (wParam==ID_TEST)
	{
		theApp.OnTest();
		return TRUE;
	}*/

	return CWnd::OnCommand(wParam, lParam);
}

/////////////////////////////////////////////////////////////////////////////
long CCommWindow::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_COMMUNIT, 0);

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CCommWindow::OnLanguageChanged(WPARAM wParam, LPARAM lParam)
{
#if _MFC_VER >= 0x0710
	return theApp.SetLanguageParameters((UINT)wParam, (DWORD)lParam);
#else
	return 0;
#endif

}
/////////////////////////////////////////////////////////////////////////////
void CCommWindow::OnViewDebuginfo()
{
	if (!m_Dlg.m_hWnd)
	{
		m_Dlg.Create(CDebugTraceDlg::IDD, this);
	}
}
