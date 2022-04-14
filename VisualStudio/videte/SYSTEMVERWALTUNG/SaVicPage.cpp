// SaVicPage.cpp : implementation file
//
					 
#include "stdafx.h"
#include "systemverwaltung.h"
#include "SaVicPage.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szSaVicUnitSystem[]		= _T("SaVicUnit");
static TCHAR BASED_CODE szSaVicUnitExe[]		= _T("SaVicUnit.exe");

static TCHAR BASED_CODE szSaVicSystemFormat[]	= _T("SaVicUnit\\Device%d\\System");
static TCHAR BASED_CODE szSaVicGeneralFormat[]	= _T("SaVicUnit\\Device%d\\General");
static TCHAR BASED_CODE szSaVicMDFormat[]		= _T("SaVicUnit\\Device%d\\MotionDetection");
static TCHAR BASED_CODE szSaVicFrontendFormat[]	= _T("SaVicUnit\\Device%d\\FrontEnd");
static TCHAR BASED_CODE szSaVicBackendFormat[]	= _T("SaVicUnit\\Device%d\\BackEnd");

static TCHAR BASED_CODE szSaVicFpsFormat[] = _T("MiCoFPS%d"); // Must be MiCoFPS not SaVicFPS
static TCHAR BASED_CODE szSecurityServerFormat[] = _T("SecurityServer");

/////////////////////////////////////////////////////////////////////////////
#define VIDEO_NORM_AUTO	0
#define VIDEO_NORM_PAL	1
#define VIDEO_NORM_NTSC	2

