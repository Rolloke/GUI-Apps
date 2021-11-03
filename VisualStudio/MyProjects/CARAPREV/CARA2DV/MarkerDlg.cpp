// MarkerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CARA2DV.h"
#include "MarkerDlg.h"
#include ".\markerdlg.h"


// CMarkerDlg dialog

IMPLEMENT_DYNAMIC(CMarkerDlg, CDialog)
CMarkerDlg::CMarkerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CMarkerDlg::IDD, pParent)
{
   m_dMin = 0;
   m_dMax = 0;
   m_dMarker = 0;
}

CMarkerDlg::~CMarkerDlg()
{
}

void CMarkerDlg::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   DDX_Text(pDX, IDC_EDT_MARKER, m_dMarker);
   DDV_MinMaxDouble(pDX, m_dMarker, m_dMin, m_dMax);
}


BEGIN_MESSAGE_MAP(CMarkerDlg, CDialog)
   ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CMarkerDlg message handlers

void CMarkerDlg::OnBnClickedOk()
{
   // TODO: Add your control notification handler code here
   OnOK();
}
