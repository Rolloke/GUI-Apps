// SVPage.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "SVPage.h"
#include "mainfrm.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSVApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSVPage property page

IMPLEMENT_DYNCREATE(CSVPage, CDialog)

CSVPage::CSVPage(UINT nIDTemplate) : CWK_Dialog(nIDTemplate)
{
	m_pParent = NULL;
	m_bModified = FALSE;
}

CSVPage::~CSVPage()
{
}

BEGIN_MESSAGE_MAP(CSVPage, CWK_Dialog)
	//{{AFX_MSG_MAP(CSVPage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CWK_Profile& CSVPage::GetProfile()
{
	CSVDoc* pDoc = m_pParent->GetDocument();
	return pDoc->GetProfile();
}
/////////////////////////////////////////////////////////////////////////////
CWK_Dongle& CSVPage::GetDongle()
{
	CSVDoc* pDoc = m_pParent->GetDocument();
	return *pDoc->GetDongle();
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::PostNcDestroy() 
{
	delete this;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVPage::PreTranslateMessage(MSG* pMsg)
{
	ASSERT(pMsg != NULL);
	ASSERT_VALID(this);
	ASSERT(m_hWnd != NULL);

	// don't call base class implementation 
	// it will eat frame window accelerators

	// don't translate dialog messages when in Shift+F1 help mode
	CFrameWnd* pFrameWnd = GetTopLevelFrame();
	if (pFrameWnd != NULL && pFrameWnd->m_bHelpMode)
		return FALSE;

	// since 'IsDialogMessage' will eat frame window accelerators,
	//   we call all frame windows' PreTranslateMessage first
	pFrameWnd = GetParentFrame();   // start with first parent frame
	while (pFrameWnd != NULL)
	{
		// allow owner & frames to translate before IsDialogMessage does
		if (pFrameWnd->PreTranslateMessage(pMsg))
			return TRUE;

		// try parent frames until there are no parent frames
		pFrameWnd = pFrameWnd->GetParentFrame();
	}

	if (CWK_Dialog::PreTranslateMessage(pMsg))
	{
		return TRUE;
	}

	// filter both messages to dialog and from children
	return PreTranslateInput(pMsg);
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::SetModified(BOOL bModified/*=TRUE*/, BOOL bServerInit/*=TRUE*/)
{
	m_bModified = bModified;
	if (m_bModified && bServerInit)
	{
		m_pParent->m_bServerInit = TRUE;
	}
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVPage::IsDataValid()
{
	// should be overridden by subclasses
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnItemChangedStatusList(int /* nDlgItemID */, int /* nListItem */)
{
	// should be overridden by subclasses
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::Initialize()
{
	// should be overridden by subclasses
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnNew()
{
	// should be overridden by subclasses
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnDelete()
{
	// should be overridden by subclasses
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVPage::CanNew()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSVPage::CanDelete()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnCancel() 
{
	if (m_bModified) 
	{
		CancelChanges();
		SetModified(FALSE,FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::OnOK()
{
	if (m_bModified) 
	{
		if (IsDataValid())
		{
			SaveChanges();
			SetModified(FALSE,FALSE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::SaveChanges()
{
	// should be overridden by subclasses
}
////////////////////////////////////////////////////////////////////////////
void CSVPage::CancelChanges()
{
	// should be overridden by subclasses
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::DisableAll()
{
	CWnd* pItem = GetWindow(GW_CHILD);

	while (pItem && IsChild(pItem) ) 
	{
		// Haben wir noch ein Child?
		pItem->EnableWindow(FALSE);
		pItem = pItem->GetNextWindow();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSVPage::ActivateTooltips(BOOL bActivate)
{
	if (WK_IS_WINDOW(&m_ToolTip))
	{
		m_ToolTip.Activate(bActivate);
		SendMessageToDescendants(m_sEnableTooltipMessage,bActivate);
	}
}
/////////////////////////////////////////////////////////////////////////////

