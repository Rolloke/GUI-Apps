/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ICPCONUnit
// FILE:		$Workfile: minimizeddlg.cpp $
// ARCHIVE:		$Archive: /Project/Units/ICPCONUnit/minimizeddlg.cpp $
// CHECKIN:		$Date: 24.05.05 16:16 $
// VERSION:		$Revision: 8 $
// LAST CHANGE:	$Modtime: 24.05.05 15:35 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ICPCONUnit.h"
#include "MinimizedDlg.h"
#include "ICPCONDll\ICP7000Module.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CICPCONUnitApp theApp;
/////////////////////////////////////////////////////////////////////////////
// CMinimizedDlg dialog
CMinimizedDlg::CMinimizedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMinimizedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMinimizedDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_bFirstMouseMsg = TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CMinimizedDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMinimizedDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMinimizedDlg, CDialog)
	//{{AFX_MSG_MAP(CMinimizedDlg)
	ON_WM_MOUSEMOVE()
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMinimizedDlg message handlers

void CMinimizedDlg::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (m_bFirstMouseMsg)
	{
		m_bFirstMouseMsg = FALSE;
	}
	else
	{
		GetParent()->PostMessage(WM_USER, USER_MSG_RESTORE);
	}
	CDialog::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMinimizedDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	m_nTimer = SetTimer(0x0815, 2000, NULL);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CMinimizedDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		HMONITOR hMon = CMonitorInfo::GetMonitorFromWindow(GetParent()->m_hWnd, MONITOR_DEFAULTTONEAREST);
		int nXPos = 0, nYPos = 0, nMonitor = -1;
		if (hMon)
		{
			MONITORINFO mi;
			mi.cbSize = sizeof(MONITORINFO);
			if (CMonitorInfo::GetMonitorInfo(hMon, &mi))
			{
				CWK_Profile wkp1(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));
				CString str = wkp1.GetString(SETTINGS_SECTION, ICP_MINIMZE_POS, NULL);
				if (str.IsEmpty())
				{
					str = _T("none");
					wkp1.WriteString(SETTINGS_SECTION, ICP_MINIMZE_POS, str);
				}
				CRect rc;
				str.MakeLower();
				nMonitor = str.Find(_T("monitor:"));
				if (nMonitor != -1)
				{
					nMonitor = _ttoi(str.Mid(nMonitor + 8));
					CMonitorInfo cMI;
					if (cMI.GetMonitorHandle(nMonitor, hMon))
					{
						CMonitorInfo::GetMonitorInfo(hMon, &mi);
					}
				}
				if (str.Find(_T("none")) != -1)
				{
					GetParent()->GetWindowRect(&rc);
					CPoint ptParent = rc.CenterPoint();
					CPoint ptMonitor = ((CRect*)&mi.rcMonitor)->CenterPoint();
					if (ptParent.x < ptMonitor.x) str  = _T("left");
					else						  str  = _T("right");
					if (ptParent.y < ptMonitor.y) str += _T("top");
					else						  str += _T("bottom");
				}

				GetClientRect(&rc);
				if (str.Find(_T("left")) != -1)
				{
					nXPos = mi.rcMonitor.left;
				}
				else if (str.Find(_T("right")) != -1)
				{
					nXPos = mi.rcMonitor.right - rc.right;
				}
				else
				{
					nXPos = mi.rcMonitor.left + (mi.rcMonitor.right-mi.rcMonitor.left)/2 - rc.right / 2;
				}

				if (str.Find(_T("top")) != -1)
				{
					nYPos = mi.rcMonitor.top;
				}
				else if (str.Find(_T("bottom")) != -1)
				{
					nYPos = mi.rcMonitor.bottom - rc.bottom;
				}
				else
				{
					nYPos = mi.rcMonitor.top + (mi.rcMonitor.bottom-mi.rcMonitor.top)/2 - rc.bottom / 2;
				}
			}
		}
		
		::SetWindowPos(m_hWnd, HWND_TOPMOST, nXPos, nYPos, 0, 0, SWP_NOSIZE);
		m_bFirstMouseMsg = TRUE;
	}	
}
/////////////////////////////////////////////////////////////////////////////
void CMinimizedDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nTimer)
	{
		if (!theApp.m_bIsInMenuLoop && 
			!(GetWindowLong(m_hWnd, GWL_EXSTYLE) & WS_EX_TOPMOST))
		{
			::SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE|SWP_NOMOVE);
		}
	}
	
	CDialog::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
void CMinimizedDlg::OnDestroy() 
{
	SAFE_KILL_TIMER(m_nTimer);
	CDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
