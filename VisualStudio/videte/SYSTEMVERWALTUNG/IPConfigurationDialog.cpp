// IPConfigurationDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "IPConfigurationDialog.h"
#include ".\ipconfigurationdialog.h"


/////////////////////////////////////////////////////////////////////////////
// CIPConfigurationDialog dialog
IMPLEMENT_DYNAMIC(CIPConfigurationDialog, CDialog)
/////////////////////////////////////////////////////////////////////////////
CIPConfigurationDialog::CIPConfigurationDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CIPConfigurationDialog::IDD, pParent)
{
	m_nType = 0;
	m_sIPaddress = _T("");
	m_iPort = 1;
}
/////////////////////////////////////////////////////////////////////////////
CIPConfigurationDialog::~CIPConfigurationDialog()
{
}
/////////////////////////////////////////////////////////////////////////////
void CIPConfigurationDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CIPConfigurationDialog)
	DDX_Text(pDX, IDC_IP_EDT_ADDRESS, m_sIPaddress);
	DDV_Validate_IP_adr(pDX, m_sIPaddress, IDC_IP_TXT_ADDRESS);
	DDX_Radio(pDX, IDC_IP_RD_UDP, m_nType);
	DDX_Text(pDX, IDC_IP_EDT_PORT, m_iPort);
	DDV_MinMaxInt(pDX, m_iPort, 1, 65535);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CIPConfigurationDialog, CDialog)
    //{{AFX_MSG_MAP(CIPConfigurationDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_IP_RD_UDP, OnBnClickedIpRdUdp)
	ON_BN_CLICKED(IDC_IP_RD_TCP, OnBnClickedIpRdUdp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CIPConfigurationDialog message handlers
void CIPConfigurationDialog::OnBnClickedOk()
{
	OnOK();
}
/////////////////////////////////////////////////////////////////////////////
void CIPConfigurationDialog::OnBnClickedIpRdUdp()
{
	CDataExchange dx(this, TRUE);
	DDX_Radio(&dx, IDC_IP_RD_UDP, m_nType);
	GetDlgItem(IDC_IP_EDT_PORT)->EnableWindow((m_nType == 0) ? TRUE:FALSE);
}
/////////////////////////////////////////////////////////////////////////////

BOOL CIPConfigurationDialog::OnInitDialog()
{
	CDialog::OnInitDialog();
	OnBnClickedIpRdUdp();
	GotoDlgCtrl(GetDlgItem(IDC_IP_EDT_PORT));

	return FALSE;
}
