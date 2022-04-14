// ReceiveDialog.cpp : implementation file
//

#include "stdafx.h"
#include "UpdateHandler.h"
#include "ReceiveDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static char BASED_CODE szFilter[] = "Alle Dateien (*.*)|*.*||";

/////////////////////////////////////////////////////////////////////////////
// CReceiveDialog dialog


CReceiveDialog::CReceiveDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CReceiveDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CReceiveDialog)
	m_SourceFileName = _T("");
	m_Destination = _T("");
	//}}AFX_DATA_INIT
	m_iType = 0;
}


void CReceiveDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CReceiveDialog)
	DDX_Text(pDX, IDC_EDIT_FILE_SOURCE, m_SourceFileName);
	DDX_Text(pDX, IDC_STATIC_DESTINATION, m_Destination);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CReceiveDialog, CDialog)
	//{{AFX_MSG_MAP(CReceiveDialog)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
void CReceiveDialog::OnSave() 
{
	CFileDialog dlg(FALSE,NULL,NULL,
					OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					szFilter,this);
	
	UpdateData();

	if (IDOK==dlg.DoModal())
	{
		m_Destination = dlg.GetPathName();
		if (m_SourceFileName.IsEmpty())
		{
			m_SourceFileName = m_Destination;
		}
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CReceiveDialog::OnOK() 
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

	if (!m_Destination.IsEmpty()/* && !m_SourceFileName.IsEmpty()*/)
	{
		CDialog::OnOK();
	}
}

BOOL CReceiveDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	((CButton*)GetDlgItem(IDC_RADIO_APPDIR))->SetCheck(1);
	
	return TRUE;
}
