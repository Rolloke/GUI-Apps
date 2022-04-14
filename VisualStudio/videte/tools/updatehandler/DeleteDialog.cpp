// DeleteDialog.cpp : implementation file
//

#include "stdafx.h"
#include "UpdateHandler.h"
#include "DeleteDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDeleteDialog dialog


CDeleteDialog::CDeleteDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CDeleteDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDeleteDialog)
	m_sDelete = _T("");
	//}}AFX_DATA_INIT
	m_iType = 0;
}


void CDeleteDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDeleteDialog)
	DDX_Text(pDX, IDC_EDIT_FILE_SOURCE, m_sDelete);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDeleteDialog, CDialog)
	//{{AFX_MSG_MAP(CDeleteDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDeleteDialog message handlers

void CDeleteDialog::OnOK() 
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
	
	CDialog::OnOK();
}