/////////////////////////////////////////////////////////////////////////////
// CSaVicPage dialog
CSaVicPage::CSaVicPage(CSVView* pParent, int nSaVicNr) : CSVPage(CSaVicPage::IDD)
{
	m_pParent			= pParent;
	m_nSaVicNr			= nSaVicNr;
	m_pInputList		= pParent->GetDocument()->GetInputs();
	m_pOutputList		= pParent->GetDocument()->GetOutputs();
	m_bRestartSaVicUnit	= FALSE;
	m_bSaVic			= FALSE;

	CString sMelder,sKamera,sRelais;
	sMelder.LoadString(IDS_ACTIVATION_INPUT);
	sKamera.LoadString(IDS_ACTIVATION_OUTPUT);
	sRelais.LoadString(IDS_RELAY);

	// Richtigen SharedMemorynamen und Defaultnamen wählen
	if (nSaVicNr == SAVIC1)
	{
		m_sSM_SaVicOutputCameras	= SM_SAVIC_OUTPUT_CAMERAS;
		m_sSM_SaVicOutputRelays		= SM_SAVIC_OUTPUT_RELAYS;
		m_sSM_SaVicInput			= SM_SAVIC_INPUT;
		m_sSM_SaVicInputMD			= SM_SAVIC_MD_INPUT;
		m_sAppName					= WK_APP_NAME_SAVICUNIT1;
	}
	else if (nSaVicNr == SAVIC2)
	{
		m_sSM_SaVicOutputCameras	= SM_SAVIC_OUTPUT_CAMERAS2;
		m_sSM_SaVicOutputRelays		= SM_SAVIC_OUTPUT_RELAYS2;
		m_sSM_SaVicInput			= SM_SAVIC_INPUT2;
		m_sSM_SaVicInputMD			= SM_SAVIC_MD_INPUT2;
		m_sAppName					= WK_APP_NAME_SAVICUNIT2;
	}
	else if (nSaVicNr == SAVIC3)
	{
		m_sSM_SaVicOutputCameras	= SM_SAVIC_OUTPUT_CAMERAS3;
		m_sSM_SaVicOutputRelays		= SM_SAVIC_OUTPUT_RELAYS3;
		m_sSM_SaVicInput			= SM_SAVIC_INPUT3;
		m_sSM_SaVicInputMD			= SM_SAVIC_MD_INPUT3;
		m_sAppName					= WK_APP_NAME_SAVICUNIT3;
	}
	else if (nSaVicNr == SAVIC4)
	{
		m_sSM_SaVicOutputCameras	= SM_SAVIC_OUTPUT_CAMERAS4;
		m_sSM_SaVicOutputRelays		= SM_SAVIC_OUTPUT_RELAYS4;
		m_sSM_SaVicInput			= SM_SAVIC_INPUT4;
		m_sSM_SaVicInputMD			= SM_SAVIC_MD_INPUT4;
		m_sAppName					= WK_APP_NAME_SAVICUNIT4;
	}
	else
		WK_TRACE_ERROR(_T("Unknown SaVicNr (%u)\n"), m_nSaVicNr);

	// Registrypath in abhängigkeit der gewählten SaVic-Karte
	m_sSaVicSystem.Format(szSaVicSystemFormat, m_nSaVicNr);
	m_sSaVicGeneral.Format(szSaVicGeneralFormat, m_nSaVicNr);
	m_sSaVicMD.Format(szSaVicMDFormat, m_nSaVicNr);
	m_sSaVicFrontend.Format(szSaVicFrontendFormat, m_nSaVicNr);
	m_sSaVicBackend.Format(szSaVicBackendFormat, m_nSaVicNr);
	m_sSaVicFps.Format(szSaVicFpsFormat, m_nSaVicNr);

	//{{AFX_DATA_INIT(CSaVicPage)
	m_bSyncCam					= FALSE;
	m_bUseGlobalColorSettings	= TRUE;
	m_bSaVic = FALSE;
	m_bUse2Fields				= FALSE;
	m_iVideoNorm = VIDEO_NORM_PAL;	// Da die SaVic z.Z. eh nur PAL unterstützt
	m_iResolution = SAVIC_RESOLUTION_HIGH;
	m_bNR = TRUE;
	//}}AFX_DATA_INIT
	Create(IDD,pParent);
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSaVicPage)
	DDX_Control(pDX, IDC_CHECK_NR, m_checkNR);
	DDX_Control(pDX, IDC_CHECK_SAVIC,				m_checkSaVic);
	DDX_Control(pDX, IDC_CHECK_SAVIC_USE2FIELDS,	m_checkUse2Fields);
	DDX_Control(pDX, IDC_CHECK_SAVIC_SYNC,			m_checkSync);
	DDX_Control(pDX, IDC_CHECK_SAVIC_GLOBAL_COLORS,	m_checkUseGlobalColorSettings);
	DDX_Check(pDX, IDC_CHECK_SAVIC_SYNC,			m_bSyncCam);
	DDX_Check(pDX, IDC_CHECK_SAVIC_GLOBAL_COLORS,	m_bUseGlobalColorSettings);
	DDX_Check(pDX, IDC_CHECK_SAVIC,					m_bSaVic);
	DDX_Check(pDX, IDC_CHECK_SAVIC_USE2FIELDS,		m_bUse2Fields);
	DDX_Radio(pDX, IDC_RADIO_AUTO, m_iVideoNorm);
	DDX_Radio(pDX, IDC_RADIO_RESOLUTION_HIGH, m_iResolution);
	DDX_Check(pDX, IDC_CHECK_NR, m_bNR);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSaVicPage, CSVPage)
	//{{AFX_MSG_MAP(CSaVicPage)
	ON_BN_CLICKED(IDC_CHECK_SAVIC_SYNC,				OnCheckSync)
	ON_BN_CLICKED(IDC_CHECK_SAVIC_GLOBAL_COLORS,	OnCheckUseGlobalColorSetting)
	ON_BN_CLICKED(IDC_CHECK_SAVIC,					OnCheckSaVic)
	ON_BN_CLICKED(IDC_CHECK_SAVIC_USE2FIELDS,		OnCheckUse2Fields)
	ON_BN_CLICKED(IDC_MD_CONFIG, OnMdConfig)
	ON_BN_CLICKED(IDC_RADIO_AUTO, OnRadioVideoNorm)
	ON_BN_CLICKED(IDC_RADIO_RESOLUTION_LOW, OnRadioResolution)
	ON_BN_CLICKED(IDC_RADIO_NTSC, OnRadioVideoNorm)
	ON_BN_CLICKED(IDC_RADIO_PAL, OnRadioVideoNorm)
	ON_BN_CLICKED(IDC_RADIO_RESOLUTION_HIGH, OnRadioResolution)
	ON_BN_CLICKED(IDC_CHECK_NR, OnCheckNr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSaVicPage message handlers
BOOL CSaVicPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	// SaVic Einstellungen aus der Registry laden.
	CancelChanges();

	if (!IsLocal())
	{
		DisableAll();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
BOOL CSaVicPage::IsDataValid()
{
	UpdateData();			  
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::SaveChanges()
{
	WK_TRACE_USER(_T("SaVic-Einstellungen wurden geändert\n"));
	UpdateData(TRUE);

	// SaVic aktualisieren
	if ( m_bSaVic ) 
	{
		GetDlgSaVicData();
	}
	else 
	{
		// Gruppen loeschen
		CProcessList* pProcessList = m_pParent->GetDocument()->GetProcesses();
		CString sGroupname (_T("SaVic"));
		DeleteProcessesOfExtensionCards(pProcessList, m_pOutputList, sGroupname);
		
		m_pInputList->DeleteGroup(m_sSM_SaVicInput);
		m_pInputList->DeleteGroup(m_sSM_SaVicInputMD);
		m_pOutputList->DeleteGroup(m_sSM_SaVicOutputCameras);
		m_pOutputList->DeleteGroup(m_sSM_SaVicOutputRelays);
	}

	// Einstellungen speichern
	SaveSaVicSettings();

	if (m_nSaVicNr == SAVIC1)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT1,m_bSaVic ? szSaVicUnitExe : _T(""));
	else if(m_nSaVicNr == SAVIC2)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT2,m_bSaVic ? szSaVicUnitExe : _T(""));
	else if(m_nSaVicNr == SAVIC3)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT3,m_bSaVic ? szSaVicUnitExe : _T(""));
	else if(m_nSaVicNr == SAVIC4)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT4,m_bSaVic ? szSaVicUnitExe : _T(""));

	SaveInputsAndOutputsAfterCheckingActivations();
	SetModified(FALSE,FALSE);
	m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, (1<<IMAGE_INPUT)|(1<<IMAGE_RELAY)|(1<<IMAGE_CAMERA));
