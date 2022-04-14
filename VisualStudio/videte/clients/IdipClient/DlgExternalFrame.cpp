// DlgExternalFrame.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgExternalFrame.h"

#include "ViewIdipClient.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CDlgExternalFrame, CSkinDialog)
/////////////////////////////////////////////////////////////////////////////
// CDlgExternalFrame dialog
CDlgExternalFrame::CDlgExternalFrame()
	: CSkinDialog(CDlgExternalFrame::IDD, NULL)
{
	Init();
}
//////////////////////////////////////////////////////////////////////////
CDlgExternalFrame::CDlgExternalFrame(CWnd* pParent, CViewIdipClient*pView, int nMonitor)
	: CSkinDialog(CDlgExternalFrame::IDD, pParent)
{
	Init();
	//{{AFX_DATA_INIT(CDlgExternalFrame)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pViewIdipClient   = pView;
	m_dwMonitorFlag = 1 << nMonitor;
	m_nMonitor      = nMonitor;
}
//////////////////////////////////////////////////////////////////////////
void CDlgExternalFrame::Init()
{
	m_nInitToolTips = FALSE;
	MoveOnClientAreaClick(false);

	m_pViewIdipClient   = NULL;
	m_dwMonitorFlag = 0;
	m_nMonitor      = 0;
	m_iXWin         = 1; // must never be 0, is use as divisor!
	m_nSmallWindows = 0;
}
//////////////////////////////////////////////////////////////////////////
void CDlgExternalFrame::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgExternalFrame)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}
//////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgExternalFrame, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgExternalFrame)
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
// CDlgExternalFrame message handlers
void CDlgExternalFrame::OnSize(UINT nType, int cx, int cy) 
{
	CSkinDialog::OnSize(nType, cx, cy);

	if (   m_pViewIdipClient != NULL 
		 && cx != 0 
		 && cy != 0)
	{
		CRect rc;
		GetClientRect(&rc);
		if (rc.Width() != cx && rc.Height() != cy)
		{
			m_pViewIdipClient->LockSmallWindows(_T(__FUNCTION__));
			CWndSmallArray swa;
			::EnumChildWindows(m_hWnd, EnumSmallWindows, (LPARAM)&swa);
			m_pViewIdipClient->SetSmallWindowPositionsXxX(swa, this);
			m_pViewIdipClient->UnlockSmallWindows();
		}
	}
}
//////////////////////////////////////////////////////////////////////////
bool CDlgExternalFrame::AddWndSmall(CWndSmall * pWnd)
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
bool CDlgExternalFrame::RemoveWndSmall(CWndSmall *pWnd)
{
	if (::GetParent(pWnd->m_hWnd) == m_hWnd)
	{
		pWnd->ShowWindow(SW_HIDE);
		::SetParent(pWnd->m_hWnd, m_pViewIdipClient->m_hWnd);
		m_nSmallWindows--;
		return true;
	}
	return false;
}
//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CDlgExternalFrame::RemoveSmallWindows(HWND hwnd, LPARAM lParam)
{
	CWnd*pWnd = CWnd::FromHandle(hwnd);
	CDlgExternalFrame *pThis = (CDlgExternalFrame*)lParam;
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CWndSmall)))
	{
		pWnd->SetParent(pThis->m_pViewIdipClient);
	}
	return 1;
}
//////////////////////////////////////////////////////////////////////////
BOOL CALLBACK CDlgExternalFrame::EnumSmallWindows(HWND hwnd, LPARAM lParam)
{
	CWnd*pWnd = CWnd::FromHandle(hwnd);
	CWndSmallArray *pswa= (CWndSmallArray*)lParam;
	if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CWndSmall)))
	{
		pswa->Add((CWndSmall*)pWnd);
	}
	return 1;
}
//////////////////////////////////////////////////////////////////////////
//BOOL CALLBACK CDlgExternalFrame::SendMessageToDescendants(HWND hwnd, LPARAM lParam)
//{
//	MSG *pmsg = (MSG*)lParam;
//	::SendMessage(hwnd, pmsg->message, pmsg->wParam, pmsg->lParam);
//	return 1;
//}
//////////////////////////////////////////////////////////////////////////
void CDlgExternalFrame::OnOK() 
{

}
//////////////////////////////////////////////////////////////////////////
void CDlgExternalFrame::OnCancel() 
{

}
//////////////////////////////////////////////////////////////////////////
BOOL CDlgExternalFrame::OnInitDialog() 
{
	m_Background.CreateSolidBrush(afxData.clrBtnHilite);
	CSkinDialog::OnInitDialog();
	
	CString str, strFormat;
	GetWindowText(strFormat);
	str.Format(strFormat, m_nMonitor+1);
	SetWindowText(str);
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
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
void CDlgExternalFrame::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CSkinDialog::OnShowWindow(bShow, nStatus);
	if (bShow)
	{
		SetWindowSize();
	}
}
//////////////////////////////////////////////////////////////////////////
void CDlgExternalFrame::OnViewFullscreen(bool bFullScreen)
{
	if (bFullScreen)
	{
		ModifyStyle(WS_BORDER, 0, SWP_NOSIZE|SWP_NOMOVE|SWP_DRAWFRAME);
	}
	else
	{
		ModifyStyle(0, WS_BORDER, SWP_NOSIZE|SWP_NOMOVE|SWP_DRAWFRAME);
	}	
	m_pViewIdipClient->LockSmallWindows(_T(__FUNCTION__));
	CWndSmallArray swa;
	::EnumChildWindows(m_hWnd, EnumSmallWindows, (LPARAM)&swa);
	m_pViewIdipClient->SetSmallWindowPositionsXxX(swa, this);
	m_pViewIdipClient->UnlockSmallWindows();
}
//////////////////////////////////////////////////////////////////////////
BOOL CDlgExternalFrame::PreTranslateMessage(MSG* pMsg) 
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
	return CSkinDialog::PreTranslateMessage(pMsg);
}
//////////////////////////////////////////////////////////////////////////
void CDlgExternalFrame::OnSysCommand(UINT nID, LPARAM lParam)
{
	switch (nID)
	{
		case SC_CLOSE:
		if (lParam == 0)
		{
			GetParent()->PostMessage(WM_SYSCOMMAND, nID, lParam);
		}break;
#ifndef _DEBUG						 // OnNCClick
		case 0xf012: // Move Caption
		case 0xf001: // Size left
		case 0xf002: // Size right
		case 0xf003: // Size top
		case 0xf006: // Size bottom
			return;
#endif
		case SC_MINIMIZE:
		case SC_MOVE: case SC_SIZE:
			return;
		case SC_RESTORE:
			nID = SC_MAXIMIZE;
			break;
	}
	CSkinDialog::OnSysCommand(nID, lParam);
}
//////////////////////////////////////////////////////////////////////////
void CDlgExternalFrame::OnClose() 
{
	CWnd *pParent = GetParent();

	::EnumChildWindows(m_hWnd, RemoveSmallWindows, (LPARAM)this);
	DestroyWindow();

	pParent->SendMessage(WM_USER, WM_CLOSE, (LPARAM)this);

	CSkinDialog::OnClose();
	
	delete this;
}
//////////////////////////////////////////////////////////////////////////
void CDlgExternalFrame::OnTimer(UINT nIDEvent) 
{
	CSkinDialog::OnTimer(nIDEvent);
}
//////////////////////////////////////////////////////////////////////////
UINT CDlgExternalFrame::OnNcHitTest(CPoint point) 
{
	return CSkinDialog::OnNcHitTest(point);
}
//////////////////////////////////////////////////////////////////////////
void CDlgExternalFrame::SetWindowSize()
{
	CRect rc;
	GetParent()->SendMessage(WM_USER, MAKELONG(GET_MONITOR_RECT, m_nMonitor), (LPARAM)&rc);
	rc.InflateRect(1,1,1,1);
	SetWindowPos(&CWnd::wndTop, rc.left, rc.top, rc.Width(), rc.Height(), SWP_NOOWNERZORDER);
}
//////////////////////////////////////////////////////////////////////////
BOOL CDlgExternalFrame::OnEraseBkgnd(CDC* pDC) 
{
	CRect rc;
	GetClientRect(&rc);
	pDC->FillRect(&rc, &m_Background);
	return TRUE;
}
//////////////////////////////////////////////////////////////////////////
void CDlgExternalFrame::OnPaint() 
{
	CPaintDC dc(this);
#ifdef _DEBUG
	CString str;
	str.Format(_T("Monitor %d"), m_nMonitor+1);
	dc.TextOut(100, 100, str, str.GetLength());
#endif
}
//////////////////////////////////////////////////////////////////////////
LRESULT CDlgExternalFrame::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (WM_IDLEUPDATECMDUI == message)
	{
		SendMessageToDescendants(message,(WPARAM)wParam, lParam, TRUE, TRUE);
	}
	
	return CSkinDialog::WindowProc(message, wParam, lParam);
}

int CDlgExternalFrame::GetSmallWindows(HWND hWnd, CWndSmallArray &swa)
{
	swa.RemoveAll();
	::EnumChildWindows(hWnd, EnumSmallWindows, (LPARAM)&swa);
	return swa.GetSize();
}
