// MultiMonitorPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "MultiMonitorPage.h"
#include "MainFrm.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// monitor radio control defines 
#define RADIO_MONITOR_1		0
#define RADIO_MONITOR_2		1	
#define RADIO_MONITOR_1_2	2

/////////////////////////////////////////////////////////////////////////////
// CMultiMonitorPage dialog
CMultiMonitorPage::CMultiMonitorPage(CSVView* pParent)
	: CSVPage(CMultiMonitorPage::IDD)
{
	//{{AFX_DATA_INIT(CMultiMonitorPage)
	m_iVision = RADIO_MONITOR_1;
	m_iRecherche = RADIO_MONITOR_1;
	m_iSystVerw = RADIO_MONITOR_1;
	//}}AFX_DATA_INIT
	m_pParent = pParent;
	Create(IDD,pParent);
}
/////////////////////////////////////////////////////////////////////////////
void CMultiMonitorPage::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMultiMonitorPage)
	DDX_Control(pDX, IDC_GRP_MULTI_MONITOR_VISION, m_grpVision);
	DDX_Control(pDX, IDC_GRP_MULTI_MONITOR_SUPERVISOR, m_grpSupervisor);
	DDX_Control(pDX, IDC_GRP_MULTI_MONITOR_RECHERCHE, m_grpRecherche);
	DDX_Control(pDX, IDC_RADIO_MULTI_MONITOR_VISION_1, m_radioVision_1);
	DDX_Control(pDX, IDC_RADIO_MULTI_MONITOR_VISION_2, m_radioVision_2);
	DDX_Control(pDX, IDC_RADIO_MULTI_MONITOR_VISION_1_2, m_radioVision_1_2);
	DDX_Control(pDX, IDC_RADIO_MULTI_MONITOR_RECHERCHE_1, m_radioRecherche_1);
	DDX_Control(pDX, IDC_RADIO_MULTI_MONITOR_RECHERCHE_2, m_radioRecherche_2);
	DDX_Control(pDX, IDC_RADIO_MULTI_MONITOR_SV_1, m_radioSupervisor_1);
	DDX_Control(pDX, IDC_RADIO_MULTI_MONITOR_SV_2, m_radioSupervisor_2);
	DDX_Radio(pDX, IDC_RADIO_MULTI_MONITOR_VISION_1, m_iVision);
	DDX_Radio(pDX, IDC_RADIO_MULTI_MONITOR_RECHERCHE_1, m_iRecherche);
	DDX_Radio(pDX, IDC_RADIO_MULTI_MONITOR_SV_1, m_iSystVerw);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMultiMonitorPage, CSVPage)
	//{{AFX_MSG_MAP(CMultiMonitorPage)
	ON_BN_CLICKED(IDC_RADIO_MULTI_MONITOR_VISION_1, OnChanged)
	ON_BN_CLICKED(IDC_RADIO_MULTI_MONITOR_VISION_2, OnChanged)
	ON_BN_CLICKED(IDC_RADIO_MULTI_MONITOR_VISION_1_2, OnChanged)
	ON_BN_CLICKED(IDC_RADIO_MULTI_MONITOR_RECHERCHE_1, OnChanged)
	ON_BN_CLICKED(IDC_RADIO_MULTI_MONITOR_RECHERCHE_2, OnChanged)
	ON_BN_CLICKED(IDC_RADIO_MULTI_MONITOR_RECHERCHE_1_2, OnChanged)
	ON_BN_CLICKED(IDC_RADIO_MULTI_MONITOR_SV_1, OnChangedSV)
	ON_BN_CLICKED(IDC_RADIO_MULTI_MONITOR_SV_2, OnChangedSV)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CMultiMonitorPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	// Set OEM specific group names
	CString sText;
// Replace Vision with idipClient, move Supervisor and hide Recherche
#if _MFC_VER >= 0x0710
	sText = COemGuiApi::GetApplicationName(WAI_IDIP_CLIENT);
	m_grpVision.SetWindowText(sText.IsEmpty()?_T("idipClient"):sText);
	CRect rcWnd;
	m_grpRecherche.GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);
	m_grpSupervisor.SetWindowPos(NULL, rcWnd.left, rcWnd.top, NULL, NULL, SWP_NOZORDER|SWP_NOSIZE);
	m_grpRecherche.ShowWindow(SW_HIDE);
	m_radioRecherche_1.GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);
	m_radioSupervisor_1.SetWindowPos(NULL, rcWnd.left, rcWnd.top, NULL, NULL, SWP_NOZORDER|SWP_NOSIZE);
	m_radioRecherche_1.EnableWindow(FALSE);
	m_radioRecherche_1.ShowWindow(SW_HIDE);
	m_radioRecherche_2.GetWindowRect(rcWnd);
	ScreenToClient(rcWnd);
	m_radioSupervisor_2.SetWindowPos(NULL, rcWnd.left, rcWnd.top, NULL, NULL, SWP_NOZORDER|SWP_NOSIZE);
	m_radioRecherche_2.EnableWindow(FALSE);
	m_radioRecherche_2.ShowWindow(SW_HIDE);
