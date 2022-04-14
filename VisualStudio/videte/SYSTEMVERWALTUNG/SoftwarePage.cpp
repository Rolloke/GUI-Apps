// SoftwarePage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SoftwarePage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC(CSoftwarePage, CSVPage)
/////////////////////////////////////////////////////////////////////////////
// CSoftwarePage dialog
CSoftwarePage::CSoftwarePage(CSVView*  pParent) : CSVPage(CSoftwarePage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CSoftwarePage)
	m_sProduct = _T("");
	m_sVersion = _T("");
	m_strText = _T("");
	//}}AFX_DATA_INIT
}
void CSoftwarePage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoftwarePage)
	DDX_Text(pDX, IDC_PRODUCT, m_sProduct);
	DDX_Text(pDX, IDC_VERSION, m_sVersion);
	DDX_Text(pDX, IDC_SOFT_TEXT, m_strText);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSoftwarePage, CSVPage)
	//{{AFX_MSG_MAP(CSoftwarePage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSoftwarePage message handlers

BOOL CSoftwarePage::OnInitDialog() 
{
	m_sProduct = COemGuiApi::CalcProductName();
	m_sVersion = GetDongle().GetVersionString();
	CSVPage::OnInitDialog();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSoftwarePage::CanNew()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSoftwarePage::CanDelete()
{
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
void CSoftwarePage::SaveChanges()
{
}
/////////////////////////////////////////////////////////////////////////////
void CSoftwarePage::CancelChanges()
{
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSoftwarePage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////


void CSoftwarePage::Create()
{
	CSVPage::Create(IDD,(CWnd*)m_pParent);
}
