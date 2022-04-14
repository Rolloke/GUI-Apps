// SmallWindow.cpp : implementation file
//

#include "stdafx.h"
#include "Vision.h"

#include "Mainfrm.h"
#include "VisionDoc.h"
#include "VisionView.h"

#include "SmallWindow.h"
#include "ExternalFrameDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SW_COLOR_NORMAL 128
#define SW_COLOR_HIGH   192

static COLORREF s_crTitleColors[] = 
{	
										RGB(0,0,SW_COLOR_NORMAL),	// blau
										RGB(0,SW_COLOR_NORMAL,0),	// dgruen
										RGB(SW_COLOR_NORMAL,0,0), // lila
										RGB(0,SW_COLOR_NORMAL,SW_COLOR_NORMAL), // dorange
										RGB(SW_COLOR_NORMAL,0,SW_COLOR_NORMAL),	// drot
										RGB(SW_COLOR_NORMAL,SW_COLOR_NORMAL,0),	// dgruen
};
static COLORREF s_crActiveTitleColors[] = 
{	
										RGB(0,0,SW_COLOR_HIGH),	// blau
										RGB(0,SW_COLOR_HIGH,0),	// dgruen
										RGB(SW_COLOR_HIGH,0,0), // lila
										RGB(0,SW_COLOR_HIGH,SW_COLOR_HIGH), // dorange
										RGB(SW_COLOR_HIGH,0,SW_COLOR_HIGH),	// drot
										RGB(SW_COLOR_HIGH,SW_COLOR_HIGH,0),	// dgruen
									};
static int		s_iNrOfTitleColors = sizeof(s_crTitleColors) / sizeof(s_crTitleColors[0]);


IMPLEMENT_DYNCREATE(CSmallWindow, CWnd)

/////////////////////////////////////////////////////////////////////////////
// CSmallWindow

CSmallWindow::CSmallWindow(CServer* pServer)
{
	m_pServer = pServer;

	if (m_pServer)
	{
		m_wServerID = pServer->GetID();
		m_wHostID   = pServer->GetHostID().GetSubID();
// gf und schon haben wir zweimal den Host 0000 - und wie sortieren wir dann? WECH!
// siehe operator <
//		if (m_wHostID == 0xfffe) m_wHostID = 0;
	}
	else
	{
		m_wServerID = 0;
		m_wHostID   = 0;
	}
	m_ToolBarSize = CSize(0, 16);
	m_bIsActive = FALSE;
	m_iImage = 0;
	
	m_bLeftClick    = FALSE;
//	m_pointAtLeftClick
	m_bLeftDblClick = FALSE;
	m_uTimerID      = 0;
	m_dwMonitor     = 0;
}

CSmallWindow::~CSmallWindow()
{
}


BEGIN_MESSAGE_MAP(CSmallWindow, CWnd)
	//{{AFX_MSG_MAP(CSmallWindow)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_VIDEO_FULLSCREEN, OnVideoFullscreen)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_FULLSCREEN, OnUpdateVideoFullscreen)
	ON_COMMAND(ID_SMALL_CLOSE, OnSmallClose)
	ON_UPDATE_COMMAND_UI(ID_SMALL_CLOSE, OnUpdateSmallClose)
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_SHOWON_MONITOR_EXCLUSIVE, OnShowonMonitorExclusive)
	ON_UPDATE_COMMAND_UI(ID_SHOWON_MONITOR_EXCLUSIVE, OnUpdateShowonMonitorExclusive)
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SHOWON_REGARDLESS, ID_SHOWON_MONITOR2, OnUpdateShowOnMonitor)
	ON_COMMAND_RANGE(ID_SHOWON_REGARDLESS, ID_SHOWON_MONITOR2, OnShowOnMonitor)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
CSecID CSmallWindow::GetID()
{
	ASSERT(FALSE);
	TRACE(_T("Call only from Derived Classes\n"));
	return CSecID();
};

