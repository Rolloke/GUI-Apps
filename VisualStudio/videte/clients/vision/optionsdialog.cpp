// OptionsDialog.cpp : implementation file
//

#include "stdafx.h"
#include "vision.h"
#include "OptionsDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog dialog


COptionsDialog::COptionsDialog(CWnd* pParent /*=NULL*/)
	: CDialog(COptionsDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionsDialog)
	m_b1PlusAlarm = FALSE;
	m_bAudibleAlarm = FALSE;
	m_bFullScreenAlarm = FALSE;
	m_iFirstCam = 0;
	//}}AFX_DATA_INIT
}


void COptionsDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionsDialog)
	DDX_Check(pDX, IDC_CHECK_1PLUS_ALARM, m_b1PlusAlarm);
	DDX_Check(pDX, IDC_CHECK_AUDIBLE_ALARM, m_bAudibleAlarm);
	DDX_Check(pDX, IDC_CHECK_FULL_ALARM, m_bFullScreenAlarm);
	DDX_Radio(pDX, IDC_RADIO_FIRST_CAM, m_iFirstCam);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionsDialog, CDialog)
	//{{AFX_MSG_MAP(COptionsDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionsDialog message handlers
