// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "GemosUnit.h"

#include "MainFrm.h"
#include "RS232Gemos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(IDM_SEND_COMMANDS,	OnSendCommands)
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_CMD_RECEIVED,		OnCmdReceived)
	ON_COMMAND(IDM_TRACE_ACK, OnTraceACK)
	ON_COMMAND(IDM_TRACE_CMD, OnTraceCMD)
	ON_COMMAND(IDM_TRACE_DC3, OnTraceDC3)
	ON_COMMAND(IDM_TRACE_ENQ, OnTraceENQ)
	ON_COMMAND(IDM_TRACE_EOT, OnTraceEOT)
	ON_COMMAND(IDM_TRACE_NAK, OnTraceNAK)
	ON_COMMAND(IDM_TRACE_SYN, OnTraceSYN)
	ON_UPDATE_COMMAND_UI(IDM_TRACE_ACK, OnUpdateTraceACK)
	ON_UPDATE_COMMAND_UI(IDM_TRACE_CMD, OnUpdateTraceCMD)
	ON_UPDATE_COMMAND_UI(IDM_TRACE_DC3, OnUpdateTraceDC3)
	ON_UPDATE_COMMAND_UI(IDM_TRACE_ENQ, OnUpdateTraceENQ)
	ON_UPDATE_COMMAND_UI(IDM_TRACE_EOT, OnUpdateTraceEOT)
	ON_UPDATE_COMMAND_UI(IDM_TRACE_NAK, OnUpdateTraceNAK)
	ON_UPDATE_COMMAND_UI(IDM_TRACE_SYN, OnUpdateTraceSYN)
	ON_COMMAND(IDM_TRACE_STATE, OnTraceState)
	ON_UPDATE_COMMAND_UI(IDM_TRACE_STATE, OnUpdateTraceState)
	ON_COMMAND(IDM_TRACE_DISPATCH, OnTraceDispatch)
	ON_UPDATE_COMMAND_UI(IDM_TRACE_DISPATCH, OnUpdateTraceDispatch)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_MESSAGE(WM_TRAYCLICKED, OnTrayClicked)	
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

/////////////////////////////////////////////////////////////////////////////
CMainFrame::CMainFrame()
{
	CWK_Profile Prof;
	m_bTraceDispatch	= (BOOL)Prof.GetInt(szSection, _T("TraceDispatch"),		0);
	m_bTraceStateRequest= (BOOL)Prof.GetInt(szSection, _T("TraceStateRequest"), 0);
}

