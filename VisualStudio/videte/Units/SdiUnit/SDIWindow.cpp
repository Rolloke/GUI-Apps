/////////////////////////////////////////////////////////////////////////////
// FILE:		$Workfile: SDIWindow.cpp $
// ARCHIVE:		$Archive: /Project/Units/SDIUnit/SDIWindow.cpp $
// CHECKIN:		$Date: 22.06.05 14:28 $
// VERSION:		$Revision: 18 $
// LAST CHANGE:	$Modtime: 22.06.05 13:28 $
// BY AUTHOR:	$Author: Uwe.freiwald $
// $Nokeywords:$

#include "stdafx.h"
#include "messages.h"
#include "resource.h"
#include "SDIUnit.h"
#include "SDIWindow.h"
#include "RecordStarInterface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSDIUnitApp theApp;

#define IDC_STARINTERFACE	1

/////////////////////////////////////////////////////////////////////////////
CSDIWindow::CSDIWindow()
{
	CreateEx(WS_EX_OVERLAPPEDWINDOW,
			 WK_APP_NAME_SDIUNIT,
			 _T("SDIUnit"),
			 WS_OVERLAPPEDWINDOW,
			 CW_USEDEFAULT, CW_USEDEFAULT,
			 CW_USEDEFAULT, CW_USEDEFAULT,
             NULL, NULL, NULL);

		
	ShowWindow(SW_HIDE);

	NOTIFYICONDATA tnd;
	HICON	hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CMenu menu;
	menu.LoadMenu(IDR_MAINFRAME);
	SetMenu(&menu);
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
/////////////////////////////////////////////////////////////////////////////
CSDIWindow::~CSDIWindow()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSDIWindow::SetToolTip(const CString& sComs)
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
LRESULT CSDIWindow::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
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
				break;
			}
		case WM_LBUTTONDBLCLK:
			ShowWindow(SW_SHOW);
			break;
		}
	}
	
	return CWnd::DefWindowProc(message, wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDIWindow, CWnd)
	//{{AFX_MSG_MAP(CSDIWindow)
	ON_WM_DESTROY()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_ABOUT, OnAbout)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_RESET_SDI, OnResetSDI)
	ON_MESSAGE(WM_SELFCHECK, DoConfirmSelfcheck)
	ON_WM_SYSCOMMAND()
	ON_WM_CREATE()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BEGIN_EVENTSINK_MAP(CSDIWindow, CWnd)
	//{{AFX_EVENTSINK_MAP(CSDIWindow)
	ON_EVENT(CSDIWindow, IDC_STARINTERFACE, 3 /* CardEvent */, OnCardEventStarinterfacectrl, VTS_I2 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CSDIWindow, IDC_STARINTERFACE, 7 /* CommError */, OnCommErrorStarinterfacectrl, VTS_I2 VTS_BSTR)
	ON_EVENT(CSDIWindow, IDC_STARINTERFACE, 10 /* ConverterDisconnect */, OnConverterDisconnectStarinterfacectrl, VTS_I2)
	ON_EVENT(CSDIWindow, IDC_STARINTERFACE, 4 /* DoorOpenEvent */, OnDoorOpenEventStarinterfacectrl, VTS_I2 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CSDIWindow, IDC_STARINTERFACE, 6 /* ForcedDoorOpenEvent */, OnForcedDoorOpenEventStarinterfacectrl, VTS_I2 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CSDIWindow, IDC_STARINTERFACE, 5 /* InputEvent */, OnInputEventStarinterfacectrl, VTS_I2 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CSDIWindow, IDC_STARINTERFACE, 8 /* IOStatusEvent */, OnIOStatusEventStarinterfacectrl, VTS_I2 VTS_BSTR VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CSDIWindow, IDC_STARINTERFACE, 9 /* PollingError */, OnPollingErrorStarinterfacectrl, VTS_I2 VTS_BSTR VTS_BSTR)
	ON_EVENT(CSDIWindow, IDC_STARINTERFACE, 2 /* ReceiveData */, OnReceiveDataStarinterfacectrl, VTS_I2 VTS_BSTR)
	ON_EVENT(CSDIWindow, IDC_STARINTERFACE, 1 /* SendData */, OnSendDataStarinterfacectrl, VTS_I2 VTS_BSTR)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSDIWindow message handlers
