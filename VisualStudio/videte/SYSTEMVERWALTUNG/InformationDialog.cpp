// InformationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "InformationDialog.h"
#include ".\informationdialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInformationDialog dialog


CInformationDialog::CInformationDialog(CWnd* pParent /*=NULL*/)
	: CWK_Dialog(CInformationDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInformationDialog)
	m_sEditor = _T("");
	m_sInformation = _T("");
	m_sObject = _T("");
	//}}AFX_DATA_INIT
}


void CInformationDialog::DoDataExchange(CDataExchange* pDX)
{
	CWK_Dialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInformationDialog)
	DDX_Text(pDX, IDC_EDITOR, m_sEditor);
	DDX_Text(pDX, IDC_INFORMATION, m_sInformation);
	DDX_Text(pDX, IDC_TXT_INPUT, m_sObject);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInformationDialog, CWK_Dialog)
	//{{AFX_MSG_MAP(CInformationDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInformationDialog message handlers


BOOL CInformationDialog::OnInitDialog()
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
