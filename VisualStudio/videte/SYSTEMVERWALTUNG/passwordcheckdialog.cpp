// PasswordCheckDialog.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "PasswordCheckDialog.h"
#include ".\passwordcheckdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPasswordCheckDialog dialog


CPasswordCheckDialog::CPasswordCheckDialog(CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CPasswordCheckDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordCheckDialog)
	m_Password = _T("");
	//}}AFX_DATA_INIT
}


void CPasswordCheckDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordCheckDialog)
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_Password);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordCheckDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CPasswordCheckDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordCheckDialog message handlers

BOOL CPasswordCheckDialog::OnInitDialog()
{
	CWK_Dialog::OnInitDialog();

	if (CSkinDialog::SetChildWindowFont(m_hWnd))
	{
		if (m_ToolTip.m_hWnd)
		{
			m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
		}
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
