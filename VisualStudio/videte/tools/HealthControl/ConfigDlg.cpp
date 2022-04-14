// ConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "HealthControl.h"
#include "ConfigDlg.h"


// CConfigDlg dialog


IMPLEMENT_DYNAMIC(CConfigDlg, CDialog)
CConfigDlg::CConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CConfigDlg::IDD, pParent)
{
	m_nDeviceNum = 0;
	m_pbSendRTE = NULL;
	m_pnDevice = NULL;
	m_pnMinValue = NULL;
	m_pnMaxValue = NULL;
	m_pfMinValue = NULL;
	m_pfMaxValue = NULL;
}

CConfigDlg::~CConfigDlg()
{
}

void CConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	if (m_pnDevice)
	{
		DDX_CBIndex(pDX, IDC_COMBO_DEVICE, *m_pnDevice);
	}
	if (m_pnMinValue)
	{
		DDX_Text(pDX, IDC_EDIT_MIN_VALUE, *m_pnMinValue);
	}
	if (m_pnMaxValue)
	{
		DDX_Text(pDX, IDC_EDIT_MAX_VALUE, *m_pnMaxValue);
	}
	if (m_pfMinValue)
	{
		DDX_Text(pDX, IDC_EDIT_MIN_VALUE, *m_pfMinValue);
	}
	if (m_pfMaxValue)
	{
		DDX_Text(pDX, IDC_EDIT_MAX_VALUE, *m_pfMaxValue);
	}
	if (m_pbSendRTE)
	{
		BOOL bCheck = FALSE;
		DWORD dwFlags = 1 << m_nDeviceNum;
		if (pDX->m_bSaveAndValidate == FALSE)
		{
			bCheck = dwFlags & *m_pbSendRTE ? TRUE : FALSE;
		}
		DDX_Check(pDX, IDC_CHECK_SEND_RTE, bCheck);
		if (pDX->m_bSaveAndValidate == TRUE)
		{
			if (bCheck) *m_pbSendRTE |= dwFlags;
			else		*m_pbSendRTE &= ~dwFlags;
		}
	}
	DDX_Control(pDX, IDC_COMBO_DEVICE, m_cDevices);
}


BEGIN_MESSAGE_MAP(CConfigDlg, CDialog)
END_MESSAGE_MAP()


// CConfigDlg message handlers

BOOL CConfigDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	SetDlgItemText(IDC_TXT_UNIT, m_sUnit);
	CString sText;
	GetWindowText(sText);
	sText += m_sValue;
	SetWindowText(sText);
	if (m_pnDevice == NULL)
	{
		m_cDevices.EnableWindow(FALSE);
	}
	else
	{
		CString str;
		str.LoadString(IDS_DEVICE_UNKNOWN);
		m_cDevices.AddString(str);
		str.LoadString(IDS_DEVICE_HARDDISK);
		m_cDevices.AddString(str);
		str.LoadString(IDS_DEVICE_CPU);
		m_cDevices.AddString(str);
		str.LoadString(IDS_DEVICE_POWER_SUPPLY);
		m_cDevices.AddString(str);
		str.LoadString(IDS_DEVICE_HW_BOARD);
		m_cDevices.AddString(str);
		str.LoadString(IDS_DEVICE_CASE);
		m_cDevices.AddString(str);
		str.LoadString(IDS_DEVICE_SYSTEM);
		m_cDevices.AddString(str);
		m_cDevices.SetCurSel(*m_pnDevice);
	}
	if (m_pbSendRTE == NULL)
	{
		GetDlgItem(IDC_CHECK_SEND_RTE)->EnableWindow(FALSE);
	}
	if (m_pnMinValue == NULL && m_pfMinValue == NULL)
	{
		GetDlgItem(IDC_EDIT_MIN_VALUE)->EnableWindow(FALSE);
	}
	if (m_pnMaxValue == NULL && m_pfMaxValue == NULL)
	{
		GetDlgItem(IDC_EDIT_MAX_VALUE)->EnableWindow(FALSE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void CConfigDlg::OnOK()
{
	// TODO: Add your specialized code here and/or call the base class

	CDialog::OnOK();
}