//	m_pParent->OnInputChanged();
//	m_pParent->OnCameraChanged();
//	m_pParent->OnRelaisChanged();
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::CancelChanges()
{
	LoadSaVicSettings();
	UpdateData(FALSE);
	EnableDlgControlsSaVic();

	if ( IsDataValid() ) 
	{
		SetModified(FALSE,FALSE);
	}
	else 
	{
		SetModified();
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::OnCheckSync() 
{
	UpdateData();			  
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::OnCheckUseGlobalColorSetting() 
{
	UpdateData();			  
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::OnCheckSaVic() 
{
	UpdateData(TRUE);
	if (m_bSaVic)
	{
		SetModified();
	}
	else
	{
		if (AfxMessageBox(IDP_HARDWARE_GROUP_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO )
			m_bSaVic	= TRUE;
		else
			SetModified();
	}
	EnableDlgControlsSaVic();
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::LoadSaVicSettings()
{
	// Ist eine SaVic im System eingetragen?
	if (m_nSaVicNr == SAVIC1)
		m_bSaVic = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT1, _T("")) != _T(""));
	else if(m_nSaVicNr == SAVIC2)
		m_bSaVic = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT2, _T("")) != _T(""));
	else if(m_nSaVicNr == SAVIC3)
		m_bSaVic = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT3, _T("")) != _T(""));
	else if(m_nSaVicNr == SAVIC4)
		m_bSaVic = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT4, _T("")) != _T(""));


	// synchrone und asynchrone Kameras
	m_bSyncCam					= GetProfile().GetInt(m_sSaVicGeneral, _T("CameraType"), FALSE);
	m_bUse2Fields				= GetProfile().GetInt(m_sSaVicGeneral, _T("StoreField"), FALSE);
	m_bUseGlobalColorSettings	= GetProfile().GetInt(m_sSaVicFrontend, _T("UseGlobalColorSetting"), TRUE);
	m_bNR						= GetProfile().GetInt(m_sSaVicFrontend,_T("YUVNoiseReduction"),TRUE);

	CString sNorm =	GetProfile().GetString(m_sSaVicFrontend,_T("VideoNormDetection"),_T("forcepal"));

	if (0==sNorm.CompareNoCase(_T("forcepal")))
	{
		m_iVideoNorm = VIDEO_NORM_PAL;
	}
	else if (0==sNorm.CompareNoCase(_T("forcentsc")))
	{
		m_iVideoNorm = VIDEO_NORM_NTSC;
	}

	if (m_iVideoNorm == VIDEO_NORM_PAL)
	{
		GetDlgItem(IDC_RADIO_RESOLUTION_HIGH)->SetWindowText(_T("768x288"));
		GetDlgItem(IDC_RADIO_RESOLUTION_LOW)->SetWindowText(_T("384x288"));
	}
	else if (m_iVideoNorm == VIDEO_NORM_NTSC)
	{
		GetDlgItem(IDC_RADIO_RESOLUTION_HIGH)->SetWindowText(_T("720x240"));
		GetDlgItem(IDC_RADIO_RESOLUTION_LOW)->SetWindowText(_T("360x240"));
	}

	DWORD dwResolution= (DWORD)(GetProfile().GetInt(m_sSaVicFrontend,_T("CaptureSizeH"), 768));
	if ((dwResolution == 768) || (dwResolution == 720))
		m_iResolution = SAVIC_RESOLUTION_HIGH;
	else
		m_iResolution = SAVIC_RESOLUTION_LOW;
}
					    
