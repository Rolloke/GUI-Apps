// WndSmall.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"

#include "Mainfrm.h"
#include "IdipClientDoc.h"
#include "ViewIdipClient.h"

#include "WndSmall.h"
#include "DlgExternalFrame.h"
#include "wnddib.h"
#include "wndtext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SW_COLOR_LOW	 96
#define SW_COLOR_NORMAL 128
#define SW_COLOR_HIGH   192
#define SW_COLOR_BRIGHT 255

static COLORREF s_crTitleColors[] = 
{	
										RGB(0,0,SW_COLOR_NORMAL),	// blue
										RGB(0,SW_COLOR_NORMAL,0),	// green
										RGB(SW_COLOR_NORMAL,0,0),	// red
										RGB(0,SW_COLOR_NORMAL,SW_COLOR_NORMAL), // cyan
										RGB(SW_COLOR_NORMAL,0,SW_COLOR_NORMAL),	// magenta
										RGB(SW_COLOR_NORMAL,SW_COLOR_NORMAL,0),	// ocker
										RGB(SW_COLOR_HIGH,SW_COLOR_LOW,0),	// orange
										RGB(SW_COLOR_HIGH,0,SW_COLOR_LOW),	// magenta
};
static COLORREF s_crActiveTitleColors[] = 
{	
										RGB(0,0,SW_COLOR_HIGH),	// blue
										RGB(0,SW_COLOR_HIGH,0),	// green
										RGB(SW_COLOR_HIGH,0,0), // red
										RGB(0,SW_COLOR_HIGH,SW_COLOR_HIGH), // cyan
										RGB(SW_COLOR_HIGH,0,SW_COLOR_HIGH),	// magenta
										RGB(SW_COLOR_HIGH,SW_COLOR_HIGH,0),	// ocker
										RGB(SW_COLOR_BRIGHT,SW_COLOR_NORMAL,0),	// orange
										RGB(SW_COLOR_BRIGHT,0,SW_COLOR_NORMAL),	// magenta
									};
static int		s_iNrOfTitleColors = sizeof(s_crTitleColors) / sizeof(s_crTitleColors[0]);


IMPLEMENT_DYNAMIC(CWndSmall, CWnd)
/////////////////////////////////////////////////////////////////////////////
// CWndSmall

CWndSmall::CWndSmall(CServer* pServer)
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

	m_iImage = 0;
	
	m_bLeftClick    = false;
//	m_pointAtLeftClick
	m_bLeftDblClick = false;
	m_uTimerID      = 0;
	m_dwMonitor     = 0;
	m_b1to1			= false;
	m_bUpdateTitle		= false;
	m_nType = WST_ALL;
}

CWndSmall::~CWndSmall()
{
	if (!theApp.IsInMainThread())
	{
		WK_TRACE_ERROR(_T("~CWndSmall() not called in MainThread"));
		CUnhandledException::TraceCallStack(NULL);
	}
}


BEGIN_MESSAGE_MAP(CWndSmall, CWnd)
	//{{AFX_MSG_MAP(CWndSmall)
	ON_WM_PAINT()
	ON_WM_SIZE()
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(ID_VIDEO_FULLSCREEN, OnVideoFullscreen)
	ON_UPDATE_COMMAND_UI(ID_VIDEO_FULLSCREEN, OnUpdateVideoFullscreen)
	ON_COMMAND(ID_CLOSE_WND_THIS, OnSmallClose)
	ON_UPDATE_COMMAND_UI(ID_CLOSE_WND_THIS, OnUpdateSmallClose)
	ON_WM_ERASEBKGND()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	ON_WM_TIMER()
	ON_COMMAND(ID_SHOWON_MONITOR_EXCLUSIVE, OnShowonMonitorExclusive)
	ON_UPDATE_COMMAND_UI(ID_SHOWON_MONITOR_EXCLUSIVE, OnUpdateShowonMonitorExclusive)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SHOWON_REGARDLESS, ID_SHOWON_MONITOR2, OnUpdateShowOnMonitor)
	ON_COMMAND_RANGE(ID_SHOWON_REGARDLESS, ID_SHOWON_MONITOR2, OnShowOnMonitor)
	ON_WM_CREATE()
	ON_UPDATE_COMMAND_UI(ID_EDIT_COMMENT, OnUpdateEditComment)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
