// FindDialog.cpp : implementation file
//

#include "stdafx.h"
#include "wktranslator.h"
#include "FindDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindDialog dialog
extern CWkTranslatorApp theApp;


CFindDialog::CFindDialog(const CString& sSearchFor, CWnd* pParent /*=NULL*/)
	: CDialog(CFindDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindDialog)
	m_sFind = sSearchFor;
	m_bCaseSensitive = FALSE;
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
void CFindDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindDialog)
	DDX_Control(pDX, IDC_EDIT_FIND, m_cFind);
	DDX_Text(pDX, IDC_EDIT_FIND, m_sFind);
	DDX_Check(pDX, IDC_CHECK_CASE_SENSITIVE, m_bCaseSensitive);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CFindDialog, CDialog)
	//{{AFX_MSG_MAP(CFindDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CFindDialog message handlers

BOOL CFindDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	theApp.SetFontForChildren(m_hWnd);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
