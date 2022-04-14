// DlgHTMLlink.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgHTMLlink.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgHTMLlink dialog


CDlgHTMLlink::CDlgHTMLlink(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CDlgHTMLlink::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgHTMLlink)
	m_sHTMLlinkInfo = _T("");
	//}}AFX_DATA_INIT

	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundColorChangeSimple, m_BaseColor);
		// MoveOnClientAreaClick(false);
	}
}


void CDlgHTMLlink::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgHTMLlink)
	DDX_Control(pDX, IDOK, m_cOK);
	DDX_Text(pDX, IDC_EDT_HL_INFO, m_sHTMLlinkInfo);
	DDV_MinChars(pDX, m_sHTMLlinkInfo, 0, IDC_TXT_HL_INFO);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgHTMLlink, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgHTMLlink)
	ON_EN_CHANGE(IDC_EDT_HL_INFO, OnChangeEdtHlInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgHTMLlink message handlers

BOOL CDlgHTMLlink::OnInitDialog() 
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();
	AutoCreateSkinStatic();
	AutoCreateSkinEdit();
	m_cOK.EnableWindow(FALSE);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDlgHTMLlink::OnChangeEdtHlInfo() 
{
	m_cOK.EnableWindow(TRUE);	
}
