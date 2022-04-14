// DisplayWindow.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"

#include "SmallToolBar.h"
#include "WndSmall.h"
#include ".\smalltoolbar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////
// Special tool bar to use in display window.
//
IMPLEMENT_DYNAMIC(CSmallToolBar, CToolBar)

BEGIN_MESSAGE_MAP(CSmallToolBar, CToolBar)
	//{{AFX_MSG_MAP(CSmallToolBar)
	ON_MESSAGE(WM_IDLEUPDATECMDUI, OnIdleUpdateCmdUI)
	ON_WM_ERASEBKGND()
	ON_WM_NCPAINT()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////
CSmallToolBar::CSmallToolBar()
{
	m_BkColor = GetSysColor(COLOR_WINDOWFRAME);
	m_Size = CSize(0, TOOLBAR_HEIGHT);
}
////////////////////////////////////////////////////////////////
CSmallToolBar::~CSmallToolBar()
{
}
////////////////////////////////////////////////////////////////
BOOL CSmallToolBar::SetTitleText(const CString &sNewTitle)
{
	if (m_sTitleText != sNewTitle)
	{
		m_sTitleText = sNewTitle;
		InvalidateRect(NULL);
		return TRUE;
	}
	return FALSE;
}
////////////////////////////////////////////////////////////////
LRESULT CSmallToolBar::OnIdleUpdateCmdUI(WPARAM wParam, LPARAM lParam)
{
	// handle delay hide/show
	BOOL bVis = GetStyle() & WS_VISIBLE;
	UINT swpFlags = 0;
	if ((m_nStateFlags & delayHide) && bVis)
		swpFlags = SWP_HIDEWINDOW;
	else if ((m_nStateFlags & delayShow) && !bVis)
		swpFlags = SWP_SHOWWINDOW;
	m_nStateFlags &= ~(delayShow|delayHide);
	if (swpFlags != 0)
	{
		SetWindowPos(NULL, 0, 0, 0, 0, swpFlags|
			SWP_NOMOVE|SWP_NOSIZE|SWP_NOZORDER|SWP_NOACTIVATE);
	}

	// the style must be visible and if it is docked
	// the dockbar style must also be visible
	if ((GetStyle() & WS_VISIBLE) &&
		(m_pDockBar == NULL || (m_pDockBar->GetStyle() & WS_VISIBLE)))
	{
		CWnd* pTarget = GetOwner();
		if (pTarget != NULL)
			OnUpdateCmdUI((CFrameWnd*)pTarget, (BOOL)wParam);
	}
	if (m_bDelayedButtonLayout)
	{
		RedrawWindow();
		m_bDelayedButtonLayout = FALSE;
	}
	return 0L;
}
////////////////////////////////////////////////////////////////
BOOL CSmallToolBar::OnEraseBkgnd(CDC* pDC)
{
	CRect rc, rcClip;
	GetClientRect(&rc);
	CBrush br(m_BkColor);
	pDC->FillRect(&rc, &br);
	COLORREF colOld = pDC->SetTextColor(m_TextColor);
	CWnd *pWnd = GetParent();
	ASSERT_KINDOF(CWndSmall, pWnd);
	rc.right -= m_Size.cx;
	((CWndSmall*)pWnd)->DrawTitleTransparent(pDC, &rc, m_sTitleText);
	pDC->SetTextColor(colOld);
	return TRUE;
}
////////////////////////////////////////////////////////////////
void CSmallToolBar::OnNcPaint()
{
	CWindowDC dc(this);
	CRect rcW, rcC;
	GetWindowRect(&rcW);
	ScreenToClient(&rcW);
	GetClientRect(&rcC);
	rcC.OffsetRect(-rcW.left, -rcW.top);
	rcW.OffsetRect(-rcW.left, -rcW.top);
	CRgn rgnW, rgnC;
	rgnW.CreateRectRgnIndirect(&rcW);
	rgnC.CreateRectRgnIndirect(&rcC);
	rgnW.CombineRgn(&rgnW, &rgnC, RGN_DIFF);
	CBrush br(m_BkColor);
	dc.FillRgn(&rgnW, &br);
}
////////////////////////////////////////////////////////////////
CSize CSmallToolBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz)
{
	CSize size = CToolBar::CalcFixedLayout(bStretch, bHorz);
	m_Size.cx = size.cx + GetToolBarCtrl().GetButtonCount()*3;
	return size;
}
////////////////////////////////////////////////////////////////
LRESULT CSmallToolBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_LBUTTONDOWN:		// send the title click to the parent window
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDBLCLK:
		case WM_RBUTTONDBLCLK:
		{
			CRect rcW;
			CPoint point(lParam);
			GetWindowRect(&rcW);
			rcW.OffsetRect(-rcW.left, -rcW.top);
			rcW.right -= m_Size.cx;
			if (rcW.PtInRect(point))
			{
				GetParent()->SendMessage(message, wParam, lParam);
			}
		}break;
	}

	return CToolBar::WindowProc(message, wParam, lParam);
}
