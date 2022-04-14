// HTMLlinkProperties.cpp : implementation file
//

#include "stdafx.h"
#include "vision.h"
#include "HTMLlinkProperties.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHTMLlinkProperties dialog


CHTMLlinkProperties::CHTMLlinkProperties(CWnd* pParent /*=NULL*/)
	: CDialog(CHTMLlinkProperties::IDD, pParent)
{
	//{{AFX_DATA_INIT(CHTMLlinkProperties)
	m_sHTMLlinkInfo = _T("");
	//}}AFX_DATA_INIT
}


void CHTMLlinkProperties::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CHTMLlinkProperties)
	DDX_Control(pDX, IDOK, m_cOK);
	DDX_Text(pDX, IDC_EDT_HL_INFO, m_sHTMLlinkInfo);
	DDV_MinChars(pDX, m_sHTMLlinkInfo, 0, IDC_TXT_HL_INFO);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CHTMLlinkProperties, CDialog)
	//{{AFX_MSG_MAP(CHTMLlinkProperties)
	ON_EN_CHANGE(IDC_EDT_HL_INFO, OnChangeEdtHlInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHTMLlinkProperties message handlers

BOOL CHTMLlinkProperties::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_cOK.EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CHTMLlinkProperties::OnChangeEdtHlInfo() 
{
	m_cOK.EnableWindow(TRUE);	
}
