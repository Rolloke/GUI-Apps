// ViewDlg.cpp : implementation file
//
#include "StdAfx.h"
#include "IdipClient.h"

#include "ViewDlg.h"


IMPLEMENT_DYNAMIC(CDlgInView, CSkinDialog)

CDlgInView::CDlgInView(UINT nID, CWnd* pParent /*=NULL*/)
	: CSkinDialog(nID, pParent)
{
	m_szMin.cx = m_szMin.cy = 0;
	m_pView = (CViewDialogBar*)pParent;
	m_eVisibility = OTVS_InitiallyYes;
}

CDlgInView::~CDlgInView(void)
{
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CDlgInView, CSkinDialog)
	ON_WM_CREATE()
	ON_WM_GETMINMAXINFO()
	ON_WM_DESTROY()
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CDlgInView message handlers
/////////////////////////////////////////////////////////////////////////////
int CDlgInView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CSkinDialog::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!theApp.IsReadOnlyModus())
	{
		CRuntimeClass*pRTC = GetRuntimeClass();
		if (pRTC)
		{
			CWK_Profile wkp;
			CString sSection(&pRTC->m_lpszClassName[1]);
			m_eVisibility = (eVisibilityStates)wkp.GetInt(IDIP_CLIENT_VISIBILITY, sSection, m_eVisibility);
		}
	}

	if (!StretchElements())
	{
		GetClientRect(m_InitialClientRect);
	}
	CRect rc;
	GetClientRect(rc);
	m_szMin.cx = rc.Width();
	m_szMin.cy = rc.Height();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CDlgInView::OnGetMinMaxInfo(MINMAXINFO* pMMI)
{
   if (m_szMin.cx && m_szMin.cy)
   {
      pMMI->ptMinTrackSize.x = m_szMin.cx;
      pMMI->ptMinTrackSize.y = m_szMin.cy;
   }
	CSkinDialog::OnGetMinMaxInfo(pMMI);
}
/////////////////////////////////////////////////////////////////////////////
void CDlgInView::OnDestroy()
{
	CSkinDialog::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
void CDlgInView::SetVisibility(eVisibilityStates eVisibility)
{
	m_eVisibility = eVisibility;
	CRuntimeClass*pRTC = GetRuntimeClass();
	if (pRTC)
	{
		CWK_Profile wkp;
		CString sSection(&pRTC->m_lpszClassName[1]);
		wkp.WriteInt(IDIP_CLIENT_VISIBILITY, sSection, m_eVisibility);
	}
}