/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::SaveSaVicSettings()
{
	// Kameratype und GlobalColorSetting sichern
	GetProfile().WriteInt(m_sSaVicGeneral, _T("CameraType"), m_bSyncCam);
	GetProfile().WriteInt(m_sSaVicFrontend, _T("UseGlobalColorSetting"), m_bUseGlobalColorSettings);
	GetProfile().WriteInt(m_sSaVicFrontend,_T("YUVNoiseReduction"),m_bNR);

	DWORD dwResolution = 768;
	switch (m_iResolution)
	{
		case SAVIC_RESOLUTION_HIGH:
			if (m_iVideoNorm == VIDEO_NORM_PAL)
				dwResolution = 768;
			else if (m_iVideoNorm == VIDEO_NORM_NTSC)
				dwResolution = 720;
			break;
		case SAVIC_RESOLUTION_LOW:
			if (m_iVideoNorm == VIDEO_NORM_PAL)
				dwResolution = 768/2;
			else if (m_iVideoNorm == VIDEO_NORM_NTSC)
				dwResolution = 720/2;
			break;
	}
	GetProfile().WriteInt(m_sSaVicFrontend,_T("CaptureSizeH"), (int)dwResolution);

	CString sNorm = _T("auto");
	switch (m_iVideoNorm)
	{
		case VIDEO_NORM_AUTO:
			sNorm = _T("auto");	// <- Wird nicht unterstützt
			break;
		case VIDEO_NORM_PAL:
			sNorm = _T("forcepal");
			break;
		case VIDEO_NORM_NTSC:
			sNorm = _T("forcentsc");
			break;
	}
	GetProfile().WriteString(m_sSaVicFrontend,_T("VideoNormDetection"),sNorm);

/* ML Die SaVic unterstützt derzeit nur ein Field
	if (m_bUse2Fields)
	{
		GetProfile().WriteInt(m_sSaVicGeneral, _T("StoreField"), 3);
		GetProfile().WriteInt(szSecurityServerFormat, m_sSaVicFps, 50);
	}
	else								   
	{
		GetProfile().WriteInt(m_sSaVicGeneral, _T("StoreField"), 0);
		GetProfile().WriteInt(szSecurityServerFormat, m_sSaVicFps, 25);
	}
*/
	GetProfile().WriteInt(m_sSaVicMD, _T("Activate"), TRUE);

	// SaVicUnit beenden
	if (m_bRestartSaVicUnit)
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

/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::EnableDlgControlsSaVic()
{
	// Enable/Disable Dialogcontrols
	m_checkUseGlobalColorSettings.EnableWindow(m_bSaVic);
	m_checkSync.EnableWindow(m_bSaVic);
	m_checkUse2Fields.ShowWindow(SW_HIDE);	// gf todo not yet implemented
	GetDlgItem(IDC_RADIO_PAL)->EnableWindow(m_bSaVic);	
	GetDlgItem(IDC_RADIO_AUTO)->EnableWindow(FALSE);	// Auto und NTSC z.Z. nicht implementiert.
	GetDlgItem(IDC_RADIO_NTSC)->EnableWindow(FALSE);	//  "	
	GetDlgItem(IDC_RADIO_RESOLUTION_HIGH)->EnableWindow(m_bSaVic);	
	GetDlgItem(IDC_RADIO_RESOLUTION_LOW)->EnableWindow(m_bSaVic);
	m_checkNR.EnableWindow(m_bSaVic);

	// Die 1. SaVic darf nicht ausgetragen werden, solange eine 2. SaVic eingetragen ist
	if (m_nSaVicNr == SAVIC1)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT2, _T("")) != _T(""))
			m_checkSaVic.EnableWindow(!m_bSaVic);
	}
	// Die 2. SaVic darf nicht ausgetragen werden, solange eine 3. SaVic eingetragen ist
	if (m_nSaVicNr == SAVIC2)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT3, _T("")) != _T(""))
			m_checkSaVic.EnableWindow(!m_bSaVic);
	}
	// Die 3. SaVic darf nicht ausgetragen werden, solange eine 4. SaVic eingetragen ist
	if (m_nSaVicNr == SAVIC3)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT4, _T("")) != _T(""))
			m_checkSaVic.EnableWindow(!m_bSaVic);
	}

	// Die 2. SAVIC darf nur eingetragen werden, wenn die 1. SAVIC ebenfalls eingetragen ist
	if (m_nSaVicNr == SAVIC2)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT1, _T("")) == _T(""))
			m_checkSaVic.EnableWindow(FALSE);
	}
	// Die 3. SAVIC darf nur eingetragen werden, wenn die 2. SAVIC ebenfalls eingetragen ist
	if (m_nSaVicNr == SAVIC3)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT2, _T("")) == _T(""))
			m_checkSaVic.EnableWindow(FALSE);
	}
	// Die 4. SAVIC darf nur eingetragen werden, wenn die 3. SAVIC ebenfalls eingetragen ist
	if (m_nSaVicNr == SAVIC4)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_SAVICUNIT3, _T("")) == _T(""))
			m_checkSaVic.EnableWindow(FALSE);
	}

	BOOL bEnable = m_bSaVic && WK_IS_RUNNING(m_sAppName);
	GetDlgItem(IDC_MD_CONFIG)->EnableWindow(bEnable);	
	
	// gf Es werden keine Variablen verändert, warum also Controls updaten?
