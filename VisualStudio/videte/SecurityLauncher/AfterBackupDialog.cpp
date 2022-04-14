// AfterBackupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "securitylauncher.h"
#include "AfterBackupDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAfterBackupDialog dialog


CAfterBackupDialog::CAfterBackupDialog(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CAfterBackupDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAfterBackupDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CAfterBackupDialog::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAfterBackupDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAfterBackupDialog, CSkinDialog)
	//{{AFX_MSG_MAP(CAfterBackupDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAfterBackupDialog message handlers

BOOL CAfterBackupDialog::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();
	AutoCreateSkinStatic();
	
	CenterWindow(CWnd::GetDesktopWindow());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
