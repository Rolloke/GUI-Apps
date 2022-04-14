// SplitterObjectView.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "SplitterTopBottom.h"

#include "MainFrm.h"
#include "ViewDialogBar.h"
#include "ViewIdipClient.h"

// CSplitterTopBottom

IMPLEMENT_DYNAMIC(CSplitterTopBottom, CSplitterWndCommon)

CSplitterTopBottom::CSplitterTopBottom()
{
	m_pDlgBarView   = NULL;
	m_pViewIdipClient = NULL;
	m_nDlgBarRow    = -1;
	m_cySplitterGap = 1;
	m_cySplitter    = 1;
	m_cxBorder = 1;
	m_cyBorder = 1;
}

CSplitterTopBottom::~CSplitterTopBottom()
{
}


BEGIN_MESSAGE_MAP(CSplitterTopBottom, CSplitterWndCommon)
	ON_WM_SIZE()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CSplitterTopBottom message handlers
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterTopBottom::OnSize(UINT nType, int cx, int cy)
{
//	TRACE(_T("CSplitterTopBottom::OnSize cx:%d , cy:%d\n"), cx, cy);
	CMainFrame* pMainFrame = theApp.GetMainFrame();
	if (   pMainFrame 
		&& pMainFrame->IsSplitterWndCreated()
//		&& nType != SIZE_MINIMIZED 
		&& nType == SIZE_UPDATE
		&& cx > 0 && cy > 0)
	{
		int nHeight		= cy - (m_cySplitterGap* (m_nRows - 1));
		int nBarHeight = 0;	// height of the DlgBar is fixed

		if (   m_pDlgBarView 
			&& !pMainFrame->IsFullScreen())
		{
			nBarHeight = m_pDlgBarView->GetBarHeight();
		}

		SetRowInfo(m_nDlgBarRow, nBarHeight, 10);
		int nViewHeight = max(nHeight-nBarHeight, 1);
		SetRowInfo(!m_nDlgBarRow, nViewHeight, 10);
		CViewIdipClient* pVV = pMainFrame->GetViewIdipClient();
		pVV->LockSmallWindowResizing(-1);
	}
	CSplitterWndCommon::OnSize(nType, cx, cy);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterTopBottom::OnLButtonUp(UINT nFlags, CPoint point)
{
	CSplitterWndCommon::OnLButtonUp(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CSplitterTopBottom::CreateView(int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext)
{
	BOOL bReturn = CSplitterWndCommon::CreateView(row, col, pViewClass, sizeInit, pContext);
	if (bReturn)
	{
		CWnd *pWnd =  GetPane(row, col);
		ASSERT(pWnd != NULL);
		if (pWnd->IsKindOf(RUNTIME_CLASS(CViewDialogBar)))
		{
			m_pDlgBarView = (CViewDialogBar*) pWnd;
			m_nDlgBarRow  = row;
		}
		if (pWnd->IsKindOf(RUNTIME_CLASS(CViewIdipClient)))
		{
			m_pViewIdipClient = (CViewIdipClient*) pWnd;
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterTopBottom::UpdateDlgControls(CCmdTarget* pTarget, BOOL bDisableIfNoHndler)
{
	if (m_pDlgBarView)
	{
		m_pDlgBarView->UpdateDlgControls(pTarget, bDisableIfNoHndler);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
void CSplitterTopBottom::OnMouseMove(UINT nFlags, CPoint point)
{
//	CSplitterWndCommon::OnMouseMove(nFlags, point);
}
/////////////////////////////////////////////////////////////////////////////////////////
int	CSplitterTopBottom::GetMinWidth()
{
	if (m_pDlgBarView && m_pDlgBarView->IsDlgVisible())
	{
		return m_pDlgBarView->GetBarWidth();
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////
int	CSplitterTopBottom::GetBarHeight()
{
	if (m_pDlgBarView && m_pDlgBarView->IsWindowVisible())
	{
		return m_pDlgBarView->GetBarHeight();
	}
	return 1;
}
/////////////////////////////////////////////////////////////////////////////////////////
UINT CSplitterTopBottom::GetSubstCmdID()
{
	return ID_VIEW_DIALOGBAR;
}
/////////////////////////////////////////////////////////////////////////////////////////
CImageList* CSplitterTopBottom::GetSubstImageList(int nID)
{
	return (nID == IdFromRowCol(0, 0)) ? CSkinDialog::GetDownButtonImages() : CSkinDialog::GetUpButtonImages();
}