//	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::GetDlgSaVicData()
{
	CInputGroup* pInputGroup = NULL;
	CInputGroup* pInputGroupMD = NULL;
	COutputGroup* pOutputGroup = NULL;
	int iAnzahl = 0;

	// Anzahl der Inputs einlesen
	iAnzahl = NR_INPUTS_SAVIC;
	
	// Daten der Inputgruppe aktualisieren, wenn bereits vorhanden...
	pInputGroup = m_pInputList->GetGroupBySMName(m_sSM_SaVicInput);
	if ( pInputGroup)
	{
		pInputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else
	{
		pInputGroup = m_pInputList->AddInputGroup(NULL, iAnzahl, m_sSM_SaVicInput);
	}

	// Daten der MD-Inputgruppe aktualisieren, wenn bereits vorhanden...
	pInputGroupMD = m_pInputList->GetGroupBySMName(m_sSM_SaVicInputMD);
	if (pInputGroupMD)
	{
		pInputGroupMD->SetSize(NR_OUTPUTS_CAMERA_SAVIC);	// Nur die 4 Kameras der SaVic sollen MotionDetection unterstützen
	}
	// ...sonst neue Gruppe anlegen
	else
	{
		pInputGroupMD = m_pInputList->AddInputGroup(NULL, iAnzahl, m_sSM_SaVicInputMD);
	}

	if (m_bSaVic)
	{
		iAnzahl = NR_OUTPUTS_CAMERA_SAVIC;
	}


	// Daten der Kamera-Outputgruppe aktualisieren, wenn bereits vorhanden...
	pOutputGroup = m_pOutputList->GetGroupBySMName(m_sSM_SaVicOutputCameras);
	if ( pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(NULL, iAnzahl, m_sSM_SaVicOutputCameras);
	}

	// Anzahl der Relay Outputs einlesen
	if (m_bSaVic)
		iAnzahl = NR_OUTPUTS_RELAY_SAVIC;
	
	// Daten der Relay-Outputgruppe aktualisieren, wenn bereits vorhanden...
	pOutputGroup = m_pOutputList->GetGroupBySMName( m_sSM_SaVicOutputRelays);
	if ( pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(NULL, iAnzahl, m_sSM_SaVicOutputRelays);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::OnCheckUse2Fields() 
{
	UpdateData(TRUE);
	SetModified();
	if (m_bUse2Fields)
	{
		CString sMsg;
		sMsg.Format(IDP_2FIELDS_WARNING);
		AfxMessageBox(sMsg, MB_OK);					  
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::SetModified(BOOL bModified, BOOL bServerInit)
{
	// Basisklasse aufrufen
	CSVPage::SetModified(bModified, bServerInit);
	m_bRestartSaVicUnit	= TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::OnMdConfig() 
{
	if (WK_IS_RUNNING(m_sAppName))
	{
		CWnd *pWnd = FindWindow(NULL, m_sAppName);
		if (pWnd && IsWindow(pWnd->m_hWnd))
		{
			pWnd->PostMessage(SAVIC_OPEN_MD_DLG, 0, (LPARAM)this->m_hWnd);
			pWnd->SetWindowPos(&wndTop, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::OnRadioVideoNorm() 
{
	UpdateData();
	SetModified();

	if (m_iVideoNorm == VIDEO_NORM_PAL)
	{
		GetDlgItem(IDC_RADIO_RESOLUTION_HIGH)->SetWindowText(_T("768x288"));
		GetDlgItem(IDC_RADIO_RESOLUTION_LOW)->SetWindowText(_T("384x288"));
	}
	else if (m_iVideoNorm == VIDEO_NORM_NTSC)
	{
		GetDlgItem(IDC_RADIO_RESOLUTION_HIGH)->SetWindowText(_T("720x240"));
		GetDlgItem(IDC_RADIO_RESOLUTION_LOW)->SetWindowText(_T("360x240"));
	}
}
/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::OnRadioResolution() 
{
	UpdateData();			  
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::OnIdleSVPage()
{
	// some controls are dependent from the running module
	EnableDlgControlsSaVic();
}
/////////////////////////////////////////////////////////////////////////////
void CSaVicPage::OnCheckNr() 
{
	UpdateData();			  
	SetModified();
}
