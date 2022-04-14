// AboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "RemoteCurveControl.h"
#include "AboutDlg.h"

 
// CAboutDlg dialog
 
IMPLEMENT_DYNAMIC(CAboutDlg, CDialog)
CAboutDlg::CAboutDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAboutDlg::IDD, pParent)
{
}

CAboutDlg::~CAboutDlg()
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CAboutDlg message handlers

BOOL CAboutDlg::OnInitDialog()
{
    CDialog::OnInitDialog();

    CString s;
    s.LoadString(IDS_EXSAMPLE_TEXT);
    s.Replace("<br>", "\r\n");
    SetDlgItemText(IDC_EDIT_SAMPLE_TXT, s);

    return TRUE;  // return TRUE unless you set the focus to a control
    // EXCEPTION: OCX Property Pages should return FALSE
}
