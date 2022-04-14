// SendDialog.cpp : implementation file
//

#include "stdafx.h"
#include "UpdateHandler.h"
#include "SendDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szFilter[] = _T("Programme (*.exe)|*.exe|Bibliotheken (*.dll)|*.dll|Virtuelle Gerätetreiber (*.vxd)|*.vxd|Alle Dateien (*.*)|*.*||");

/////////////////////////////////////////////////////////////////////////////
// CSendDialog dialog


CSendDialog::CSendDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSendDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSendDialog)
	m_DestinationFileName = _T("");
	m_Source = _T("");
	m_bReboot = FALSE;
	//}}AFX_DATA_INIT
	m_iType = 0;
}


void CSendDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSendDialog)
	DDX_Text(pDX, IDC_EDIT_FILE_SOURCE, m_DestinationFileName);
	DDX_Text(pDX, IDC_STATIC_SOURCE, m_Source);
	DDX_Check(pDX, IDC_CHECK_REBOOT, m_bReboot);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSendDialog, CDialog)
	//{{AFX_MSG_MAP(CSendDialog)
	ON_BN_CLICKED(IDC_OPEN, OnOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSendDialog message handlers

BOOL CSendDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSendDialog::OnOK() 
{
	// Destination: 0=AppDir, 1=Windows, 2=Windows/System, 3=Root
	//				4=use sFileName
	if ( ((CButton*)GetDlgItem(IDC_RADIO_APPDIR))->GetCheck())
		m_iType = 0;
	if ( ((CButton*)GetDlgItem(IDC_RADIO_WINDOWS))->GetCheck())
		m_iType = 1;
	if ( ((CButton*)GetDlgItem(IDC_RADIO_SYSTEM))->GetCheck())
		m_iType = 2;
	if ( ((CButton*)GetDlgItem(IDC_RADIO_ROOT))->GetCheck())
		m_iType = 3;
	if ( ((CButton*)GetDlgItem(IDC_RADIO_FULL))->GetCheck())
		m_iType = 4;
	
	UpdateData();

	if (!m_Source.IsEmpty() && !m_DestinationFileName.IsEmpty())
	{
		CDialog::OnOK();
	}
}

void CSendDialog::OnOpen() 
{
	CFileDialog dlg(TRUE,NULL,NULL,
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					szFilter,this);
	
	UpdateData();

	if (IDOK==dlg.DoModal())
	{
		m_Source = dlg.GetPathName();
		if (m_DestinationFileName.IsEmpty())
		{
			m_DestinationFileName = m_Source;
		}
		UpdateData(FALSE);
	}
}
