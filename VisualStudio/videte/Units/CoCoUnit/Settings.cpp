/////////////////////////////////////////////////////////////////////////////
// PROJECT:		CoCoUnit
// FILE:		$Workfile: Settings.cpp $
// ARCHIVE:		$Archive: /Project/Units/CoCoUnit/Settings.cpp $
// CHECKIN:		$Date: 5.08.98 9:43 $
// VERSION:		$Revision: 4 $
// LAST CHANGE:	$Modtime: 5.08.98 9:43 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "win16_32.h"
#include "cocounitapp.h"
#include "ccoco.h"
#include "cmegra.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CSettings::m_bOK = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CSettings dialog

CSettings::CSettings(CCoCo* pCoCo, CMegra* pMegra, CWnd* pParent)
	: CDialog(CSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	if (!m_bOK)
	{
		// TRACE("CSettings::CSettings\n");
		m_pCoCo  = pCoCo;
		m_pMegra = pMegra;

		if (!Create(CSettings::IDD, pParent))
			return;

		m_bOK = TRUE;
	}
}

/////////////////////////////////////////////////////////////////////////////
CSettings::~CSettings()
{
   // TRACE("CSettings::~CSettings\n");
	m_bOK = FALSE;
}

void CSettings::Dump( CDumpContext &dc ) const
{
	 CObject::Dump( dc );
	 dc << "CSettings = ";
}
/////////////////////////////////////////////////////////////////////////////
void CSettings::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettings)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSettings, CDialog)
	//{{AFX_MSG_MAP(CSettings)
	ON_BN_CLICKED(IDC_ACQUISITION, OnAcquisition)
	ON_BN_CLICKED(IDC_DELAYS, OnDelays)
	ON_BN_CLICKED(IDC_LUM, OnLuminance)
	ON_BN_CLICKED(IDC_POSITION, OnPosition)
	ON_BN_CLICKED(IDC_PLL, OnPll)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettings message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	if (m_pMegra)
	{
		GetDlgItem(IDC_POSITION)->EnableWindow(TRUE);
		GetDlgItem(IDC_ACQUISITION)->EnableWindow(TRUE);
		GetDlgItem(IDC_DELAYS)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_POSITION)->EnableWindow(FALSE);
		GetDlgItem(IDC_ACQUISITION)->EnableWindow(FALSE);
		GetDlgItem(IDC_DELAYS)->EnableWindow(FALSE);
	}

/* Diese Einstellungen erfolgen nun über den SecurityExplorer
	if (m_pCoCo)
		GetDlgItem(IDC_LUM)->EnableWindow(TRUE);
	else
		GetDlgItem(IDC_LUM)->EnableWindow(FALSE);
*/

	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
	
	// TODO: Add extra initialization here
	ShowWindow(SW_SHOW);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CSettings::OnAcquisition() 
{
	if (m_pMegra)
		m_pMegra->ActivateAcquisitionDialog(m_hWnd);
	
}

/////////////////////////////////////////////////////////////////////////////
void CSettings::OnDelays() 
{
	if (m_pMegra)
		m_pMegra->ActivateDelayDialog(m_hWnd);
	
}

/////////////////////////////////////////////////////////////////////////////
void CSettings::OnLuminance() 
{
	if (m_pCoCo)
		m_pCoCo->DlgLuminance();
	
}

/////////////////////////////////////////////////////////////////////////////
void CSettings::OnPosition() 
{
	if (m_pMegra)
		m_pMegra->ActivatePosDialog(m_hWnd);
	
}

/////////////////////////////////////////////////////////////////////////////
void CSettings::OnPll() 
{
	if (m_pMegra)
		m_pMegra->ActivatePllDialog(m_hWnd);
}

void CSettings::OnCancel() 
{
	// TODO: Add extra cleanup here
	CDialog::OnCancel();
	DestroyWindow();

	delete this;
}

void CSettings::OnOK() 
{
	// TODO: Add extra validation here
	if (m_pCoCo)
		m_pCoCo->SaveConfig();

	if (m_pMegra)
		m_pMegra->SaveConfig();

	CDialog::OnOK();
	DestroyWindow();

	delete this;
}