/////////////////////////////////////////////////////////////////////////////
void CSDIWindow::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnDestroy() 
{
	NOTIFYICONDATA tnd;
	// Unused HICON	hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	CString sTip = "SDIUnit";

	tnd.cbSize		= sizeof(NOTIFYICONDATA);
	tnd.hWnd		= m_hWnd;
	tnd.uID			= 1;
	tnd.uFlags		= 0;

	Shell_NotifyIcon(NIM_DELETE, &tnd);

	CWnd::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnAppExit() 
{
	DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnAbout() 
{
	COemGuiApi::AboutDialog(this);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIWindow::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	return CWnd::OnCommand(wParam, lParam);
}
/////////////////////////////////////////////////////////////////////////////
long CSDIWindow::OnResetSDI(WPARAM wParam, LPARAM lParam)
{
	theApp.OnReset();
	return 0L;
}
/////////////////////////////////////////////////////////////////////////////
long CSDIWindow::DoConfirmSelfcheck(WPARAM wParam, LPARAM lParam)
{
	HWND hWnd = (HWND)wParam;

	if (hWnd && IsWindow(hWnd))
		::PostMessage(hWnd, WM_SELFCHECK, WAI_SDIUNIT, 0);

	return 0;
}
void CSDIWindow::OnSysCommand(UINT nID, LPARAM lParam)
{
	CWnd::OnSysCommand(nID, lParam);
	if (nID == SC_MINIMIZE)
	{
		ShowWindow(SW_HIDE);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
int CSDIWindow::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	CRect rect;
	if (!m_StarInterface.Create(_T("StarInterface"),WS_VISIBLE|WS_CHILD,rect,this,IDC_STARINTERFACE))
	{
		WK_TRACE_ERROR(_T("cannot create Star Interface Control\n"));
	}

	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnCardEventStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR ReaderIndex, LPCTSTR IDNumber, LPCTSTR EventDate, LPCTSTR EventTime, LPCTSTR EventStatus, LPCTSTR FunctionKey) 
{
	CSDIControlRecordArray& controls = theApp.GetControlRecords();
	for (int i=0;i<controls.GetSize();i++)
	{
		CSDIControlRecord* pSCR = controls.GetAtFast(i);
		if (   pSCR
			&& pSCR->GetType()==SDICT_STAR_INTERFACE)
		{
			CRecordStarInterface* pRSI = (CRecordStarInterface*)pSCR;
			pRSI->OnCardEventStarinterfacectrl(nIndex,BoardIndex,ReaderIndex,IDNumber,EventDate, EventTime, EventStatus, FunctionKey);
		}
	}

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnCommErrorStarinterfacectrl(short nIndex, LPCTSTR BoardIndex) 
{
	// TODO: Add your control notification handler code here

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnConverterDisconnectStarinterfacectrl(short nIndex) 
{
	// TODO: Add your control notification handler code here

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnDoorOpenEventStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR DoorIndex, LPCTSTR EventDate, LPCTSTR EventTime, LPCTSTR EventStatus) 
{
	// TODO: Add your control notification handler code here

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnForcedDoorOpenEventStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR DoorIndex, LPCTSTR EventDate, LPCTSTR EventTime, LPCTSTR EventStatus) 
{
	// TODO: Add your control notification handler code here

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnInputEventStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR InputPort, LPCTSTR EventDate, LPCTSTR EventTime, LPCTSTR EventStatus) 
{
	// TODO: Add your control notification handler code here

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnIOStatusEventStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR InputStatus, LPCTSTR OutputStatus, LPCTSTR FunctionKey) 
{
	// TODO: Add your control notification handler code here

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnPollingErrorStarinterfacectrl(short nIndex, LPCTSTR BoardIndex, LPCTSTR ErrorCode) 
{
	// TODO: Add your control notification handler code here

}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnReceiveDataStarinterfacectrl(short nIndex, LPCTSTR ReceiveString) 
{
	CSDIControlRecordArray& controls = theApp.GetControlRecords();
	for (int i=0;i<controls.GetSize();i++)
	{
		CSDIControlRecord* pSCR = controls.GetAtFast(i);
		if (   pSCR
			&& pSCR->GetType()==SDICT_STAR_INTERFACE)
		{
			CRecordStarInterface* pRSI = (CRecordStarInterface*)pSCR;
			pRSI->OnReceiveDataStarinterfacectrl(nIndex,ReceiveString);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////
void CSDIWindow::OnSendDataStarinterfacectrl(short nIndex, LPCTSTR SendString) 
{
	CSDIControlRecordArray& controls = theApp.GetControlRecords();
	for (int i=0;i<controls.GetSize();i++)
	{
		CSDIControlRecord* pSCR = controls.GetAtFast(i);
		if (   pSCR
			&& pSCR->GetType()==SDICT_STAR_INTERFACE)
		{
			CRecordStarInterface* pRSI = (CRecordStarInterface*)pSCR;
			pRSI->OnSendDataStarinterfacectrl(nIndex,SendString);
		}
	}
}