BOOL CSmallWindow::IsRequesting()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::operator < (CSmallWindow & second)
{
	// The one-and-only sort function for Window sorting
	// Sort local windows (HostID== 0xfffe) always as first window
//	if (GetServerID()<second.GetServerID())

	WORD wHostID1 = GetHostID();
	WORD wHostID2 = second.GetHostID();
	if (wHostID1 != wHostID2)
	{
		if (wHostID1 == 0xfffe)
		{
			return TRUE;
		}
		else if (wHostID2 == 0xfffe)
		{
			return FALSE;
		}
		else if (wHostID1 < wHostID2)
		{
			return TRUE;
		}
		else // if (wHostID1 > wHostID2)
		{
			return FALSE;
		}
	}
	else // bei gleicher HostID
	{
		if (GetID().IsOutputID() && second.GetID().IsInputID())
		{
			return TRUE;
		}
		else if (GetID().IsInputID() && second.GetID().IsOutputID())
		{
			return FALSE;
		}
		else
		{
			if (GetID().GetID()<second.GetID().GetID())
			{
				return TRUE;
			}
			else
			{
				return FALSE;
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::IsDisplayWindow()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::IsHtmlWindow()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::IsTextWindow()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::IsReferenceWindow()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CString CSmallWindow::GetTitleText(CDC* pDC)
{
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::DrawTitle(CDC* pDC)
{
	if (m_ToolBarSize.cy == 0) return;
	CRect rect;
	CBrush brush;
	COLORREF crTitle = RGB(64,64,64);
	COLORREF rgbTitle  = RGB(255,255,255);

//	GetClientRect(rect);
//	rect.bottom = max(2*VW_TEXTOFF + m_pVisionView->m_iFontHeight, m_ToolBarSize.cy);
	GetTitleRect(&rect);
	rect.right -= m_ToolBarSize.cx;

	if (GetServerID()!=0)
	{
		if (IsActive())
		{
			crTitle = s_crActiveTitleColors[(GetServerID()-1)%s_iNrOfTitleColors];
		}
		else
		{
			crTitle = s_crTitleColors[(GetServerID()-1)%s_iNrOfTitleColors];
		}

		if (IsRequesting())
		{
			rgbTitle = GetSysColor(COLOR_CAPTIONTEXT);
		}
		else
		{
			rgbTitle = GetSysColor(COLOR_INACTIVECAPTIONTEXT);
		}
	}
	else
	{
		// ein Kartenfenster
		if (IsActive())
		{
			crTitle = RGB(0,0,0);
		}
	}

	brush.CreateSolidBrush(crTitle);
	pDC->FillRect(rect,&brush);
	brush.DeleteObject();

	CFont* pOldFont;
	int iOldBkMode;
	COLORREF rgbOld;
	CString sTitle;

	pOldFont = pDC->SelectObject(&m_pVisionView->m_TitleFont);
	iOldBkMode = pDC->SetBkMode(TRANSPARENT);


	sTitle = GetTitleText(pDC);
	rgbOld = pDC->SetTextColor(rgbTitle);
	if (m_pServer && m_pServer->IsAlarm())
	{
		pDC->TextOut(VW_TEXTOFF+_IMAGE_WIDTH+2,VW_TEXTOFF,sTitle);
		m_pVisionView->m_Images.Draw(pDC,m_iImage,CPoint(1,(rect.Height()-14)/2),ILD_TRANSPARENT);
		if (m_iImage==1)
		{
			m_iImage = 0;
		}
		else if (m_iImage==0)
		{
			m_iImage = 1;
		}
	}
	else
	{
		pDC->TextOut(VW_TEXTOFF,VW_TEXTOFF,sTitle);
	}
	pDC->SetTextColor(rgbOld);
	pDC->SetBkMode(iOldBkMode);
	pDC->SelectObject(pOldFont);
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::DrawTitleTransparent(CDC* pDC)
{
	CFont* pOldFont;
	int iOldBkMode;
	COLORREF rgbOld;
	COLORREF rgbTitle;
	CString sTitle;

	pOldFont = pDC->SelectObject(&m_pVisionView->m_TitleFont);
	iOldBkMode = pDC->SetBkMode(TRANSPARENT);
	if (IsActive() || IsRequesting())
	{
		rgbTitle = GetSysColor(COLOR_CAPTIONTEXT);
	}
	else
	{
		rgbTitle = GetSysColor(COLOR_INACTIVECAPTIONTEXT);
	}
	sTitle = GetTitleText(pDC);
	rgbOld = pDC->SetTextColor(RGB(0,0,0));
	pDC->TextOut(VW_TEXTOFF+1,VW_TEXTOFF+1,sTitle);
	pDC->SetTextColor(rgbTitle);
	pDC->TextOut(VW_TEXTOFF,VW_TEXTOFF,sTitle);

	pDC->SetTextColor(rgbOld);
	pDC->SetBkMode(iOldBkMode);
	pDC->SelectObject(pOldFont);
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::GetTitleRect(LPRECT lpRect)
{
	GetClientRect(lpRect);
	if (m_ToolBarSize.cy)
	{
		lpRect->bottom = max(2*VW_TEXTOFF + m_pVisionView->m_iFontHeight,m_ToolBarSize.cy);
	}
	else
	{
		lpRect->bottom = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::GetFrameRect(LPRECT lpRect)
{
	GetClientRect(lpRect);
	if (m_ToolBarSize.cy)
	{
		lpRect->top = max(2*VW_TEXTOFF + m_pVisionView->m_iFontHeight,m_ToolBarSize.cy);
	}
	else
	{
		lpRect->top = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::GetVideoClientRect(LPRECT lpRect, CSize picturesize, BOOL b1to1)
{
	GetFrameRect(lpRect);
	int w,h;
	CSize s = picturesize;
	if (s.cx > s.cy*2)
	{
		s.cy *= 2;
	}

	w = lpRect->right - lpRect->left;
	h = lpRect->bottom - lpRect->top;

	if (   b1to1 
		&& (   (s.cx<w && s.cy<h)
//			|| m_pVisionView->Is1x1()	// gf only take 1:1 if enough place, 
			)
		)
	{
		lpRect->left = lpRect->left + (w - s.cx)/2;
		lpRect->top = lpRect->top + (h - s.cy)/2;
		lpRect->right = lpRect->left + s.cx;
		lpRect->bottom = lpRect->top + s.cy;
	}
	else
	{
		int xoff = 0;
		int yoff = 0;
		if ( (w*s.cy) > (h*s.cx))
		{
			// zu breit
			yoff = 0;
			xoff =  (w-(s.cx*h)/s.cy)/2;
		}
		else if ( (w*s.cy) < (h*s.cx))
		{
			// zu hoch
			xoff = 0;
			yoff =  (h-(s.cy*w)/s.cx)/2;
		}
		else
		{
			// passt
			xoff = 0;
			yoff = 0;
		}

		lpRect->top += yoff;
		lpRect->bottom -= yoff;
		lpRect->left += xoff;
		lpRect->right -= xoff;
	}
} 
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::SetActive(BOOL bActive/*=TRUE*/)
{
	if (m_bIsActive != bActive)
	{
		m_bIsActive = bActive;
		{
			CRect rect;
			GetTitleRect(rect);
			InvalidateRect(rect,FALSE);
		}
//		TRACE(_T("%08lx is %s\n"),GetID().GetID(),m_bIsActive ? _T("active") : _T("not active"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnPaint() 
{
	if (IsWindowVisible())
	{
		CPaintDC dc(this); // device context for painting
		
		OnDraw(&dc);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnDraw(CDC* pDC)
{
	// implemented in derived classes
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnSize(UINT nType, int cx, int cy) 
{
	CWnd::OnSize(nType, cx, cy);

	if (WK_IS_WINDOW(&m_wndToolBar))
	{
		m_wndToolBar.MoveWindow(cx-m_ToolBarSize.cx,0,m_ToolBarSize.cx,m_ToolBarSize.cy);
		CRect rect;
		m_wndToolBar.GetClientRect(rect);
		m_wndToolBar.InvalidateRect(rect);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);

	// allow top level routing frame to handle the message
	if (GetRoutingFrame() != NULL)
		return FALSE;

	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	TCHAR szFullText[256];
	CString strTipText;
	UINT nID = pNMHDR->idFrom;
	if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
		pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
	{
		// idFrom is actually the HWND of the tool
		nID = ::GetDlgCtrlID((HWND)nID);
	}

	if (nID != 0) // will be zero on a separator
	{
		AfxLoadString(nID, szFullText);
			// this is the command id, not the button index
		AfxExtractSubString(strTipText, szFullText, 1, '\n');
	}
#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText, _countof(pTTTA->szText));
	else
		_mbstowcsz(pTTTW->szText, strTipText, _countof(pTTTW->szText));
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText, _countof(pTTTA->szText));
	else
		lstrcpyn(pTTTW->szText, strTipText, _countof(pTTTW->szText));
#endif
	*pResult = 0;

	// bring the tooltip window above other popup windows
	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
		SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

	return TRUE;    // message was handled
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_bLeftClick)
	{
		m_bLeftClick = TRUE;
		m_pointAtLeftClick = point;
		if (m_uTimerID == 0)
		{
			m_uTimerID = SetTimer(2,(GetDoubleClickTime()*11)/10,NULL);
		}
	}
	CWnd::OnLButtonDown(nFlags,point);
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	TRACE(_T("CSmallWindow::OnLButtonDblClk\n"));
	if (!theApp.m_bDisableDblClick || !IsFullScreen())
	{
		m_bLeftDblClick = TRUE;
	}
	CWnd::OnLButtonDblClk(nFlags,point);
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnTimer(UINT nIDEvent) 
{
//	TRACE(_T("CSmallWindow::OnTimer\n"));
	if (nIDEvent == m_uTimerID)
	{
		KillTimer(m_uTimerID);
		m_uTimerID = 0;
		if (m_bLeftDblClick)
		{
			// Einziger Aufruf, Kommentar siehe dort
			m_pVisionView->SetSmallWindowView1(this);
		}
		else if (m_bLeftClick)
		{
			m_pVisionView->SetCmdActiveSmallWindow(this);
			CRect rect;
			GetTitleRect(rect);
			if (rect.PtInRect(m_pointAtLeftClick))
			{
				m_pVisionView->SetBigSmallWindow(this);
			}
		}
		m_bLeftClick = FALSE;
		m_bLeftDblClick = FALSE;
	}
	
	CWnd::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::IsFullScreen()
{
	CMainFrame* pMF = (CMainFrame*)m_pVisionView->GetParentFrame();
	if (WK_IS_WINDOW(pMF))
	{
		return pMF->IsFullScreen();	
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnVideoFullscreen() 
{
	CMainFrame* pMF = (CMainFrame*)m_pVisionView->GetParentFrame();
	pMF->ChangeFullScreenModus();
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnUpdateVideoFullscreen(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(IsFullScreen());
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnSmallClose() 
{
	// attention delete's myself
	m_pVisionView->DeleteSmallWindow(m_wServerID,GetID());
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnUpdateSmallClose(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSmallWindow::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnRButtonDown(UINT nFlags, CPoint point) 
{
	m_pVisionView->SetCmdActiveSmallWindow(this);
	CWnd::OnRButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnUpdateShowOnMonitor(CCmdUI* pCmdUI)
{
	BOOL bOn = FALSE;
	switch (pCmdUI->m_nID)
	{
		case ID_SHOWON_MONITOR1:   if (m_dwMonitor & USE_MONITOR_1) bOn = TRUE; break;
		case ID_SHOWON_MONITOR2:   if (m_dwMonitor & USE_MONITOR_2) bOn = TRUE; break;
	}

	pCmdUI->Enable();
	pCmdUI->SetCheck(bOn);
}
/////////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnShowOnMonitor(UINT nID)
{
	switch (nID)
	{
		case ID_SHOWON_MONITOR1:
			if (m_dwMonitor & USE_MONITOR_1)
			{
				m_dwMonitor = 0;
			}
			else
			{
				m_dwMonitor = USE_MONITOR_1;
			}
			break;
		case ID_SHOWON_MONITOR2:
			if (m_dwMonitor & USE_MONITOR_2)
			{
				m_dwMonitor = 0;
			}
			else
			{
				m_dwMonitor = USE_MONITOR_2;
			}
			break;
	}
//	if (m_dwMonitor)
	{
		m_pVisionView->PostMessage(WM_COMMAND, ID_VIEW_MONITOR16, 0);
	}
}
void CSmallWindow::OnUpdateShowonMonitorExclusive(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((m_dwMonitor & USE_MONITOR_ALL) != 0? TRUE : FALSE);
	pCmdUI->SetCheck((m_dwMonitor & USE_MONITOR_EXCLUSIVE) != 0 ? TRUE : FALSE);
}
//////////////////////////////////////////////////////////////////////////
void CSmallWindow::OnShowonMonitorExclusive() 
{
	if (m_dwMonitor & USE_MONITOR_EXCLUSIVE)
	{
		m_dwMonitor &= ~USE_MONITOR_EXCLUSIVE;
	}
	else
	{
		CSmallWindow *pSW = m_pVisionView->GetExclusiveSmallWindow();
		if (pSW)
		{
			pSW->OnShowonMonitorExclusive();
		}
		m_dwMonitor |=  USE_MONITOR_EXCLUSIVE;
	}
	m_pVisionView->PostMessage(WM_COMMAND, ID_VIEW_MONITOR16, 0);
}
/////////////////////////////////////////////////////////////////////////////
DWORD CSmallWindow::GetMonitorFlag()
{
	return m_dwMonitor;
}
/////////////////////////////////////////////////////////////////////////////
int CSmallWindow::GetMonitor()
{
	CMainFrame *pFrame = (CMainFrame*)GetParentFrame();
	HWND hwndParent = ::GetParent(m_hWnd);
	if (hwndParent == m_pVisionView->m_hWnd)	
	{
		return 0;
	}
	else
	{
		CExternalFrameDlg *pExternalFrame;
		int i, nCount = pFrame->GetNoOfExternalFrames();
		for (i=0; i<nCount; i++)
		{
			pExternalFrame = pFrame->GetExternalFrame(i);
			if (hwndParent == pExternalFrame->m_hWnd)
			{
				return pExternalFrame->GetMonitor();
			}
		}
	}
	return 0;
}

void CSmallWindow::EnableTitle(BOOL bEnable)
{
	if (bEnable)
	{
		m_ToolBarSize.cy = 20;
	}
	else
	{
		m_ToolBarSize.cy = 0;
	}

	ASSERT(WK_IS_WINDOW(&m_wndToolBar));
	ASSERT(GetParent() != NULL);

	m_wndToolBar.ShowWindow(bEnable ? SW_SHOW : SW_HIDE);
	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);
	rc.InflateRect(1,1);
	MoveWindow(&rc, FALSE);
	rc.DeflateRect(1,1);
	MoveWindow(&rc);
}
/////////////////////////////////////////////////////////////////////////////
