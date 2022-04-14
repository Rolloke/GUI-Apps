// TashaPage.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "TashaPage.h"
#include ".\tashapage.h"

static TCHAR BASED_CODE szTashaUnitSystem[]		= _T("TashaUnit");
static TCHAR BASED_CODE szTashaUnitExe[]		= _T("TashaUnit.exe");

static TCHAR BASED_CODE szTashaSystemFormat[]	= _T("TashaUnit\\Device%d\\System");
static TCHAR BASED_CODE szTashaGeneralFormat[]	= _T("TashaUnit\\Device%d\\General");
static TCHAR BASED_CODE szTashaMDFormat[]		= _T("TashaUnit\\Device%d\\MotionDetection");
static TCHAR BASED_CODE szTashaFrontendFormat[]	= _T("TashaUnit\\Device%d\\FrontEnd");
static TCHAR BASED_CODE szTashaBackendFormat[]	= _T("TashaUnit\\Device%d\\BackEnd");

static TCHAR BASED_CODE szTashaFpsFormat[] = _T("MiCoFPS%d"); // Must be MiCoFPS not TashaFPS
static TCHAR BASED_CODE szSecurityServerFormat[] = _T("SecurityServer");

#define VIDEO_NORM_AUTO	0
#define VIDEO_NORM_PAL	1
#define VIDEO_NORM_NTSC	2

// CTashaPage dialog

IMPLEMENT_DYNAMIC(CTashaPage, CSVPage)
CTashaPage::CTashaPage(CSVView* pParent, int nTashaNr)
	: CSVPage(CTashaPage::IDD)
	, m_bTasha(FALSE)
{
	m_pParent			= pParent;
	m_nTashaNr			= nTashaNr;
	m_pInputList		= pParent->GetDocument()->GetInputs();
	m_pOutputList		= pParent->GetDocument()->GetOutputs();

	m_bTasha = FALSE;
	m_iVideoNorm = VIDEO_NORM_AUTO;
	m_bNR = FALSE;

	CString sMelder,sKamera,sRelais;
	sMelder.LoadString(IDS_ACTIVATION_INPUT);
	sKamera.LoadString(IDS_ACTIVATION_OUTPUT);
	sRelais.LoadString(IDS_RELAY);

	// Richtigen SharedMemorynamen und Defaultnamen wählen
	if (nTashaNr == TASHA1)
	{
		m_sSM_TashaOutputCameras	= SM_TASHA_OUTPUT_CAMERAS;
		m_sSM_TashaOutputRelays		= SM_TASHA_OUTPUT_RELAYS;
		m_sSM_TashaInput			= SM_TASHA_INPUT;
		m_sSM_TashaInputMD			= SM_TASHA_MD_INPUT;
		m_sAppName					= WK_APP_NAME_TASHAUNIT1;
	}
	else if (nTashaNr == TASHA2)
	{
		m_sSM_TashaOutputCameras	= SM_TASHA_OUTPUT_CAMERAS2;
		m_sSM_TashaOutputRelays		= SM_TASHA_OUTPUT_RELAYS2;
		m_sSM_TashaInput			= SM_TASHA_INPUT2;
		m_sSM_TashaInputMD			= SM_TASHA_MD_INPUT2;
		m_sAppName					= WK_APP_NAME_TASHAUNIT2;
	}
	else if (nTashaNr == TASHA3)
	{
		m_sSM_TashaOutputCameras	= SM_TASHA_OUTPUT_CAMERAS3;
		m_sSM_TashaOutputRelays		= SM_TASHA_OUTPUT_RELAYS3;
		m_sSM_TashaInput			= SM_TASHA_INPUT3;
		m_sSM_TashaInputMD			= SM_TASHA_MD_INPUT3;
		m_sAppName					= WK_APP_NAME_TASHAUNIT3;
	}
	else if (nTashaNr == TASHA4)
	{
		m_sSM_TashaOutputCameras	= SM_TASHA_OUTPUT_CAMERAS4;
		m_sSM_TashaOutputRelays		= SM_TASHA_OUTPUT_RELAYS4;
		m_sSM_TashaInput			= SM_TASHA_INPUT4;
		m_sSM_TashaInputMD			= SM_TASHA_MD_INPUT4;
		m_sAppName					= WK_APP_NAME_TASHAUNIT4;
	}
	else
		WK_TRACE_ERROR(_T("Unknown TashaNr (%u)\n"), m_nTashaNr);

	// Registrypath in abhängigkeit der gewählten Tasha-Karte
	m_sTashaSystem.Format(szTashaSystemFormat, m_nTashaNr);
	m_sTashaGeneral.Format(szTashaGeneralFormat, m_nTashaNr);
	m_sTashaMD.Format(szTashaMDFormat, m_nTashaNr);
	m_sTashaFrontend.Format(szTashaFrontendFormat, m_nTashaNr);
	m_sTashaBackend.Format(szTashaBackendFormat, m_nTashaNr);
	m_sTashaFps.Format(szTashaFpsFormat, m_nTashaNr);



	Create(IDD,pParent);
}

