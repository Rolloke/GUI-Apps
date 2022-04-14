// BeforeBackupDialog.cpp : implementation file
//

#include "stdafx.h"
#include "securitylauncher.h"
#include "BeforeBackupDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBeforeBackupDialog dialog


CBeforeBackupDialog::CBeforeBackupDialog(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CBeforeBackupDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBeforeBackupDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CBeforeBackupDialog::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBeforeBackupDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBeforeBackupDialog, CSkinDialog)
	//{{AFX_MSG_MAP(CBeforeBackupDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBeforeBackupDialog message handlers

BOOL CBeforeBackupDialog::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();
	AutoCreateSkinStatic();
	
	CenterWindow(CWnd::GetDesktopWindow());
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
