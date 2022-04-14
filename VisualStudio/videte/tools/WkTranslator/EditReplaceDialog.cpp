// EditReplaceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "wktranslator.h"
#include "EditReplaceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditReplaceDialog dialog

extern CWkTranslatorApp theApp;

CEditReplaceDialog::CEditReplaceDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CEditReplaceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEditReplaceDialog)
	m_sFind = _T("");
	m_sReplace = _T("");
	//}}AFX_DATA_INIT
}


void CEditReplaceDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditReplaceDialog)
	DDX_Control(pDX, IDC_EDIT_REPLACE, m_cReplace);
	DDX_Control(pDX, IDC_EDIT_FIND, m_cFind);
	DDX_Text(pDX, IDC_EDIT_FIND, m_sFind);
	DDX_Text(pDX, IDC_EDIT_REPLACE, m_sReplace);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditReplaceDialog, CDialog)
	//{{AFX_MSG_MAP(CEditReplaceDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditReplaceDialog message handlers

BOOL CEditReplaceDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();

	theApp.SetFontForChildren(m_hWnd);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