CTashaPage::~CTashaPage()
{
}

void CTashaPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	DDX_Check(pDX, IDC_CHECK_TASHA, m_bTasha);
	DDX_Radio(pDX, IDC_RADIO_AUTO, m_iVideoNorm);
	DDX_Check(pDX, IDC_CHECK_NR, m_bNR);
	DDX_Control(pDX, IDC_CHECK_NR, m_checkNR);
	DDX_Control(pDX, IDC_CHECK_TASHA, m_checkTasha);
}


BEGIN_MESSAGE_MAP(CTashaPage, CSVPage)
	ON_BN_CLICKED(IDC_CHECK_TASHA, OnBnClickedCheckTasha)
	ON_BN_CLICKED(IDC_CHECK_NR, OnBnClickedCheckNr)
END_MESSAGE_MAP()


// CTashaPage message handlers
////////////////////////////////////////////////////////////////////////////////////
BOOL CTashaPage::IsDataValid()
{
	UpdateData();			  
	return TRUE;
}
////////////////////////////////////////////////////////////////////////////////////
void CTashaPage::SaveChanges()
{
	WK_TRACE_USER(_T("Tasha-Einstellungen wurden geändert\n"));
	UpdateData(TRUE);

	// Tasha aktualisieren
	if ( m_bTasha ) 
	{
		GetDlgTashaData();
	}
	else 
	{
		// Gruppen loeschen
		CProcessList* pProcessList = m_pParent->GetDocument()->GetProcesses();
		CString sGroupname (_T("Tasha"));
		DeleteProcessesOfExtensionCards(pProcessList, m_pOutputList, sGroupname);
		
		m_pInputList->DeleteGroup(m_sSM_TashaInput);
		m_pInputList->DeleteGroup(m_sSM_TashaInputMD);
		m_pOutputList->DeleteGroup(m_sSM_TashaOutputCameras);
		m_pOutputList->DeleteGroup(m_sSM_TashaOutputRelays);
	}

	// Einstellungen speichern
	SaveTashaSettings();

	if (m_nTashaNr == TASHA1)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT1,m_bTasha ? szTashaUnitExe : _T(""));
	else if(m_nTashaNr == TASHA2)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT2,m_bTasha ? szTashaUnitExe : _T(""));
	else if(m_nTashaNr == TASHA3)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT3,m_bTasha ? szTashaUnitExe : _T(""));
	else if(m_nTashaNr == TASHA4)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT4,m_bTasha ? szTashaUnitExe : _T(""));

	SaveInputsAndOutputsAfterCheckingActivations();
	SetModified(FALSE,FALSE);
	m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, (1<<IMAGE_INPUT)|(1<<IMAGE_RELAY)|(1<<IMAGE_CAMERA));
}
////////////////////////////////////////////////////////////////////////////////////
void CTashaPage::CancelChanges()
{
	LoadTashaSettings();
	UpdateData(FALSE);
	EnableDlgControlsTasha();

	if ( IsDataValid() ) 
	{
		SetModified(FALSE,FALSE);
	}
	else 
	{
		SetModified();
	}
}
////////////////////////////////////////////////////////////////////////////////////
void CTashaPage::SetModified(BOOL bModified /*= TRUE*/ ,BOOL bServerInit /*= TRUE*/)
{
	CSVPage::SetModified(bModified, bServerInit);
	m_bRestartTashaUnit = TRUE;
}
////////////////////////////////////////////////////////////////////////////////////
void CTashaPage::OnIdleSVPage()
{
	EnableDlgControlsTasha();
}
////////////////////////////////////////////////////////////////////////////////////
void CTashaPage::LoadTashaSettings()
{
	// Ist eine Tasha im System eingetragen?
	if (m_nTashaNr == TASHA1)
		m_bTasha = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT1, _T("")) != _T(""));
	else if(m_nTashaNr == TASHA2)
		m_bTasha = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT2, _T("")) != _T(""));
	else if(m_nTashaNr == TASHA3)
		m_bTasha = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT3, _T("")) != _T(""));
	else if(m_nTashaNr == TASHA4)
		m_bTasha = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT4, _T("")) != _T(""));


	m_bNR = GetProfile().GetInt(m_sTashaFrontend,_T("YUVNoiseReduction"),m_bNR);

	CString sNorm =	GetProfile().GetString(m_sTashaFrontend,_T("VideoNormDetection"),_T("auto"));

	if (0==sNorm.CompareNoCase(_T("forcepal")))
	{
		m_iVideoNorm = VIDEO_NORM_PAL;
	}
	else if (0==sNorm.CompareNoCase(_T("forcentsc")))
	{
		m_iVideoNorm = VIDEO_NORM_NTSC;
	}
}
////////////////////////////////////////////////////////////////////////////////////
void CTashaPage::SaveTashaSettings()
{
	// Kameratype und GlobalColorSetting sichern
	GetProfile().WriteInt(m_sTashaFrontend,_T("YUVNoiseReduction"),m_bNR);

	CString sNorm = _T("auto");
	switch (m_iVideoNorm)
	{
	case VIDEO_NORM_AUTO:
		sNorm = _T("auto");
		break;
	case VIDEO_NORM_PAL:
		sNorm = _T("forcepal");
		break;
	case VIDEO_NORM_NTSC:
		sNorm = _T("forcentsc");
		break;
	}
	GetProfile().WriteString(m_sTashaFrontend,_T("VideoNormDetection"),sNorm);

	GetProfile().WriteInt(m_sTashaMD, _T("Activate"), TRUE);

	// TashaUnit beenden
	if (m_bRestartTashaUnit)
	{
		if (WK_IS_RUNNING(m_sAppName))
		{
			CWnd *pWnd = FindWindow(NULL, m_sAppName);
			if (pWnd && IsWindow(pWnd->m_hWnd))
			{
				CWnd *pWndOk = pWnd->GetDlgItem(IDOK);
				if (pWndOk && IsWindow(pWnd->m_hWnd))
				{
					pWnd->PostMessage(WM_COMMAND,(WPARAM)MAKELONG(IDOK,BN_CLICKED),(LPARAM)pWndOk->m_hWnd);
				}
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////
void CTashaPage::EnableDlgControlsTasha()
{
	// Enable/Disable Dialogcontrols

	GetDlgItem(IDC_RADIO_PAL)->EnableWindow(m_bTasha);	
	GetDlgItem(IDC_RADIO_AUTO)->EnableWindow(m_bTasha);	// Auto und NTSC z.Z. nicht implementiert.
	GetDlgItem(IDC_RADIO_NTSC)->EnableWindow(m_bTasha);	//  "	
	m_checkNR.EnableWindow(m_bTasha);

	// Die 1. Tasha darf nicht ausgetragen werden, solange eine 2. Tasha eingetragen ist
	if (m_nTashaNr == TASHA1)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT2, _T("")) != _T(""))
			m_checkTasha.EnableWindow(!m_bTasha);
	}
	// Die 2. Tasha darf nicht ausgetragen werden, solange eine 3. Tasha eingetragen ist
	if (m_nTashaNr == TASHA2)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT3, _T("")) != _T(""))
			m_checkTasha.EnableWindow(!m_bTasha);
	}
	// Die 3. Tasha darf nicht ausgetragen werden, solange eine 4. Tasha eingetragen ist
	if (m_nTashaNr == TASHA3)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT4, _T("")) != _T(""))
			m_checkTasha.EnableWindow(!m_bTasha);
	}

	// Die 2. TASHA darf nur eingetragen werden, wenn die 1. TASHA ebenfalls eingetragen ist
	if (m_nTashaNr == TASHA2)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT1, _T("")) == _T(""))
			m_checkTasha.EnableWindow(FALSE);
	}
	// Die 3. TASHA darf nur eingetragen werden, wenn die 2. TASHA ebenfalls eingetragen ist
	if (m_nTashaNr == TASHA3)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT2, _T("")) == _T(""))
			m_checkTasha.EnableWindow(FALSE);
	}
	// Die 4. TASHA darf nur eingetragen werden, wenn die 3. TASHA ebenfalls eingetragen ist
	if (m_nTashaNr == TASHA4)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_TASHAUNIT3, _T("")) == _T(""))
			m_checkTasha.EnableWindow(FALSE);
	}
}
////////////////////////////////////////////////////////////////////////////////////
void CTashaPage::GetDlgTashaData()
{
	CInputGroup* pInputGroup = NULL;
	CInputGroup* pInputGroupMD = NULL;
	COutputGroup* pOutputGroup = NULL;
	int iAnzahl = 0;

	// Anzahl der Inputs einlesen

	// Daten der Inputgruppe aktualisieren, wenn bereits vorhanden...
	pInputGroup = m_pInputList->GetGroupBySMName(m_sSM_TashaInput);
	if ( pInputGroup)
	{
		pInputGroup->SetSize((WORD)NR_INPUTS_TASHA);
	}
	// ...sonst neue Gruppe anlegen
	else
	{
		pInputGroup = m_pInputList->AddInputGroup(NULL, NR_INPUTS_TASHA, m_sSM_TashaInput);
	}

	iAnzahl = NR_OUTPUTS_CAMERA_TASHA;

	if (GetDongle().RestrictedTo8CamerasPerBoard())
	{
		iAnzahl = NR_OUTPUTS_CAMERA_TASHA/2;
	}

	// Daten der MD-Inputgruppe aktualisieren, wenn bereits vorhanden...
	pInputGroupMD = m_pInputList->GetGroupBySMName(m_sSM_TashaInputMD);
	if (pInputGroupMD)
	{
		pInputGroupMD->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else
	{
		pInputGroupMD = m_pInputList->AddInputGroup(NULL, iAnzahl, m_sSM_TashaInputMD);
	}

	// Daten der Kamera-Outputgruppe aktualisieren, wenn bereits vorhanden...
	pOutputGroup = m_pOutputList->GetGroupBySMName(m_sSM_TashaOutputCameras);
	if ( pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(NULL, iAnzahl, m_sSM_TashaOutputCameras);
	}

	// Anzahl der Relay Outputs einlesen

	// Daten der Relay-Outputgruppe aktualisieren, wenn bereits vorhanden...
	pOutputGroup = m_pOutputList->GetGroupBySMName( m_sSM_TashaOutputRelays);
	if ( pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)NR_OUTPUTS_RELAY_TASHA);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(NULL, NR_OUTPUTS_RELAY_TASHA, m_sSM_TashaOutputRelays);
	}
}
////////////////////////////////////////////////////////////////////////////////////
void CTashaPage::OnBnClickedCheckTasha()
{
	UpdateData(TRUE);
	if (m_bTasha)
	{
		SetModified();
	}
	else
	{
		if (AfxMessageBox(IDP_HARDWARE_GROUP_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO )
			m_bTasha = TRUE;
		else
			SetModified();
	}
	EnableDlgControlsTasha();
}

BOOL CTashaPage::OnInitDialog()
{
	CSVPage::OnInitDialog();

	// Tasha Einstellungen aus der Registry laden.
	CancelChanges();

	if (!IsLocal())
	{
		DisableAll();
	}


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CTashaPage::OnBnClickedCheckNr()
{
	SetModified();
}
