// ExternalFrameDlg.cpp : implementation file
//

#include "stdafx.h"
#include "vision.h"
#include "ExternalFrameDlg.h"

#include "VisionView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExternalFrameDlg dialog
CExternalFrameDlg::CExternalFrameDlg()
	: CDialog(CExternalFrameDlg::IDD, NULL)
{
	Init();
}
//////////////////////////////////////////////////////////////////////////
CExternalFrameDlg::CExternalFrameDlg(CWnd* pParent, CVisionView*pView, int nMonitor)
	: CDialog(CExternalFrameDlg::IDD, pParent)
{
	Init();
	//{{AFX_DATA_INIT(CExternalFrameDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pVisionView   = pView;
	m_dwMonitorFlag = 1 << nMonitor;
	m_nMonitor      = nMonitor;
}
//////////////////////////////////////////////////////////////////////////
void CExternalFrameDlg::Init()
{
	m_pVisionView   = NULL;
	m_dwMonitorFlag = 0;
	m_nMonitor      = 0;
	m_iXWin         = 1; // must never be 0, is use as divisor!
	m_nSmallWindows = 0;
}
//////////////////////////////////////////////////////////////////////////
void CExternalFrameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CExternalFrameDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CExternalFrameDlg, CDialog)
	//{{AFX_MSG_MAP(CExternalFrameDlg)
	ON_WM_SIZE()
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_WM_NCHITTEST()
	ON_WM_SYSCOMMAND()
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExternalFrameDlg message handlers
void CExternalFrameDlg::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	if (   m_pVisionView != NULL 
		 && cx != 0 
		 && cy != 0)
	{
		CRect rc;
		GetClientRect(&rc);
		if (rc.Width() != cx && rc.Height() != cy)
		{
			CSmallWindows swa;
			::EnumChildWindows(m_hWnd, EnumSmallWindows, (LPARAM)&swa);
			m_pVisionView->SetSmallWindowPositionsXxX(swa, this);
		}
	}
}
//////////////////////////////////////////////////////////////////////////
bool CExternalFrameDlg::AddSmallWindow(CSmallWindow * pWnd)
{
	if (::GetParent(pWnd->m_hWnd) != m_hWnd)
	{
		pWnd->ShowWindow(SW_HIDE);
		::SetParent(pWnd->m_hWnd, m_hWnd);
		m_nSmallWindows++;
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
bool CExternalFrameDlg::RemoveSmallWindow(CSmallWindow *pWnd)
{
	if (::GetParent(pWnd->m_hWnd) == m_hWnd)
	{
		pWnd->ShowWindow(SW_HIDE);
		::SetParent(pWnd->m_hWnd, m_pVisionView->m_hWnd);
		m_nSmallWindows--;
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CExternalFrameDlg::RemoveSmallWindows(HWND hwnd, LPARAM lParam)
{
	CWnd*pWnd = CWnd::FromHandle(hwnd);
	CExternalFrameDlg *pThis = (CExternalFrameDlg*)lParam;
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CSmallWindow)))
	{
		pWnd->SetParent(pThis->m_pVisionView);
	}
	return 1;
}
//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CExternalFrameDlg::EnumSmallWindows(HWND hwnd, LPARAM lParam)
{
	CWnd*pWnd = CWnd::FromHandle(hwnd);
	CSmallWindows *pswa= (CSmallWindows*)lParam;
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CSmallWindow)))
	{
		pswa->Add((CSmallWindow*)pWnd);
	}
	return 1;
}
//////////////////////////////////////////////////////////////////////////
void CExternalFrameDlg::OnOK() 
{

}
//////////////////////////////////////////////////////////////////////////
void CExternalFrameDlg::OnCancel() 
{

}
//////////////////////////////////////////////////////////////////////////
BOOL CExternalFrameDlg::OnInitDialog() 
{
	m_Backgroud.Attach(GetSysColorBrush(COLOR_WINDOW));
	CDialog::OnInitDialog();
	
	CString str, strFormat;
	GetWindowText(strFormat);
	str.Format(strFormat, m_nMonitor+1);
	SetWindowText(str);
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME_IST));
	if (hIcon)
	{
		SetIcon(hIcon, FALSE);
		SetIcon(hIcon, TRUE );
	}
	
	SetWindowSize();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
