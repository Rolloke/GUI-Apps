// SVPropertySheet.cpp: implementation of the CSVPropertySheet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SVPropertySheet.h"
#include "PropPageVariable.h"
#include "SVView.h"
#include "SVPage.h"
#include "SVTree.h"
#include "MainFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CSVPropertySheet
IMPLEMENT_DYNAMIC(CSVPropertySheet, CPropertySheet)

/////////////////////////////////////////////////////////////////////////////
CSVPropertySheet::CSVPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
   m_bDeleting = false;
   m_bInitialized = false;
   m_nLastBtnClicked = 0;
   m_psh.dwFlags |= PSH_WIZARD;
}
/////////////////////////////////////////////////////////////////////////////
CSVPropertySheet::CSVPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
   m_bDeleting = false;
   m_nLastBtnClicked = 0;
   m_psh.dwFlags |= PSH_WIZARD;
}
/////////////////////////////////////////////////////////////////////////////
CSVPropertySheet::~CSVPropertySheet()
{
   DeletePages();
   GetSVView()->EmptyWizardPointer();
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSVPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CSVPropertySheet)
	ON_WM_CLOSE()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_MESSAGE(WM_USER, OnUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CSVPropertySheet message handlers
CSVView *CSVPropertySheet::GetSVView()
{
   ASSERT_KINDOF(CSVView, m_pParentWnd);
   return (CSVView*)m_pParentWnd;
}
/////////////////////////////////////////////////////////////////////////////
void CSVPropertySheet::DeletePages()
{
	m_bDeleting = true;
	while (m_PageList.GetCount())
	{
		CPropertyPage*pP = (CPropertyPage*) m_PageList.RemoveHead();
		if (m_hWnd) RemovePage(pP);
		WK_DELETE(pP);
	}
	m_bDeleting = false;
}
/////////////////////////////////////////////////////////////////////////////
CPropPageVariable* CSVPropertySheet::InsertPage(CPropPageVariable *pP)
{
   ASSERT(pP != NULL);
   m_PageList.AddHead(pP);
   AddPage(pP);
   return pP;
}
/////////////////////////////////////////////////////////////////////////////
void CSVPropertySheet::OnClose() 
{
	m_pParentWnd->SendMessage(WM_SET_WIZARD_SIZE, NULL, 0);
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	pMF->SetWizardCloseProc(false);
	CPropertySheet::OnClose();
}
/////////////////////////////////////////////////////////////////////////////
int CSVPropertySheet::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CPropertySheet::OnCreate(lpCreateStruct) == -1)
		return -1;
	SetParent(m_pParentWnd);
	CMainFrame *pMF = (CMainFrame*)AfxGetMainWnd();
	pMF->SetWizardCloseProc(true);
   
	return 0;
}
BOOL CSVPropertySheet::OnInitDialog()
{
	BOOL bReturn = CPropertySheet::OnInitDialog();
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
bool CSVPropertySheet::AllowNew()
{
   bool bReturn = false;
   GetActivePage()->SendMessage(WM_GET_WPAGE_PARAM, ALLOW_NEW, (WPARAM)&bReturn);
   return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
bool CSVPropertySheet::AllowDelete()
{
   bool bReturn = false;
   GetActivePage()->SendMessage(WM_GET_WPAGE_PARAM, ALLOW_DELETE, (WPARAM)&bReturn);
   return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
bool CSVPropertySheet::AllowSave()
{
   bool bReturn = false;
   GetActivePage()->SendMessage(WM_GET_WPAGE_PARAM, ALLOW_SAVE, (WPARAM)&bReturn);
   return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
bool CSVPropertySheet::AllowCancel()
{
   bool bReturn = false;
   GetActivePage()->SendMessage(WM_GET_WPAGE_PARAM, ALLOW_CANCEL, (WPARAM)&bReturn);
   return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
bool CSVPropertySheet::AllowInit()
{
   bool bReturn = false;
   GetActivePage()->SendMessage(WM_GET_WPAGE_PARAM, ALLOW_INIT, (WPARAM)&bReturn);
   return bReturn;
}
/////////////////////////////////////////////////////////////////////////////
void CSVPropertySheet::OnDestroy() 
{
	GetSVView()->GetSVPage()->EnableWindow();
//   GetSVView()->GetSVTree()->EnableWindow();
	GetSVView()->GetSVTree()->DisableMouse(false);
	GetSVView()->GetSVPage()->SetSelection(0);
	GetSVView()->GetSVPage()->SetSelections(0);

	CPropertySheet::OnDestroy();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVPropertySheet::Create(CWnd* pParentWnd)
{
	return CPropertySheet::Create(pParentWnd,
											WS_POPUP|WS_CHILD|WS_CLIPSIBLINGS|WS_TILED|DS_SETFONT|DS_3DLOOK,
											WS_EX_LEFT|WS_EX_LTRREADING|WS_EX_CONTROLPARENT|WS_EX_TOOLWINDOW|WS_EX_STATICEDGE);
}
/////////////////////////////////////////////////////////////////////////////
bool CSVPropertySheet::IsDeleting()
{
	return m_bDeleting;
}
/////////////////////////////////////////////////////////////////////////////
void CSVPropertySheet::PressLastButton()
{
	if (m_nLastBtnClicked)
	{
		PostMessage(WM_COMMAND, m_nLastBtnClicked);
	}
	else
	{
		PostMessage(WM_COMMAND, ID_WIZFINISH);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVPropertySheet::SetLastButton(int nID)
{
	m_nLastBtnClicked = nID;
}
/////////////////////////////////////////////////////////////////////////////
LRESULT CSVPropertySheet::OnUser(WPARAM wParam, LPARAM lParam)
{
	if (LOWORD(wParam) == WM_SET_WIZARD_SIZE && !m_bInitialized)
	{
		CRect rc;
		GetClientRect(&rc);
		m_pParentWnd->SendMessage(WM_SET_WIZARD_SIZE, (WPARAM)&rc, 0);
		m_bInitialized = true;
	}
	return 0;
}
/////////////////////////////////////////////////////////////////////////////
void CSVPropertySheet::PostNcDestroy()
{
	delete this;
}
