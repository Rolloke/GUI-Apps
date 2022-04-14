// SDIDefaultMicrolockDlg.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SDIDefaultMicrolockDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSDIDefaultMicrolockDlg dialog
CSDIDefaultMicrolockDlg::CSDIDefaultMicrolockDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSDIDefaultMicrolockDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSDIDefaultMicrolockDlg)
	m_iType = SDI_MICROLOCK_TYPE_SYSTEM1X;
	//}}AFX_DATA_INIT
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDefaultMicrolockDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSDIDefaultMicrolockDlg)
	DDX_Control(pDX, IDC_RADIO_DEFAULT_SDI_MICROLOCK_SYSTEM1, m_btnSystem1);
	DDX_Radio(pDX, IDC_RADIO_DEFAULT_SDI_MICROLOCK_SYSTEM1x, m_iType);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSDIDefaultMicrolockDlg, CDialog)
	//{{AFX_MSG_MAP(CSDIDefaultMicrolockDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
int CSDIDefaultMicrolockDlg::GetType() 
{
	return m_iType;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSDIDefaultMicrolockDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CSkinDialog::SetChildWindowFont(m_hWnd);
	
	m_btnSystem1.EnableWindow(FALSE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSDIDefaultMicrolockDlg::OnOK() 
{
	UpdateData();
	CDialog::OnOK();
}
