// DisplayWindow.cpp : implementation file
//

#include "stdafx.h"
#include "Vision.h"

#include "SmallToolBar.h"

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
////////////////////////////////////////////////////////////////
CSmallToolBar::CSmallToolBar()
{
}
////////////////////////////////////////////////////////////////
CSmallToolBar::~CSmallToolBar()
{
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
	return 0L;
}