#else
	sText = COemGuiApi::GetApplicationName(WAI_EXPLORER);
	m_grpVision.SetWindowText(sText.IsEmpty()?_T("Vision"):sText);
#endif
	sText = COemGuiApi::GetApplicationName(WAI_DATABASE_CLIENT);
	m_grpRecherche.SetWindowText(sText.IsEmpty()?_T("Recherche"):sText);
	sText = COemGuiApi::GetApplicationName(WAI_SUPERVISOR);
	m_grpSupervisor.SetWindowText(sText.IsEmpty()?_T("Supervisor"):sText);

	CancelChanges();
	SaveChanges();	// to init the registry keys and values at least once
	
	if (!IsLocal())
	{
		DisableAll();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMultiMonitorPage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CMultiMonitorPage::SaveChanges()
{
	WK_TRACE_USER(_T("Multi-Monitor-Einstellungen wurden geändert\n"));
	UpdateData();

	DWORD dwFlags;
	dwFlags = RadioControlToFlags(m_iVision);
	GetProfile().WriteInt(REG_KEY_CLIENT_SETTINGS,REG_KEY_USE_MONITOR, dwFlags);

#if _MFC_VER < 0x0710
	dwFlags = RadioControlToFlags(m_iRecherche);
	GetProfile().WriteInt(_T("Recherche"),REG_KEY_USE_MONITOR, dwFlags);
#endif

	DWORD dwOld = GetProfile().GetInt(REG_KEY_SUPERVISOR,REG_KEY_USE_MONITOR, 0);
	dwFlags = RadioControlToFlags(m_iSystVerw);
	GetProfile().WriteInt(REG_KEY_SUPERVISOR,REG_KEY_USE_MONITOR, dwFlags);
	if (dwOld != dwFlags)
	{
		CWnd * pWnd = theApp.GetMainWnd();
		if (pWnd && pWnd->IsKindOf(RUNTIME_CLASS(CMainFrame)))
		{
			((CMainFrame*)pWnd)->SetMonitor();
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CMultiMonitorPage::CancelChanges()
{
	DWORD dwFlags = USE_MONITOR_1;
	dwFlags = GetProfile().GetInt(REG_KEY_CLIENT_SETTINGS,REG_KEY_USE_MONITOR, dwFlags);
	m_iVision = FlagsToRadioControl(dwFlags);
	dwFlags = GetProfile().GetInt(_T("Recherche"),REG_KEY_USE_MONITOR, dwFlags);
	m_iRecherche = FlagsToRadioControl(dwFlags);
	dwFlags = GetProfile().GetInt(REG_KEY_SUPERVISOR,REG_KEY_USE_MONITOR, dwFlags);
	m_iSystVerw = FlagsToRadioControl(dwFlags);
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
int CMultiMonitorPage::FlagsToRadioControl(DWORD dwFlags)
{
	int iControl = RADIO_MONITOR_1;
	switch (dwFlags)
	{
		case USE_MONITOR_1:			iControl = RADIO_MONITOR_1;			break;
		case USE_MONITOR_2:			iControl = RADIO_MONITOR_2;			break;
		case USE_MONITOR_1_2:		iControl = RADIO_MONITOR_1_2;		break;
		default: WK_FAILED(); break;
	}
	return iControl;
}
/////////////////////////////////////////////////////////////////////////////
DWORD CMultiMonitorPage::RadioControlToFlags(int iRadioControl)
{
	DWORD dwFlags = USE_MONITOR_1;
	switch (iRadioControl)
	{
		case RADIO_MONITOR_1:		dwFlags = USE_MONITOR_1;			break;
		case RADIO_MONITOR_2:		dwFlags = USE_MONITOR_2;			break;
		case RADIO_MONITOR_1_2:		dwFlags = USE_MONITOR_1_2;			break;
		default: WK_FAILED(); break;
	}
	return dwFlags;
}
/////////////////////////////////////////////////////////////////////////////
void CMultiMonitorPage::OnChanged() 
{
	UpdateData();
#if _MFC_VER >= 0x0710
	SetModified();
	m_pParent->AddInitApp(WK_APP_NAME_IDIP_CLIENT);
#else
	SetModified(TRUE, FALSE);
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CMultiMonitorPage::OnChangedSV() 
{
	UpdateData();
	SetModified(TRUE, FALSE);
}
