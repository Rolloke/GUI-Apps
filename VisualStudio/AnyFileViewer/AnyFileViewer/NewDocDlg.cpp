// NewDocDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AnyFileViewer.h"
#include "NewDocDlg.h"
#include ".\newdocdlg.h"


// CNewDocDlg dialog

IMPLEMENT_DYNAMIC(CNewDocDlg, CDialog)
CNewDocDlg::CNewDocDlg(CWnd* pParent /*=NULL*/)
    : CDialog(CNewDocDlg::IDD, pParent)
   , m_nNewDocSize(1024)
{
}

CNewDocDlg::~CNewDocDlg()
{
}

void CNewDocDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_EDT_DOC_SIZE, m_nNewDocSize);
   DDV_MinMaxUInt(pDX, m_nNewDocSize, 1, 524288000);
}


BEGIN_MESSAGE_MAP(CNewDocDlg, CDialog)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
   ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
END_MESSAGE_MAP()


// CNewDocDlg message handlers

void CNewDocDlg::OnBnClickedOk()
{
   OnOK();
}

void CNewDocDlg::OnBnClickedCancel()
{
   OnCancel();
}