CSecID CWndSmall::GetID()
{
	//This function was checked against Recherche by TKR, 13.04.2004
	ASSERT(FALSE);
	TRACE(_T("Call only from Derived Classes\n"));
	return CSecID();
};
/////////////////////////////////////////////////////////////////////////////
BOOL CWndSmall::IsRequesting()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndSmall::CanPrint()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndSmall::operator < (CWndSmall & second)
{
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
			// now check for Live, Play, Dib windows
			CString sRuntimeClass1 = GetRuntimeClass()->m_lpszClassName;
			CString sRuntimeClass2 = second.GetRuntimeClass()->m_lpszClassName;
			DWORD dwID1 = GetID().GetID();
			DWORD dwID2 = second.GetID().GetID();
			if ( sRuntimeClass1 == sRuntimeClass2)
			{
				// Play und Dib window sort by name
				if (   IsWndPlay()
					|| GetType() == WST_DIB)
				{
					ASSERT_KINDOF(CWndImageRecherche, this);
					return GetName() > second.GetName();
				}
				// other windows sort by ID
				else
				{
					return (dwID1 < dwID2);
				}
			}
			else // different type of window
			{
				if (IsWndPlay())
				{
					if (second.GetType() == WST_DIB)
					{
						return TRUE;
					}
					else
					{
						return FALSE;
					}
				}
				else if (GetType() == WST_DIB)
				{
					return FALSE;
				}
				else if (second.IsWndPlay())
				{
					if (GetType() == WST_DIB)
					{
						return FALSE;
					}
					else
					{
						return TRUE;
					}
				}
				else if (second.GetType() == WST_DIB)
				{
					return TRUE;
				}
				else
				{
					return (dwID1 < dwID2);
				}
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndSmall::IsWndLive()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndSmall::IsWndPlay()
{
	//This function was checked against Recherche by TKR, 13.04.2004
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CSystemTime CWndSmall::GetTimeStamp()
{
	//This function was implemented by GF, 26.05.2004
	return m_stTimeStamp;
}
/////////////////////////////////////////////////////////////////////////////
CString CWndSmall::GetTitleText(CDC* pDC)
{
	//This function was checked against Recherche by TKR, 13.04.2004
	return _T("");
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::DrawTitle(CDC* pDC)
{
	if (m_wndToolBar.GetSize().cy > 0)
	{
		COLORREF crTitleBkGnd = RGB(64,64,64);
		COLORREF crTitleText  = RGB(255,255,255);

		if (GetServerID()!=0)
		{
			if (IsCmdActive())
			{
//				crTitleBkGnd = s_crActiveTitleColors[(GetServerID()-1)%s_iNrOfTitleColors];
				crTitleBkGnd = s_crActiveTitleColors[(GetServerID()-1)&0x00000007];
				crTitleText =  SKIN_COLOR_WHITE;
			}
			else
			{
//				crTitleBkGnd = s_crTitleColors[(GetServerID()-1)%s_iNrOfTitleColors];
				crTitleBkGnd = s_crTitleColors[(GetServerID()-1)&0x00000007];
				crTitleText = SKIN_COLOR_GREY_LIGHT;
			}
			// RKE: IsRequesting was used before
		}
		else
		{
			// ein Kartenfenster
			if (IsCmdActive())
			{
				crTitleBkGnd = RGB(0,0,0);
				crTitleText =  SKIN_COLOR_GREY_LIGHT;
			}
		}

		RedrawTitleToolbar(pDC,crTitleBkGnd, crTitleText);
	}
	else if (m_wndToolBar.GetSize().cy < 0 && GetServerID()!=0)
	{
		//TODO! TKR (nach der Ifsec) Titel innerhalb des Bildes zeichnen, nicht darüber
		//draw title within picture
		CRect rect;
		GetFrameRect(&rect);
		CString str = GetTitleText(pDC);
		COLORREF crTitleText = RGB(255,255,255);

		if (IsCmdActive())
		{
			crTitleText = s_crActiveTitleColors[(GetServerID()-1)%s_iNrOfTitleColors];
		}
		else
		{
			crTitleText = s_crTitleColors[(GetServerID()-1)%s_iNrOfTitleColors];
		}
		pDC->SetTextColor(crTitleText);
		rect.left = VW_TEXTOFF;
		DrawTitleTransparent(pDC, &rect, str);
	}
	m_bUpdateTitle = false;
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::DrawTitleTransparent(CDC* pDC, CRect*pRect, CString &sTitle)
{
	BOOL bAlarm = m_pServer && m_pServer->IsAlarm();

	pDC->SaveDC();
	pDC->SelectObject(&m_pViewIdipClient->m_TitleFont);
	pDC->SetBkMode(TRANSPARENT);

	if (bAlarm)
	{
		pRect->left += _IMAGE_WIDTH+2;
	}
//	pRect->top  = VW_TEXTOFF;

	pDC->DrawText(sTitle, pRect, DT_LEFT|DT_TOP|DT_SINGLELINE);
	if (bAlarm)
	{
		m_pViewIdipClient->m_Images.Draw(pDC,m_iImage, CPoint(1, (pRect->Height()-14)/2), ILD_TRANSPARENT);
		if (m_iImage==1)
		{
			m_iImage = 0;
		}
		else if (m_iImage==0)
		{
			m_iImage = 1;
		}
	}

	pDC->RestoreDC(-1);
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::GetTitleRect(LPRECT lpRect)
{
	//This function was checked against Recherche by TKR, 13.04.2004
	GetClientRect(lpRect);
	if (m_wndToolBar.GetSize().cy > 0)
	{
		lpRect->bottom = m_wndToolBar.GetSize().cy;
	}
	else
	{
		lpRect->bottom = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::GetFrameRect(LPRECT lpRect)
{
	GetClientRect(lpRect);
	if (m_wndToolBar.GetSize().cy > 0)
	{
		lpRect->top = m_wndToolBar.GetSize().cy;
	}
	else
	{
		lpRect->top = 0;
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnSetCmdActive(BOOL bActive, CWndSmall* pWndSmall)
{
	m_bUpdateTitle = true;
	Invalidate();
	UpdateWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnSetSmallBig(BOOL bActive, CWndSmall* pWndSmall)
{
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnPaint() 
{
	//This function was checked against Recherche by TKR, 13.04.2004
	if (IsWindowVisible())
	{
		if (!theApp.GetMainFrame()->IsFullScreenChanging()) //from Recherche CSmallWindow::OnPaint() TKR
		{
			// only draw myself if Mainframe
			// doesn't change the fullscreen mode
			CPaintDC dc(this); // device context for painting
			OnDraw(&dc);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnDraw(CDC* pDC)
{
	//This function was checked against Recherche by TKR, 13.04.2004
	// implemented in derived classes
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnSize(UINT nType, int cx, int cy) 
{
	if (cx == 0 && cy == 0)
	{
		CRect rc;
		GetClientRect(&rc);
		cx = rc.Width();
		cy = rc.Height();
	}
	CWnd::OnSize(nType, cx, cy);

	if (m_wndToolBar.m_hWnd && m_wndToolBar.GetSize().cy > 0)
	{
		m_wndToolBar.MoveWindow(0, 0, cx, m_wndToolBar.GetSize().cy);
		int nIndent = cx - m_wndToolBar.GetSize().cx;
		if (nIndent < 0) nIndent = 0;
		m_wndToolBar.GetToolBarCtrl().SetIndent(nIndent);
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndSmall::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
{
	//This function was checked against Recherche by TKR, 14.04.2004
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

	if (CSkinDialog::UseGlobalFonts() && pNMHDR->hwndFrom)
	{
		::SendMessage(pNMHDR->hwndFrom, WM_SETFONT, (WPARAM)CSkinDialog::GetDialogItemGlobalFont()->GetSafeHandle(), 0);
	}
	return TRUE;    // message was handled
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnLButtonDown(UINT nFlags, CPoint point) 
{
	if (!m_bLeftClick)
	{
		m_bLeftClick = true;
		m_pointAtLeftClick = point;
		if (m_uTimerID == 0)
		{
			m_uTimerID = SetTimer(2, MulDiv(GetDoubleClickTime(), 11, 10),NULL);
		}
	}
	CWnd::OnLButtonDown(nFlags,point);
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//This function was checked against Recherche by TKR, 13.04.2004
	TRACE(_T("CWndSmall::OnLButtonDblClk\n"));
	if (!theApp.m_bDisableDblClick || !theApp.GetMainFrame()->IsFullScreen())
	{
		m_bLeftDblClick = true;
	}
	CWnd::OnLButtonDblClk(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnTimer(UINT nIDEvent) 
{
//	TRACE(_T("CWndSmall::OnTimer\n"));
	if (nIDEvent == m_uTimerID)
	{
		KillTimer(m_uTimerID);
		m_uTimerID = 0;
		if (m_bLeftDblClick)
		{
			// Einziger Aufruf, Kommentar siehe dort
			m_pViewIdipClient->SetWndSmallView1(this);
		}
		else if (m_bLeftClick)
		{
			CRect rect;
			GetTitleRect(rect);
			if (UseSetActiveArea())
			{
				rect.bottom += SET_ACTIVE_AREA_HEIGHT;
			}

			if (GetMonitor() > 0)
			{
				theApp.GetMainFrame()->SetActiveView(m_pViewIdipClient);
			}
			m_pViewIdipClient->SetWndSmallCmdActive(this);

			if (rect.PtInRect(m_pointAtLeftClick))
			{
				m_pViewIdipClient->SetWndSmallBig(this);
			}
			else if (m_pViewIdipClient->IsView1plus() == FALSE)
			{
				m_pViewIdipClient->SetWndSmallBig(this);
			}
		}
		m_bLeftClick = false;
		m_bLeftDblClick = false;
	}
	
	CWnd::OnTimer(nIDEvent);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndSmall::UseSetActiveArea()
{
	if (m_wndToolBar.GetSize().cy <= 0)
	{
		if (m_pViewIdipClient->IsView1plus())
		{
			return !m_pViewIdipClient->IsWndSmallBig(this);
		}
		else
		{
			return !m_pViewIdipClient->IsWndSmallCmdActive(this);
		}
	}
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnVideoFullscreen() 
{
	//This function was checked against Recherche by TKR, 14.04.2004
	CMainFrame* pMF = theApp.GetMainFrame();
	pMF->ChangeFullScreenModus();
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnUpdateVideoFullscreen(CCmdUI* pCmdUI) 
{
	//This function was checked against Recherche by TKR, 14.04.2004
	pCmdUI->SetCheck(theApp.GetMainFrame()->IsFullScreen());
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnSmallClose() 
{
	//This function was checked against Recherche by TKR, 14.04.2004
	// attention delete's myself
	if (m_pViewIdipClient)
	{
		if (theApp.m_bVerifyUserToCloseNote
			&& GetType() == WST_TEXT
			&& !((CWndText*)this)->IsDebuggerWindow())
		{
			CDlgLogin dlg(this, 0, DLGLOGIN_NO_LANGUAGE|DLGLOGIN_NO_NOTIFY_MSG);
			if (dlg.DoModal() != IDOK)
			{
				return;
			}
		}
		CMainFrame* pMF = (CMainFrame*)theApp.m_pMainWnd;
		if(IsWndPlay())
		{
			if (pMF && pMF->IsSyncMode())
			{
				//im Syncmode beim manuellen Schließen des Archivfensters auch
				//im Archivbaum entsprechende Kamera aushaken
				//in DeleteArchive wird dann DeleteWndSmall() aufgerufen
				pMF->GetSyncCoolBar()->DeleteArchive(m_wServerID,GetID().GetGroupID(),TRUE);
			}
			else
			{
				m_pViewIdipClient->DeleteWndSmall(m_wServerID, GetID());
			}
		}
		else
		{
			m_pViewIdipClient->DeleteWndSmall(m_wServerID, GetID());
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnUpdateSmallClose(CCmdUI* pCmdUI) 
{
	//This function was checked against Recherche by TKR, 14.04.2004
//	pCmdUI->Enable(TRUE); // Vision
	pCmdUI->Enable(!theApp.GetMainFrame()->IsFullScreen() || m_pViewIdipClient->m_SmallWindows.GetSize()>1); //Recherche
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndSmall::OnEraseBkgnd(CDC* pDC) 
{
	//This function was checked against Recherche by TKR, 13.04.2004
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnRButtonDown(UINT nFlags, CPoint point) 
{
	//This function was checked against Recherche by TKR, 13.04.2004
	m_pViewIdipClient->SetWndSmallCmdActive(this);
	CWnd::OnRButtonDown(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnUpdateShowOnMonitor(CCmdUI* pCmdUI)
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
void CWndSmall::OnShowOnMonitor(UINT nID)
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
		m_pViewIdipClient->PostMessage(WM_COMMAND, ID_VIEW_MONITOR16, 0);
	}
}
void CWndSmall::OnUpdateShowonMonitorExclusive(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(  m_dwMonitor & USE_MONITOR_ALL       ? TRUE : FALSE);
	pCmdUI->SetCheck(m_dwMonitor & USE_MONITOR_EXCLUSIVE ? TRUE : FALSE);
}
//////////////////////////////////////////////////////////////////////////
void CWndSmall::OnShowonMonitorExclusive() 
{
	if (m_dwMonitor & USE_MONITOR_EXCLUSIVE)
	{
		m_dwMonitor &= ~USE_MONITOR_EXCLUSIVE;
	}
	else
	{
		CWndSmall *pSW = m_pViewIdipClient->GetWndSmallExclusive();
		if (pSW)
		{
			pSW->OnShowonMonitorExclusive();
		}
		m_dwMonitor |=  USE_MONITOR_EXCLUSIVE;
	}
	m_pViewIdipClient->PostMessage(WM_COMMAND, ID_VIEW_MONITOR16, 0);
}
/////////////////////////////////////////////////////////////////////////////
int CWndSmall::GetMonitor()
{
	CMainFrame *pFrame = theApp.GetMainFrame();
	HWND hwndParent = ::GetParent(m_hWnd);
	if (hwndParent == m_pViewIdipClient->m_hWnd)	
	{
		return 0;
	}
	else if (pFrame)
	{
		CDlgExternalFrame *pExternalFrame;
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
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::EnableTitle(BOOL bEnable)
{
	int		nShowWindow = SW_HIDE;
	int		nOldCY = m_wndToolBar.GetSize().cy;

	if (bEnable&SHOW_TITLE)
	{
		if (bEnable&SHOW_TITLE_IN_PICTURE)
		{
			m_wndToolBar.GetSize().cy = -1;
		}
		else
		{
			m_wndToolBar.GetSize().cy = TOOLBAR_HEIGHT;
			nShowWindow = SW_SHOW;
			m_bUpdateTitle = true;
		}
	}
	else
	{
		m_wndToolBar.GetSize().cy = 0;
	}

	if (m_wndToolBar.m_hWnd)
	{
		m_wndToolBar.ShowWindow(nShowWindow);
	}

	if (nOldCY != m_wndToolBar.GetSize().cy)
	{
		PostMessage(WM_SIZE, SIZE_RESTORED, 0);
		if (nShowWindow == SW_SHOW)
		{
			InvalidateRect(NULL);
		}
	}

/*
	ASSERT(GetParent() != NULL);
	CRect rc;
	GetWindowRect(&rc);
	GetParent()->ScreenToClient(&rc);
	rc.InflateRect(1,1);
	MoveWindow(&rc, FALSE);
	rc.DeflateRect(1,1);
	MoveWindow(&rc);
*/
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CRuntimeClass* pClass = GetRuntimeClass();
	CString sMsg;
	sMsg.Format(_T("%s not overwritten for %s\n"), _T(__FUNCTION__), (LPCTSTR)CWK_String(pClass->m_lpszClassName));
	WK_TRACE_WARNING(_T("%s"), sMsg);
	pDC->TextOut(10,10, sMsg);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndSmall::OnPreparePrinting(CPrintInfo* pInfo)
{
	CRuntimeClass* pClass = GetRuntimeClass();
	CString sMsg;
	sMsg.Format(_T("%s not overwritten for %s\n"), _T(__FUNCTION__), (LPCTSTR)CWK_String(pClass->m_lpszClassName));
	WK_TRACE_WARNING(_T("%s"), sMsg);
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CString	CWndSmall::GetName()
{
	//This function was checked against Recherche by TKR, 13.04.2004
	CRuntimeClass* pClass = GetRuntimeClass();
	WK_TRACE_WARNING(_T("CWndSmall::GetName not overwritten for %s\n"), CWK_String(pClass->m_lpszClassName));
	return "";
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnIdle()
{
	//This function was checked against Recherche by TKR, 13.04.2004
	CRuntimeClass* pClass = GetRuntimeClass();
	WK_TRACE_WARNING(_T("CWndSmall::OnIdle not overwritten for %s\n"), CWK_String(pClass->m_lpszClassName));
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndSmall::Create(const RECT& rect, CViewIdipClient* pParentWnd)
{
	CRuntimeClass* pClass = GetRuntimeClass();
	WK_TRACE_WARNING(_T("CWndSmall::Create not overwritten for %s\n"), CWK_String(pClass->m_lpszClassName));
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndSmall::PreDestroyWindow(CViewIdipClient* pViewIdipClient)
{
	CRuntimeClass* pClass = GetRuntimeClass();
	CString s(pClass->m_lpszClassName);
	WK_TRACE_WARNING(_T("CWndSmall::PreDestroyWindow not overwritten for %s\n"), s);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
UINT CWndSmall::GetToolBarID()
{
	//This function was checked against Recherche by TKR, 13.04.2004
	CRuntimeClass* pClass = GetRuntimeClass();
	CString s(pClass->m_lpszClassName);
	WK_TRACE_WARNING(_T("CWndSmall::GetToolBarID not overwritten for %s\n"), s);
	return (UINT)(-1);
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnSetWindowSize()
{
	//This function was checked against Recherche by TKR, 14.04.2004
	CRuntimeClass* pClass = GetRuntimeClass();
	CString s(pClass->m_lpszClassName);
	WK_TRACE_WARNING(_T("CWndSmall::OnSetWindowSize not overwritten for %s\n"), s);
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::PopupMenu(CPoint pt)
{
	//This function was checked against Recherche by TKR, 14.04.2004
	CRuntimeClass* pClass = GetRuntimeClass();
	CString s(pClass->m_lpszClassName);
	WK_TRACE_WARNING(_T("CWndSmall::PopupMenu not overwritten for %s\n"), s);
}
/////////////////////////////////////////////////////////////////////////////
int CWndSmall::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	//This function was checked against Recherche by TKR, 13.04.2004
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (   !m_wndToolBar.Create(this) 
		|| !m_wndToolBar.LoadToolBar(GetToolBarID()))
	{
		TRACE(_T("Failed to create dib toolbar\n"));
		return -1;      // fail to create
	}

	DWORD dwStyle = m_wndToolBar.GetBarStyle();
	dwStyle &= ~(CBRS_BORDER_RIGHT|CBRS_BORDER_TOP|CBRS_BORDER_LEFT|CBRS_BORDER_BOTTOM);
	dwStyle |= CBRS_TOOLTIPS | CBRS_SIZE_FIXED;
	m_wndToolBar.SetBarStyle(dwStyle);
	if (CSkinDialog::UseOEMSkin())
	{
		m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT);
	}
	else
	{
		m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT);
	}
	CSize sizeButton(16,16);
	CSize sizeImage(12,12);
	m_wndToolBar.SendMessage(TB_SETBITMAPSIZE, 0, MAKELONG(sizeImage.cx, sizeImage.cy));
	m_wndToolBar.SendMessage(TB_SETBUTTONSIZE, 0, MAKELONG(sizeButton.cx, sizeButton.cy));
	m_wndToolBar.CalcFixedLayout(FALSE,TRUE);
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CWndSmall::OnSmallContext() 
{
	//This function was checked against Recherche by TKR, 14.04.2004
	CRect trect;
	CRect brect;
	CPoint pt;

	m_wndToolBar.GetItemRect(m_wndToolBar.CommandToIndex(ID_SMALL_CONTEXT),brect);
	m_wndToolBar.GetWindowRect(trect);
	ScreenToClient(trect);
	pt.x = trect.left + brect.left;
	pt.y = trect.bottom;
	ClientToScreen(&pt);
	// Fenster aktivieren für den Updatehandler und die korrekte Nachrichtenverarbeitung
	m_pViewIdipClient->SetWndSmallCmdActive(this);

	PopupMenu(pt);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWndSmall::PreTranslateMessage(MSG* pMsg)
{
	return CWnd::PreTranslateMessage(pMsg);
}
///////////////////////////////////////////////////////////////////
void CWndSmall::Set1to1(BOOL b1to1)
{
	//This function was checked against Recherche by TKR, 14.04.2004
	m_b1to1 = b1to1 ? true : false;
	RedrawWindow();
}
///////////////////////////////////////////////////////////////////
void CWndSmall::RedrawTitleToolbar(CDC* pDC, COLORREF crTitleBkGnd, COLORREF crTitleText)
{
	BOOL bRedraw = 3;
	COLORREF colOld = m_wndToolBar.GetBkColor();
	m_wndToolBar.SetBkColor(crTitleBkGnd);
	m_wndToolBar.SetTextColor(crTitleText);

	//invalidate WndLive only if BkColor has changed
	//because of Toolbar "flackering"
	if(IsWndLive() || IsWndPlay())
	{
		if (colOld == crTitleBkGnd)
		{
			bRedraw = FALSE;
		}
	}

	if (m_wndToolBar.GetSize().cy > 0)
	{
		if (m_wndToolBar.SetTitleText(GetTitleText(pDC)))
		{
			bRedraw &= ~2;
		}
		if(bRedraw)
		{
			m_wndToolBar.OnNcPaint();
			if (bRedraw & 2)
			{
				m_wndToolBar.InvalidateRect(NULL);
			}
		}
	}

}
///////////////////////////////////////////////////////////////////
CServer* CWndSmall::GetSafeServer()
{
	if (m_pViewIdipClient)
	{
		return m_pViewIdipClient->GetDocument()->GetServer(m_wServerID);
	}
	return NULL;
}
///////////////////////////////////////////////////////////////////
CString CWndSmall::InsertSpacesForTabbedOutput(const CString sIn)
{
	CStringArray sa;
	CString sResult;
	SplitString(sIn, sa, _T('\n'));
	int i, nCount = sa.GetCount(), nFind, nMax = 0;
	for (i=0; i<nCount; i++)
	{
		nFind = sa.GetAt(i).Find(_T('\t'));
		if(nFind > nMax)
		{
			nMax = nFind;
		}
	}
	sResult.Empty();
	for (i=0; i<nCount; i++)
	{
		nFind = sa.GetAt(i).Find(_T('\t'));
		CString sSpace(_T(' '), MulDiv(nMax - nFind, 15, 10));
		sResult += sa.GetAt(i).Left(nFind) + sSpace + sa.GetAt(i).Mid(nFind);
		if (i<nCount-1)
		{
			sResult += _T("\n");
		}
	}

	return sResult;
}
///////////////////////////////////////////////////////////////////
BOOL CWndSmall::IsCmdActive()
{
	return (void*)m_pViewIdipClient->m_pWndSmallCmdActive == (void*)this;
}
///////////////////////////////////////////////////////////////////
void CWndSmall::OnUpdateEditComment(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(FALSE);
}