//////////////////////////////////////////////////////////////////////////
void CExternalFrameDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		SetWindowSize();
	}
}
//////////////////////////////////////////////////////////////////////////
void CExternalFrameDlg::OnViewFullscreen(bool bFullScreen)
{
	if (bFullScreen)
	{
		ModifyStyle(WS_BORDER, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_DRAWFRAME);
	}
	else
	{
		ModifyStyle(0, WS_BORDER, SWP_NOSIZE|SWP_NOMOVE|SWP_DRAWFRAME);
	}	
	CSmallWindows swa;
	::EnumChildWindows(m_hWnd, EnumSmallWindows, (LPARAM)&swa);
	m_pVisionView->SetSmallWindowPositionsXxX(swa, this);
}
//////////////////////////////////////////////////////////////////////////
BOOL CExternalFrameDlg::PreTranslateMessage(MSG* pMsg) 
{
	switch (pMsg->message)
	{
		case WM_KEYDOWN:
			GetParent()->PostMessage(pMsg->message, pMsg->wParam, pMsg->lParam);
		break;
		case WM_NCLBUTTONDBLCLK:
			return TRUE;
			break;
	}
	return CDialog::PreTranslateMessage(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void CExternalFrameDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch (nID)
	{
		case SC_CLOSE:
		if (lParam == 0)
		{
			GetParent()->PostMessage(WM_SYSCOMMAND, nID, lParam);
		}break;
						 // OnNCClick
		case 0xf012: // Move Caption
		case 0xf001: // Size left
		case 0xf002: // Size right
		case 0xf003: // Size top
		case 0xf006: // Size bottom
			return;
		case SC_MINIMIZE:
		case SC_MOVE: case SC_SIZE:
			return;
		case SC_RESTORE:
			nID = SC_MAXIMIZE;
			break;
	}
	CDialog::OnSysCommand(nID, lParam);
}
//////////////////////////////////////////////////////////////////////////
void CExternalFrameDlg::OnClose() 
{
	CWnd *pParent = GetParent();

	::EnumChildWindows(m_hWnd, RemoveSmallWindows, (LPARAM)this);
	DestroyWindow();

	pParent->SendMessage(WM_USER, WM_CLOSE, (LPARAM)this);

	CDialog::OnClose();
	
	delete this;
}
//////////////////////////////////////////////////////////////////////////
void CExternalFrameDlg::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////////
UINT CExternalFrameDlg::OnNcHitTest(CPoint point) 
{
	return CDialog::OnNcHitTest(point);
}
//////////////////////////////////////////////////////////////////////////
void CExternalFrameDlg::SetWindowSize()
{
	CRect rc;
	GetParent()->SendMessage(WM_USER, MAKELONG(GET_MONITOR_RECT, m_nMonitor), (LPARAM)&rc);
	rc.InflateRect(1,1,1,1);
	SetWindowPos(&CWnd::wndTop, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOOWNERZORDER);
}
//////////////////////////////////////////////////////////////////////////
BOOL CExternalFrameDlg::OnEraseBkgnd(CDC* pDC) 
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&rc, &m_Backgroud);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
void CExternalFrameDlg::OnPaint() 
{
	CPaintDC dc(this);
#ifdef _DEBUG
//	CString str;
//	str.Format(_T("Monitor %d"), m_nMonitor+1);
//	dc.TextOut(100, 100, str, str.GetLength());
#endif
}
//////////////////////////////////////////////////////////////////////////
LRESULT CExternalFrameDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (WM_IDLEUPDATECMDUI == message)
	{
		CSmallWindows swa;
		int i, nCount = GetSmallWindows(m_hWnd, swa);
		for (i=0; i<nCount; i++)
		{
			swa.GetAt(i)->SendMessageToDescendants(message,
				(WPARAM)wParam, lParam, TRUE, TRUE);
		}
	}
	
	return CDialog::WindowProc(message, wParam, lParam);
}

int CExternalFrameDlg::GetSmallWindows(HWND hWnd, CSmallWindows &swa)
{
	swa.RemoveAll();
	::EnumChildWindows(hWnd, EnumSmallWindows, (LPARAM)&swa);
	return swa.GetSize();
}
