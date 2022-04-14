// InsertDialog.cpp : implementation file
//

#include "stdafx.h"
#include "WkWizard.h"
#include "InsertDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInsertDialog dialog


CInsertDialog::CInsertDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CInsertDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInsertDialog)
	m_Dest = _T("");
	m_Lang = _T("");
	m_Orig = _T("");
	//}}AFX_DATA_INIT
}


void CInsertDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInsertDialog)
	DDX_Text(pDX, IDC_STATIC_FILENAME, m_Dest);
	DDX_Text(pDX, IDC_EDIT_LANG, m_Lang);
	DDX_Text(pDX, IDC_STATIC_ORIGINAL, m_Orig);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInsertDialog, CDialog)
	//{{AFX_MSG_MAP(CInsertDialog)
	ON_BN_CLICKED(IDC_BUTTON_BROWSE, OnButtonBrowse)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInsertDialog message handlers
static char BASED_CODE szFilter[] = "Language Files (*.lng)|*.lng||";
void CInsertDialog::OnButtonBrowse() 
{
	CFileDialog dlg(TRUE, "*.lng", (const char*)m_Dir, OFN_HIDEREADONLY ,szFilter, this);

	if (IDOK==dlg.DoModal())
	{
		m_Dest = dlg.GetPathName();
		UpdateData(FALSE);
	}
}

BOOL CInsertDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_Dir += "*.lng";
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
