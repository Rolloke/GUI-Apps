// DlgOptions.cpp : implementation file
//

#include "stdafx.h"
#include "IdipClient.h"
#include "DlgOptions.h"
#include "DlgAudio.h"
#include "CIPCAudioIdipClient.h"
#include "Server.h"
#include "IdipClientDoc.h"
#include "MainFrm.h"
#include "CipcMediaRecord.h"
#include "ViewCamera.h"
#include ".\dlgoptions.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgOptions dialog


CDlgOptions::CDlgOptions(CWnd* pParent /*=NULL*/)
	: CSkinDialog(CDlgOptions::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgOptions)
	m_b1PlusAlarm = FALSE;
	m_bAudibleAlarm = FALSE;
	m_bFullScreenAlarm = FALSE;
	m_bFirstCam = 0;
	//}}AFX_DATA_INIT

	m_nInitToolTips = FALSE;
	if (UseOEMSkin())
	{
		CreateTransparent(StyleBackGroundColorChangeSimple, m_BaseColor);
		// MoveOnClientAreaClick(false);
	}
}


void CDlgOptions::DoDataExchange(CDataExchange* pDX)
{
	int iFirstCam;
	switch (m_bFirstCam)
	{
		case TRUE    : iFirstCam = 0; break;
		case ALL_CAMS: iFirstCam = 1; break;
		case NO_CAM  : iFirstCam = -1; break;
	}

	CSkinDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgOptions)
	DDX_Check(pDX, IDC_CHECK_1PLUS_ALARM, m_b1PlusAlarm);
	DDX_Check(pDX, IDC_CHECK_AUDIBLE_ALARM, m_bAudibleAlarm);
	DDX_Check(pDX, IDC_CHECK_FULL_ALARM, m_bFullScreenAlarm);
	DDX_Radio(pDX, IDC_RADIO_FIRST_CAM, iFirstCam);
	//}}AFX_DATA_MAP

	switch (iFirstCam)
	{
		case 0: m_bFirstCam = TRUE; break;
		case 1: m_bFirstCam = ALL_CAMS; break;
		case -1: m_bFirstCam = NO_CAM; break;
	}
}


BEGIN_MESSAGE_MAP(CDlgOptions, CSkinDialog)
	//{{AFX_MSG_MAP(CDlgOptions)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgOptions message handlers

BOOL CDlgOptions::OnInitDialog()
{
	CSkinDialog::OnInitDialog();
	AutoCreateSkinButtons();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
