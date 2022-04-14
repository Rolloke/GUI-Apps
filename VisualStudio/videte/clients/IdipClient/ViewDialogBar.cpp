// DialogBarView.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"

#include "ViewDialogBar.h"

#include "DlgShowOEM.h"
#include "DlgBarIdipClient.h"
#include "DlgSubstitute.h"

#include "SplitterTopBottom.h"
#include "SplitterObjectView.h"
#include ".\viewdialogbar.h"

/////////////////////////////////////////////////////////////////////////////////////////
// CViewDialogBar

IMPLEMENT_DYNCREATE(CViewDialogBar, CView)
/////////////////////////////////////////////////////////////////////////////////////////
CViewDialogBar::CViewDialogBar()
{
	m_pDlgBar = NULL;
	m_pDlgSubstitute = NULL;
	m_bAvertRecursion = FALSE;
}
/////////////////////////////////////////////////////////////////////////////////////////
CViewDialogBar::~CViewDialogBar()
{
	WK_DELETE(m_pDlgBar);
	WK_DELETE(m_pDlgSubstitute);
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CViewDialogBar::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	BOOL bReturn = CWnd::CreateEx(0, lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
	if (bReturn)
	{
		if (pParentWnd->IsKindOf(RUNTIME_CLASS(CSplitterTopBottom)))
		{
			m_pDlgBar = new CDlgBarIdipClient(this);;
		}
		else
		{
			m_pDlgBar = new CDlgShowOEM(this);
		}
		if (m_pDlgBar->m_eVisibility == OTVS_InitiallyYes)
		{
			m_pDlgBar->ShowWindow(SW_SHOW);
		}
		else
		{
			AfxGetMainWnd()->PostMessage(WM_USER, WM_CLOSE, (LPARAM)m_pDlgBar);
		}
	}
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CViewDialogBar, CView)
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////////////////
// CViewDialogBar drawing
void CViewDialogBar::OnDraw(CDC* pDC)
{
}
/////////////////////////////////////////////////////////////////////////////////////////
// CViewDialogBar diagnostics
#ifdef _DEBUG
void CViewDialogBar::AssertValid() const
{
	CView::AssertValid();
}
/////////////////////////////////////////////////////////////////////////////////////////
void CViewDialogBar::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////////////////
// CViewDialogBar message handlers
void CViewDialogBar::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	if (m_pDlgBar && m_pDlgBar->IsWindowVisible() && ::GetParent(m_pDlgBar->m_hWnd) == m_hWnd)
	{
		m_pDlgBar->MoveWindow(0, 0, cx, cy);
	}

	if (m_pDlgSubstitute && m_pDlgSubstitute->IsWindowVisible())
	{
		m_pDlgSubstitute->MoveWindow(0, 0, cx, cy);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
void CViewDialogBar::UpdateDlgControls(CCmdTarget* pTarget, BOOL bDisableIfNoHndler)
{
	if (!m_bAvertRecursion && m_pDlgBar)
	{
		m_bAvertRecursion = TRUE;
		m_pDlgBar->UpdateDialogControls(pTarget, bDisableIfNoHndler);
		m_bAvertRecursion = FALSE;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
// retrieves the height of the dialog bar
int CViewDialogBar::GetBarHeight(void)
{
	if (m_pDlgSubstitute && m_pDlgSubstitute->IsWindowVisible())
	{
		return theApp.GetIntValue(_T("MinimizedBarHeight"), 12);
	}
	if (m_pDlgBar)
	{
		return m_pDlgBar->GetHeight();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////
int CViewDialogBar::GetBarWidth(void)
{
	if (m_pDlgSubstitute && m_pDlgSubstitute->IsWindowVisible())
	{
		return theApp.GetIntValue(_T("MinimizedBarWidth"), 10);
	}
	if (m_pDlgBar)
	{
		return m_pDlgBar->GetWidth();
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CViewDialogBar::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
//	return CView::OnEraseBkgnd(pDC);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CViewDialogBar::ShowDialog(BOOL bShow)
{
	CWnd *pWnd = GetParent();
	ASSERT_KINDOF(CSplitterWndCommon, pWnd);
	CSplitterWndCommon*pSW = (CSplitterWndCommon*)pWnd;

	int nID = GetDlgCtrlID();
	if (m_pDlgSubstitute == NULL && !bShow)
	{
		m_pDlgSubstitute = new CDlgSubstitute(this);
	}

	if (m_pDlgSubstitute)
	{
		m_pDlgSubstitute->SetImageList(pSW->GetSubstImageList(nID));
		m_pDlgSubstitute->SetShowID(pSW->GetSubstCmdID());
	}

	if (m_pDlgBar)
	{
		m_pDlgBar->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	}

	if (m_pDlgSubstitute)
	{
		m_pDlgSubstitute->ShowWindow(bShow ? SW_HIDE : SW_SHOW);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////
bool CViewDialogBar::IsDlgVisible()
{
	if (m_pDlgBar)
	{
		return m_pDlgBar->IsWindowVisible() ? true : false;
	}
	return false;
}
/////////////////////////////////////////////////////////////////////////////////////////
bool CViewDialogBar::IsDlgSubstituteVisible()
{
	if (m_pDlgSubstitute)
	{
		return m_pDlgSubstitute->IsWindowVisible() ? true : false;
	}
	return false;
}

void CViewDialogBar::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// RKE: Do Nothing
	// Base class calls Invalidate(TRUE);
}

void CViewDialogBar::OnDestroy()
{
	GetParent()->SendMessage(WM_USER, WM_DESTROY, (LPARAM)this);
	CView::OnDestroy();
}
