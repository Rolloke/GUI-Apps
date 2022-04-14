// CmdLinParams.cpp : implementation file
//

#include "stdafx.h"
#include "updatehandler.h"
#include "CmdLinParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCmdLineParamDlg dialog


CCmdLineParamDlg::CCmdLineParamDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCmdLineParamDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCmdLineParamDlg)
	m_sCmdLine = _T("");
	m_nWaitTime = 2000;
	//}}AFX_DATA_INIT
}


void CCmdLineParamDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCmdLineParamDlg)
	DDX_Text(pDX, IDC_EDIT_CMD_LINE_PARAMS, m_sCmdLine);
	if (m_nWaitTime != 0)
	{
		DDX_Text(pDX, IDC_EDT_WAIT_TIME, m_nWaitTime);
		if (m_nWaitTime != -1)
		{
			DDV_MinMaxInt(pDX, m_nWaitTime, 500, 90000);
		}
	}
	//}}AFX_DATA_MAP
}

BOOL CCmdLineParamDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	if (!m_sHeadline.IsEmpty())
	{
		SetWindowText(m_sHeadline);
		GetDlgItem(IDC_TXT_WAIT_TIME)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDT_WAIT_TIME)->ShowWindow(SW_SHOW);
	}
	GotoDlgCtrl(GetDlgItem(IDC_EDIT_CMD_LINE_PARAMS));
	if (m_nWaitTime == 0)
	{
		GetDlgItem(IDC_EDT_WAIT_TIME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TXT_WAIT_TIME)->ShowWindow(SW_HIDE);
	}

	return FALSE;
}

BEGIN_MESSAGE_MAP(CCmdLineParamDlg, CDialog)
	//{{AFX_MSG_MAP(CCmdLineParamDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCmdLineParamDlg message handlers
