// BaseCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "GraphCreator.h"
#include "BaseCtrl.h"
#include "GraphCreatorView.h"
#include ".\basectrl.h"


/////////////////////////////////////////////////////////////////////////////
// CBaseCtrl
int CBaseCtrl::gm_nPinOffsetY     = 10;
int CBaseCtrl::gm_nPinDistanceY   = 5;
int CBaseCtrl::gm_nMinHeight      = 80;
int CBaseCtrl::gm_nTextDistance   = 4;
int CBaseCtrl::gm_nFilterDistance = 40;

/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNCREATE(CBaseCtrl, CButton)
/////////////////////////////////////////////////////////////////////////////
CBaseCtrl::CBaseCtrl():
	m_ptMove(-1, -1)
{
	m_bFocus = false;
}
/////////////////////////////////////////////////////////////////////////////
CBaseCtrl::~CBaseCtrl()
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CBaseCtrl, CButton)
	//{{AFX_MSG_MAP(CBaseCtrl)
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_ERASEBKGND()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CBaseCtrl message handlers
/////////////////////////////////////////////////////////////////////////////
void CBaseCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	CButton::OnSetFocus(pOldWnd);
	m_bFocus = true;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CBaseCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	m_bFocus = false;
	InvalidateRect(NULL);
}
/////////////////////////////////////////////////////////////////////////////
void CBaseCtrl::PostNcDestroy() 
{
	CButton::PostNcDestroy();
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
void CBaseCtrl::PreSubclassWindow()
{
	CButton::PreSubclassWindow();
	SetButtonStyle(GetButtonStyle() | BS_OWNERDRAW);
}
/////////////////////////////////////////////////////////////////////////////
void CBaseCtrl::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	dc.SaveDC();
	if (m_bFocus)
	{
		dc.SelectStockObject(WHITE_BRUSH);
	}
	else
	{
		dc.SelectStockObject(LTGRAY_BRUSH);
	}

//	OnDraw(&dc);
	dc.RestoreDC(-1);
}
//////////////////////////////////////////////////////////////////////////
void CBaseCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
}
/////////////////////////////////////////////////////////////////////////////
CGraphCreatorView* CBaseCtrl::GetView()
{
	CWnd *pWnd = GetParent();
	ASSERT(pWnd != NULL);
	ASSERT_KINDOF(CGraphCreatorView, pWnd);
	return (CGraphCreatorView*) pWnd;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBaseCtrl::OnEraseBkgnd(CDC* pDC)
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBaseCtrl::AddNextCtrl(UINT nID)
{
	m_dwaNextCtrls.Add(nID);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CBaseCtrl::AddPrevCtrl(UINT nID)
{
	m_dwaPrevCtrls.Add(nID);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CBaseCtrl::RemoveCtrlID(UINT nID)
{
	int i, n = m_dwaPrevCtrls.GetCount();
	for (i=0; i<n; i++)
	{
		if (nID == m_dwaPrevCtrls[i])
		{
			m_dwaPrevCtrls.RemoveAt(i);
			break;
		}
	}
	n = m_dwaNextCtrls.GetCount();
	for (i=0; i<n; i++)
	{
		if (nID == m_dwaNextCtrls[i])
		{
			m_dwaNextCtrls.RemoveAt(i);
			break;
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
CBaseCtrl*CBaseCtrl::GetNextCtrl(int i)
{
	if (i>=0 && i<m_dwaNextCtrls.GetCount())
	{
		CWnd *pWnd = GetView()->GetDlgItem(m_dwaNextCtrls.GetAt(i));
#ifdef _DEBUG
		if (pWnd)
		{
			ASSERT_KINDOF(CBaseCtrl, pWnd);
		}
#endif
		return (CBaseCtrl*)pWnd;
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
CBaseCtrl*CBaseCtrl::GetPrevCtrl(int i)
{
	if (i>=0 && i<m_dwaPrevCtrls.GetCount())
	{
		CWnd *pWnd =  GetView()->GetDlgItem(m_dwaPrevCtrls.GetAt(i));
#ifdef _DEBUG
		if (pWnd)
		{
			ASSERT_KINDOF(CBaseCtrl, pWnd);
		}
#endif
		return (CBaseCtrl*)pWnd;
	}
	return NULL;
}
/////////////////////////////////////////////////////////////////////////////
void CBaseCtrl::OnDestroy()
{
	GetView()->PostMessage(WM_USER, WPARAM_CTRL_REMOVED, GetDlgCtrlID());
	CButton::OnDestroy();
}
