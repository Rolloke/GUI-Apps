// IPDlg.cpp : implementation file
//

#include "stdafx.h"
#include "SDITest.h"
#include "IPDlg.h"
#include ".\ipdlg.h"


// CIPDlg dialog

IMPLEMENT_DYNAMIC(CIPDlg, CDialog)
CIPDlg::CIPDlg(UINT iIPPort, CString sAddress, UINT iIPType)
	: CDialog(CIPDlg::IDD, NULL)
{
	m_iIPPort = iIPPort;
	m_iIPType = iIPType;
	m_sIPAddress = sAddress;
}

CIPDlg::~CIPDlg()
{
}

void CIPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX,IDC_COMBO_IP_TYPE,m_cbType);
	DDX_Control(pDX,IDC_IPADDRESS,m_ipCtrl);
	DDX_Text(pDX,IDC_EDIT_PORT,m_iIPPort);
	if (pDX->m_bSaveAndValidate)
	{
		BYTE b1,b2,b3,b4;
		b1 = b2 = b3 = b4 = 0;
		m_ipCtrl.GetAddress(b1,b2,b3,b4);
		m_sIPAddress.Format(_T("%d.%d.%d.%d"),b1,b2,b3,b4);
		int iCursel = m_cbType.GetCurSel();
		switch (iCursel)
		{
		case 0:
			m_iIPType = SOCK_DGRAM;
			break;
		case 1:
			m_iIPType = SOCK_STREAM;
			break;
		}
	}
	else
	{
		DWORD dw1,dw2,dw3,dw4;
		dw1 = dw2 = dw3 = dw4 = 0;

		if (4==_stscanf(m_sIPAddress,_T("%d.%d.%d.%d"),&dw1,&dw2,&dw3,&dw4))
		{
			m_ipCtrl.SetAddress((BYTE)dw1,(BYTE)dw2,(BYTE)dw3,(BYTE)dw4);
		}
		switch (m_iIPType)
		{
		case SOCK_DGRAM:
			m_cbType.SetCurSel(0);
			break;
		case SOCK_STREAM:
			m_cbType.SetCurSel(1);
			break;
		}
	}
}


BEGIN_MESSAGE_MAP(CIPDlg, CDialog)
END_MESSAGE_MAP()


// CIPDlg message handlers

BOOL CIPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	m_cbType.AddString(_T("UDP"));
	m_cbType.AddString(_T("TCP"));

	UpdateData(FALSE);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
