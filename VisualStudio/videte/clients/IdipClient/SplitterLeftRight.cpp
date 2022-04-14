// SplitterLeftRight.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "SplitterLeftRight.h"

#include "MainFrm.h"
#include "ViewDlg.h"

#include "ViewDialogBar.h"

#include "SplitterTopBottom.h"
#include "SplitterObjectView.h"


// CSplitterLeftRight
IMPLEMENT_DYNAMIC(CSplitterLeftRight, CSplitterWndCommon)

CSplitterLeftRight::CSplitterLeftRight()
{
	m_nObjectViewWidth = 0;
	m_nTopBottom  = 0;	// OEM abhängig
	m_nObjectView = 1;
	m_bShowObjectViews = TRUE;
	m_bCorrectObjectViewWidth = FALSE;

	m_cxBorder = 1;
	m_cyBorder = 1;
	m_cxSplitterGap = 7;
	m_cxSplitter    = 7;
}

CSplitterLeftRight::~CSplitterLeftRight()
{
}


BEGIN_MESSAGE_MAP(CSplitterLeftRight, CSplitterWndCommon)
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()

// CSplitterLeftRight message handlers
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterLeftRight::OnSize(UINT nType, int cx, int cy)
{
	if (theApp.GetMainFrame() 
		&& theApp.GetMainFrame()->IsSplitterWndCreated()
//		&& nType != SIZE_MINIMIZED 
		&& nType == SIZE_UPDATE
		&& cx > 0 && cy > 0)
	{
		int nWidthTopBottom, nWidthObjectView;
		GetWidths(cx, nWidthTopBottom, nWidthObjectView);

		if (nWidthTopBottom < 0) nWidthTopBottom = 0;
		SetColumnInfo(m_nTopBottom, nWidthTopBottom , 10);

		if (nWidthObjectView < 0) nWidthObjectView = 0;
		SetColumnInfo(m_nObjectView, nWidthObjectView, 10);

		if (nType == SIZE_UPDATE)
		{
			CSplitterTopBottom *pTB = GetSplitterTopBottom();
			CSplitterObjectView*pOV = GetSplitterObjectView();
			if (pTB)
			{
				pTB->PostMessage(WM_SIZE, nType, MAKELPARAM(nWidthTopBottom, cy));
			}
			if (pOV)
			{
				pOV->PostMessage(WM_SIZE, nType, MAKELPARAM(nWidthObjectView, cy));
			}
		}
	}
	CSplitterWndCommon::OnSize(nType, cx, cy);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterLeftRight::OnLButtonUp(UINT nFlags, CPoint point)
{
	CRect rect;
	GetWindowRect( &rect );
	m_nObjectViewWidth = (rect.Width()- m_cxSplitterGap / 2) - point.x;
	m_bCorrectObjectViewWidth = TRUE;
	StopTracking(FALSE);
//	CSplitterWndCommon::OnLButtonUp(nFlags, point);
	UpdateSizes();
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CSplitterLeftRight::CreateView(int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext)
{
	BOOL bReturn = CSplitterWndCommon::CreateView(row, col, pViewClass, sizeInit, pContext);
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////////////////
CSplitterTopBottom  * CSplitterLeftRight::GetSplitterTopBottom()
{
	CWnd *pWnd = GetPane(0, m_nTopBottom);
	ASSERT(pWnd != NULL);
	ASSERT_KINDOF(CSplitterTopBottom, pWnd);
	return (CSplitterTopBottom*)pWnd;
}
/////////////////////////////////////////////////////////////////////////////////////////
CSplitterObjectView * CSplitterLeftRight::GetSplitterObjectView()
{
	CWnd *pWnd = GetPane(0, m_nObjectView);
	ASSERT(pWnd != NULL);
	ASSERT_KINDOF(CSplitterObjectView, pWnd);
	return (CSplitterObjectView*)pWnd;
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterLeftRight::SetObjectView(int nObjectView)
{
	m_nObjectView = nObjectView;
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterLeftRight::SetTopBottom(int nTopBottom)
{
	m_nTopBottom = nTopBottom;
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterLeftRight::InitColumns(void)
{
	CRect rc;
	GetInsideRect(rc);
	int nOVWidth = GetSplitterObjectView()->GetViewOEMDlg()->GetDlgInView()->GetWidth();
	int nTBWidth = GetSplitterTopBottom()->GetViewDlgBar()->GetDlgInView()->GetWidth();
	m_nObjectViewWidth = nOVWidth;
	SetColumnInfo(m_nObjectView, nOVWidth, 10);
	SetColumnInfo(m_nTopBottom, nTBWidth, 10);
	RecalcLayout();
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterLeftRight::ShowObjectView(BOOL bShow)
{
	m_bShowObjectViews = bShow;
	m_cxSplitterGap = m_cxSplitter = bShow ? 7 : 1;
	GetSplitterObjectView()->ShowObjectView(bShow);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterLeftRight::UpdateSizes()
{
	CRect rc;
	GetClientRect(rc);
	PostMessage(WM_SIZE, SIZE_UPDATE, MAKELPARAM(rc.Width(), rc.Height()));
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterLeftRight::GetWidths(int cx, int &nWidthTopBottom, int &nWidthObjectView)
{
	CSplitterTopBottom *pTB = GetSplitterTopBottom();
	CSplitterObjectView*pOV = GetSplitterObjectView();
	int nTopBottomMin   = pTB->GetMinWidth();
	int nObjectViewMin  = pOV->GetMinWidth();

	// if cx<0 it is a request for the minimal widths
	if (cx <0)
	{
		nWidthTopBottom  = nTopBottomMin;
		nWidthObjectView = nObjectViewMin;
	}
	else
	{
//		cx -= ((m_cxBorder + m_cxSplitterGap) * (m_nCols-1));
		cx -= GetGapAndBorderWidth();

		CMainFrame* pMainFrame = theApp.GetMainFrame();
		if (   pMainFrame
			&& pMainFrame->IsFullScreen()
			)
		{
			nWidthTopBottom  = cx;
			nWidthObjectView = 0;
			return;
		}

		nWidthTopBottom  = cx - m_nObjectViewWidth;
		nWidthObjectView = m_nObjectViewWidth;

		if (IsObjectViewVisible())						// Show ObjectViews
		{
			if (IsDialogBarVisible())					// Dialogbar: fixed size
			{
				if (IsOEMDialogVisible())				// OEM Dialog: fixed size
				{
					if (nWidthTopBottom < nTopBottomMin)// DialogBar is more important
					{
						nWidthTopBottom = nTopBottomMin;
						nWidthObjectView = cx - nWidthTopBottom;
					}
					else if (   nWidthObjectView < nObjectViewMin	// after OnLButtonUP !
							&& (nTopBottomMin + nObjectViewMin) < cx)
					{
						nWidthObjectView = nObjectViewMin;
						nWidthTopBottom  = cx - nWidthObjectView;
					}
				}
				else if (nWidthTopBottom < nTopBottomMin)// DialogBar is more important
				{
					nWidthTopBottom = nTopBottomMin;
					nWidthObjectView = cx - nWidthTopBottom;
				}
				if (m_bCorrectObjectViewWidth)
				{
					m_nObjectViewWidth = nWidthObjectView;
					m_bCorrectObjectViewWidth = FALSE;
				}
			}
		}
		else	// fixed, show IdipClientView and ObjectViews substitute button		
		{
			nWidthObjectView = pOV->GetViewOEMDlg()->GetBarWidth();
			nWidthTopBottom = cx - nWidthObjectView;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CSplitterLeftRight::IsOEMDialogVisible()
{
	return GetSplitterObjectView()->IsOEMDialogVisible();
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CSplitterLeftRight::IsDialogBarVisible()
{
	return GetSplitterTopBottom()->GetViewDlgBar()->GetDlgInView()->IsWindowVisible();
}
/////////////////////////////////////////////////////////////////////////////////////////
int CSplitterLeftRight::GetGapAndBorderWidth()
{
	return ( m_cxSplitterGap * (m_nCols-1) + m_cxBorder * 2);
}
