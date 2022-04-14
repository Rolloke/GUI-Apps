// ICStatusBar.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "ICStatusBar.h"
#include ".\icstatusbar.h"


// CICStatusBar

IMPLEMENT_DYNAMIC(CICStatusBar, CStatusBar)
CICStatusBar::CICStatusBar()
{
}

CICStatusBar::~CICStatusBar()
{
}


BEGIN_MESSAGE_MAP(CICStatusBar, CStatusBar)
	ON_WM_CREATE()
END_MESSAGE_MAP()

// CICStatusBar message handlers
int CICStatusBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CStatusBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	EnableToolTips(TRUE);
	return 0;
}

INT_PTR CICStatusBar::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	CRect rc;
	int i = -1, nWidth;
	CString str;
	UINT nID, nStyle;
	for (i=m_nCount-1; i>=0; i--)
	{
		GetPaneInfo(i, nID, nStyle, nWidth);
		GetItemRect(i, &rc);
		if (rc.PtInRect(point))
		{
			switch (nID)
			{
				case ID_SEQUENCE_PANE: break;
				case ID_INDICATOR_OVR: str = _T("Network Rate [%]"); break;
				case ID_INDICATOR_REC: str = _T("Performance Level [%]"); break;
				default: str.LoadString(nID); break;
			}
			break;
		}
		rc.right -= nWidth;
	}

	if (pTI && !str.IsEmpty())
	{
		pTI->uId      = (UINT) m_hWnd;
		pTI->uFlags  |= TTF_IDISHWND;
		pTI->hwnd     = m_hWnd;
		pTI->lpszText = _tcsdup(str);
	}
	else
	{
		i = -1;
	}
	return i;
//	return CStatusBar::OnToolHitTest(point, pTI);
}
