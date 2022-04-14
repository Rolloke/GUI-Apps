// StationNameDialog.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgStationName.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgStationName dialog


CDlgStationName::CDlgStationName(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CDlgStationName::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgStationName)
	m_sStationName = _T("");
	//}}AFX_DATA_INIT

	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundColorChangeSimple, m_BaseColor);
		// MoveOnClientAreaClick(false);
	}
}


void CDlgStationName::DoDataExchange(CDataExchange* pDX)
{
	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgStationName)
	DDX_Text(pDX, IDC_EDIT_NAME, m_sStationName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgStationName, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgStationName)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStationName message handlers

BOOL CDlgStationName::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinEdit();
	AutoCreateSkinStatic();
	AutoCreateSkinButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
