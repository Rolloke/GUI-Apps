// GAWindow.cpp : implementation file
//

#include "stdafx.h"
#include "GAUnit.h"
#include "GAWindow.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGAWindow
CGAWindow::CGAWindow()
{
	CreateEx(WS_EX_OVERLAPPEDWINDOW,WK_APP_NAME_GAUUNIT,"GA - Unit",
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

CGAWindow::~CGAWindow()
{
	PostQuitMessage(0);
}

BEGIN_MESSAGE_MAP(CGAWindow, CWnd)
	//{{AFX_MSG_MAP(CGAWindow)
	ON_WM_DESTROY()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_ABOUT, OnAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGAWindow message handlers
void CGAWindow::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CGAWindow::OnDestroy() 
{
	NOTIFYICONDATA tnd;
	CString sTip = "GA - Unit";

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);

	CWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CGAWindow::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
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
				pM->TrackPopupMenu(TPM_LEFTALIGN,pt.x,pt.y,this);		
				}
				break;
			}
		}
	
	return CWnd::DefWindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
void CGAWindow::OnAppExit() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CGAWindow::OnAbout() 
{
	COemGuiApi::AboutDialog(this);
}
