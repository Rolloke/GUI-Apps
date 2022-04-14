// ServiceDialog.cpp : implementation file
//

#include "stdafx.h"
#include "securitylauncher.h"
#include "ServiceDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CServiceDialog dialog


CServiceDialog::CServiceDialog(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CServiceDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CServiceDialog)
	m_sService = _T("");
	//}}AFX_DATA_INIT
	m_bDone = FALSE;
	m_bShowSuccess = FALSE;
	m_bSuccess = FALSE;
}


void CServiceDialog::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServiceDialog)
	DDX_Control(pDX, IDC_NO_SUCCESS, m_txtNoSuccess);
	DDX_Control(pDX, IDC_SUCCESS, m_txtSuccess);
	DDX_Control(pDX, IDC_TXT1, m_ToDoTxt);
	DDX_Control(pDX, IDC_TXT2, m_DoneTxt);
	DDX_Text(pDX, IDC_EDIT_SERVICE, m_sService);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CServiceDialog, CSkinDialog)
	//{{AFX_MSG_MAP(CServiceDialog)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CServiceDialog message handlers

BOOL CServiceDialog::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();
	AutoCreateSkinStatic();
	
	CenterWindow(GetDesktopWindow());

	m_ToDoTxt.ShowWindow((!m_bDone && !m_bShowSuccess)?SW_SHOW:SW_HIDE);
	m_DoneTxt.ShowWindow((m_bDone && !m_bShowSuccess)?SW_SHOW:SW_HIDE);
	m_txtSuccess.ShowWindow((m_bSuccess && m_bShowSuccess)?SW_SHOW:SW_HIDE);
	m_txtNoSuccess.ShowWindow((!m_bSuccess && m_bShowSuccess)?SW_SHOW:SW_HIDE);

	SetTimer(1,((m_bShowSuccess||m_bDone)?10:30)*1000,NULL);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CServiceDialog::OnTimer(UINT nIDEvent) 
{
	KillTimer(1);
	OnCancel();
}
