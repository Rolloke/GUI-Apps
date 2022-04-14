// MinimizedDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ICPCONClient.h"
#include "MinimizedDlg.h"
#include "multimon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CIPCCONClient theApp;
/////////////////////////////////////////////////////////////////////////////
// CMinimizedDlg dialog
CMinimizedDlg::CMinimizedDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMinimizedDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMinimizedDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
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
	ON_WM_SHOWWINDOW()
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMinimizedDlg message handlers

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
		HMONITOR hMon = MonitorFromWindow(GetParent()->m_hWnd, MONITOR_DEFAULTTONEAREST);
		int nXPos = 0, nYPos = 0;
		if (hMon)
		{
			MONITORINFO mi;
			mi.cbSize = sizeof(MONITORINFO);
			if (GetMonitorInfo(hMon, &mi))
			{
				CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));
				CString str = wkp.GetString(SETTINGS_SECTION, ICP_MINIMZE_POS, NULL);
				if (str.IsEmpty())
				{
					str = _T("LeftBottom");
					wkp.WriteString(SETTINGS_SECTION, ICP_MINIMZE_POS, str);
				}
				CRect rc;
				GetClientRect(&rc);
				if (str.Find(_T("Left")) != -1)
				{
					nXPos = mi.rcMonitor.left;
				}
				else if (str.Find(_T("Right")) != -1)
				{
					nXPos = mi.rcMonitor.right - rc.right;
				}
				else
				{
					nXPos = mi.rcMonitor.left + (mi.rcMonitor.right-mi.rcMonitor.left)/2 - rc.right / 2;
				}

				if (str.Find(_T("Top")) != -1)
				{
					nYPos = mi.rcMonitor.top;
				}
				else if (str.Find(_T("Bottom")) != -1)
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
	}	
}
/////////////////////////////////////////////////////////////////////////////
void CMinimizedDlg::OnTimer(UINT nIDEvent) 
{
	if (nIDEvent == m_nTimer)
	{
		if (!theApp.m_bIsInMenuLoop)
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

void CMinimizedDlg::OnLButtonUp(UINT nFlags, CPoint point) 
{
	GetParent()->PostMessage(WM_USER, USER_MSG_RESTORE);

	CDialog::OnLButtonUp(nFlags, point);
}
