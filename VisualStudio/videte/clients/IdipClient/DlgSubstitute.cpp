// DlgSubstitute.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgSubstitute.h"
#include ".\dlgsubstitute.h"


/////////////////////////////////////////////////////////////////////////////////////////
// CDlgSubstitute dialog
/////////////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(CDlgSubstitute, CDlgInView)
/////////////////////////////////////////////////////////////////////////////////////////
CDlgSubstitute::CDlgSubstitute(CWnd* pParent)
	: CDlgInView(CDlgSubstitute::IDD, pParent)
{
	m_nInitToolTips = FALSE;
	m_nShowID = 0;
	Create(CDlgSubstitute::IDD, pParent);
}
/////////////////////////////////////////////////////////////////////////////////////////
void CDlgSubstitute::DoDataExchange(CDataExchange* pDX)
{
	CDlgInView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SHOW_ANCESTOR, m_btnShowAncestor);
}
/////////////////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgSubstitute, CDlgInView)
	ON_BN_CLICKED(IDC_SHOW_ANCESTOR, OnBnClickedShowAncestor)
	ON_WM_SIZE()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////////////////
// CDlgSubstitute message handlers
BOOL CDlgSubstitute::OnInitDialog()
{
	StretchButtonHeight(true);
	MoveOnClientAreaClick(false);
	CDlgInView::OnInitDialog();
//	m_btnShowAncestor.EnableActionOnButtonDown();
	m_btnShowAncestor.SetSurface(SurfacePlaneColor);
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////////////////
void CDlgSubstitute::SetImageList(CImageList*pIL)
{
	m_btnShowAncestor.SetImageList(pIL);
}
/////////////////////////////////////////////////////////////////////////////////////////
BOOL CDlgSubstitute::StretchElements()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////////////////
void CDlgSubstitute::OnBnClickedShowAncestor()
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND, m_nShowID);	
}
/////////////////////////////////////////////////////////////////////////////////////////
void CDlgSubstitute::OnSize(UINT nType, int cx, int cy)
{
	if (m_btnShowAncestor.m_hWnd)
	{
		m_btnShowAncestor.MoveWindow(0, 0, cx, cy);
	}
}