/////////////////////////////////////////////////////////////////////////////
CMainFrame::~CMainFrame()
{
	CWK_Profile Prof;
	Prof.WriteInt(szSection, _T("TraceDispatch"),		m_bTraceDispatch);
	Prof.WriteInt(szSection, _T("TraceStateRequest"),	m_bTraceStateRequest);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	BOOL ret =  CFrameWnd::PreCreateWindow(cs);

	cs.style = WS_OVERLAPPED | WS_CAPTION 
		| WS_THICKFRAME | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
	cs.lpszClass = WK_APPNAME_GEMOSUNIT;
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


/////////////////////////////////////////////////////////////////////////////
int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND|MF_GRAYED);

	// Icon ins Systemtray
	NOTIFYICONDATA tnd;

	CString sTip = COemGuiApi::GetApplicationName(WAI_GEMOS_UNIT);

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;

	tnd.uFlags		= NIF_MESSAGE|NIF_ICON|NIF_TIP;
	tnd.uCallbackMessage = WM_TRAYCLICKED;
	tnd.hIcon		= theApp.LoadIcon(IDR_MAINFRAME);

	lstrcpyn(tnd.szTip, sTip, sTip.GetLength()+1);
	Shell_NotifyIcon(NIM_ADD, &tnd);

	SetTimer(1, 60*1000, NULL);

	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnSendCommands() 
{
	CWK_Profile wkp;
	CStringArray sCmds;
	CString		 sCmdFileName = _T(".\\Script.txt");

	
	sCmdFileName = wkp.GetString(szSection,_T("CmdScript"),sCmdFileName);

	WK_FileToStringArray(sCmdFileName, sCmds);

	for (int nI = 0; nI < sCmds.GetSize(); nI++)
	{
		CString sCmd = sCmds[nI];
		sCmd.MakeLower();

		WK_TRACE(_T("Script: Cmd:%s\n"), sCmd);

		if (sCmd.Find(_T("wait"),0) != -1)
		{
			int nStart = sCmd.Find(_T("("))+1;
			int nEnd = sCmd.Find(_T(")"));

			if ((nStart != -1) && (nEnd != -1))
			{
				long lWait = _ttoi(sCmd.Mid(nStart, nEnd-nStart));
				Sleep(lWait);
			}
		}
		else if (sCmd.Find(_T("goto"),0) != -1)
		{
			int nLine	= 0;
			int nFrom	= 0;
			int nTo		= 0;
			if (_stscanf(sCmd, _T("goto %d, %d, %d"), &nLine, &nFrom, &nTo) != 3)
			{
				if (_stscanf(sCmd, _T("goto %d, %d"), &nLine, &nFrom) != 2)
				{
					if (_stscanf(sCmd, _T("goto %d, %d"), &nLine) != 1)
					{
					}
				}
			}
			if ((nFrom == nTo) && (nFrom != 0) && (nTo != 0))
				sCmds[nI].Format(_T("goto %d, %d, %d"), nLine, 0, nTo);
			else
			{
				nFrom++;
				sCmds[nI].Format(_T("goto %d, %d, %d"), nLine, nFrom, nTo);
				nI = nLine-1;
			}
		}
		else
		{
			if ((sCmds[nI].Left(1) != _T(";")) && (sCmds[nI].Left(1) != _T("#")) && (sCmds[nI].Left(2) != _T("//")))
			{
				theApp.GetRS232Gemos()->SendCommand(sCmds[nI]);	
				Sleep(100);
			}
		}
	}
	MessageBeep(0);
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::OnCmdReceived(WPARAM wParam, LPARAM lParam)
{
	// Ein Kommando aus der Kommandoqueue holen.
	CString sCmd = theApp.GetRS232Gemos()->GetAndRemoveCommand();
	if (sCmd.IsEmpty())
		return 0;

	if (sCmd == _T("S?"))
	{
		// "Statusübertragung Start" senden
		theApp.GetRS232Gemos()->SendCommand(_T("SA"));
		OnRequestState();
		// "Statusübertragung Ende" senden
		theApp.GetRS232Gemos()->SendCommand(_T("SE"));
	}
	else
	{
		// sCmd = LLLLMMZZ
		DWORD dwLine = 0, dwInput = 0, dwState = 0;
		if ((_stscanf(sCmd.Mid(0, 4),_T("%04x"),&dwLine )	== 1) &&
			 (_stscanf(sCmd.Mid(4, 2),_T("%02x"),&dwInput)	== 1) &&
			 (_stscanf(sCmd.Mid(6, 2),_T("%02x"),&dwState)	== 1))
		{	
			if ((dwLine <= 4999) && (dwInput <= 0xff) && (dwState <= 0xff))
				OnDispatchCommand((WORD)dwLine, (WORD)dwInput, (WORD)dwState, sCmd.Mid(8));
			else
				WK_TRACE_ERROR(_T("Line, Input or State out of range. (Linie=%w Input=%x, State=%x)\n"), dwLine, dwInput, dwState);
		}
		else
			WK_TRACE_ERROR(_T("Can't scan command <%s>\n"), sCmd);
	}


	return 0;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTraceDispatch() 
{
	m_bTraceDispatch = !m_bTraceDispatch;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTraceState() 
{
	m_bTraceStateRequest = !m_bTraceStateRequest;
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTraceACK() 
{
	theApp.GetRS232Gemos()->SetTraceACK(!theApp.GetRS232Gemos()->GetTraceACK());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTraceNAK() 
{
	theApp.GetRS232Gemos()->SetTraceNAK(!theApp.GetRS232Gemos()->GetTraceNAK());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTraceENQ() 
{
	theApp.GetRS232Gemos()->SetTraceENQ(!theApp.GetRS232Gemos()->GetTraceENQ());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTraceSYN() 
{
	theApp.GetRS232Gemos()->SetTraceSYN(!theApp.GetRS232Gemos()->GetTraceSYN());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTraceDC3() 
{
	theApp.GetRS232Gemos()->SetTraceDC3(!theApp.GetRS232Gemos()->GetTraceDC3());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTraceEOT() 
{
	theApp.GetRS232Gemos()->SetTraceEOT(!theApp.GetRS232Gemos()->GetTraceEOT());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTraceCMD() 
{
	theApp.GetRS232Gemos()->SetTraceCMD(!theApp.GetRS232Gemos()->GetTraceCMD());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateTraceState(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceStateRequest);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateTraceDispatch(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bTraceDispatch);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateTraceACK(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.GetRS232Gemos()->GetTraceACK());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateTraceNAK(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.GetRS232Gemos()->GetTraceNAK());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateTraceENQ(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.GetRS232Gemos()->GetTraceENQ());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateTraceSYN(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.GetRS232Gemos()->GetTraceSYN());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateTraceDC3(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.GetRS232Gemos()->GetTraceDC3());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateTraceEOT(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.GetRS232Gemos()->GetTraceEOT());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnUpdateTraceCMD(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(theApp.GetRS232Gemos()->GetTraceCMD());
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnDispatchCommand(WORD wLine, WORD wInput, WORD wState, const CString &sText)
{
	if (m_bTraceDispatch)
		WK_TRACE(_T("OnDispatchCommand Line=%d Melder=0x%02x Status=0x%02x Text=%s\n"), wLine, wInput, wState, sText);
	
	// Hier müssen die Kommandos an Vision weitergeleitet werden.
	theApp.OnDispatchCommand(wLine, wInput, wState, sText);
}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnRequestState()
{
	// Hier sollen nun der Status aller Melder an GEMOS übertragen werden...
	WK_TRACE(_T("OnRequestState\n"));

}

/////////////////////////////////////////////////////////////////////////////
void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if (!theApp.GetRS232Gemos()->TestingConnection())
		WK_TRACE(_T("Connectiontest failed\n"));
		
	CFrameWnd::OnTimer(nIDEvent);
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

	KillTimer(1);	
}

/////////////////////////////////////////////////////////////////////////////
long CMainFrame::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_GEMOS_UNIT, 0);

	return 0;
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
