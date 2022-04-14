// Settings.cpp : implementation file
//

#include "stdafx.h"
#include "mmsystem.h"
#include "MiCoUnitApp.h"
#include "CMiCo.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BOOL CSettings::m_bOK = FALSE;

/////////////////////////////////////////////////////////////////////////////
// CSettings dialog

CSettings::CSettings(CMiCo* pMiCo, CWnd* pParent)
	: CDialog(CSettings::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSettings)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	if (!m_bOK)
	{
		// TRACE("CSettings::CSettings\n");
		m_pMiCo  = pMiCo;

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
	ON_BN_CLICKED(IDC_LUM, OnLuminance)
	ON_BN_CLICKED(IDC_COLOR, OnColor)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettings message handlers

/////////////////////////////////////////////////////////////////////////////
BOOL CSettings::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE); 
	
	// TODO: Add extra initialization here
	ShowWindow(SW_SHOW);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CSettings::OnLuminance() 
{
	if (m_pMiCo)
		m_pMiCo->DlgLuminance();
	
}

/////////////////////////////////////////////////////////////////////////////
void CSettings::OnCancel() 
{
	DestroyWindow();

	delete this;
}

/////////////////////////////////////////////////////////////////////////////
void CSettings::OnOK() 
{
	if (m_pMiCo)
		m_pMiCo->SaveConfig();

	DestroyWindow();

	delete this;
}				   

/////////////////////////////////////////////////////////////////////////////
void CSettings::OnColor() 
{
	if (m_pMiCo)
		m_pMiCo->DlgLuminance();
}
