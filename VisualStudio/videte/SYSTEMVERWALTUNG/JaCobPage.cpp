// JaCobPage.cpp : implementation file
//
					 
#include "stdafx.h"
#include "systemverwaltung.h"
#include "JaCobPage.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szJaCobUnitSystem[]		= _T("JaCobUnit");
static TCHAR BASED_CODE szJaCobUnitExe[]		= _T("JaCobUnit.exe");

static TCHAR BASED_CODE szJaCobSystemFormat[]	= _T("JaCobUnit\\Device%d\\System");
static TCHAR BASED_CODE szJaCobGeneralFormat[]	= _T("JaCobUnit\\Device%d\\General");
static TCHAR BASED_CODE szJaCobMDFormat[]		= _T("JaCobUnit\\Device%d\\MotionDetection");
static TCHAR BASED_CODE szJaCobFrontendFormat[]	= _T("JaCobUnit\\Device%d\\FrontEnd");
static TCHAR BASED_CODE szJaCobBackendFormat[]	= _T("JaCobUnit\\Device%d\\BackEnd");

static TCHAR BASED_CODE szJaCobFpsFormat[] = _T("MiCoFPS%d"); // Must be MiCoFPS not JaCobFPS
static TCHAR BASED_CODE szSecurityServerFormat[] = _T("SecurityServer");

/////////////////////////////////////////////////////////////////////////////
// CJaCobPage dialog

CJaCobPage::CJaCobPage(CSVView* pParent, int nJaCobNr) : CSVPage(CJaCobPage::IDD)
{
	m_pParent			= pParent;
	m_nJaCobNr			= nJaCobNr;
	m_pInputList		= pParent->GetDocument()->GetInputs();
	m_pOutputList		= pParent->GetDocument()->GetOutputs();
	m_bRestartJaCobUnit	= FALSE;
	m_bJaCob			= FALSE;

	CString sMelder,sKamera,sRelais;
	sMelder.LoadString(IDS_ACTIVATION_INPUT);
	sKamera.LoadString(IDS_ACTIVATION_OUTPUT);
	sRelais.LoadString(IDS_RELAY);

	// Richtigen SharedMemorynamen und Defaultnamen wählen
	if (nJaCobNr == JACOB1)
	{
		m_sSM_JaCobOutputCameras	= SM_MICO_OUTPUT_CAMERAS;
		m_sSM_JaCobOutputRelays		= SM_MICO_OUTPUT_RELAYS;
		m_sSM_JaCobInput			= SM_MICO_INPUT;
		m_sSM_JaCobInputMD			= SM_MICO_MD_INPUT;
		m_sAppName					= WK_APP_NAME_JACOBUNIT1;
	}
	else if (nJaCobNr == JACOB2)
	{
		m_sSM_JaCobOutputCameras	= SM_MICO_OUTPUT_CAMERAS2;
		m_sSM_JaCobOutputRelays		= SM_MICO_OUTPUT_RELAYS2;
		m_sSM_JaCobInput			= SM_MICO_INPUT2;
		m_sSM_JaCobInputMD			= SM_MICO_MD_INPUT2;
		m_sAppName					= WK_APP_NAME_JACOBUNIT2;
	}
	else if (nJaCobNr == JACOB3)
	{
		m_sSM_JaCobOutputCameras	= SM_MICO_OUTPUT_CAMERAS3;
		m_sSM_JaCobOutputRelays		= SM_MICO_OUTPUT_RELAYS3;
		m_sSM_JaCobInput			= SM_MICO_INPUT3;
		m_sSM_JaCobInputMD			= SM_MICO_MD_INPUT3;
		m_sAppName					= WK_APP_NAME_JACOBUNIT3;
	}
	else if (nJaCobNr == JACOB4)
	{
		m_sSM_JaCobOutputCameras	= SM_MICO_OUTPUT_CAMERAS4;
		m_sSM_JaCobOutputRelays		= SM_MICO_OUTPUT_RELAYS4;
		m_sSM_JaCobInput			= SM_MICO_INPUT4;
		m_sSM_JaCobInputMD			= SM_MICO_MD_INPUT4;
		m_sAppName					= WK_APP_NAME_JACOBUNIT4;
	}
	else
		WK_TRACE_ERROR(_T("Unknown JaCobNr (%u)\n"), m_nJaCobNr);

	// Registrypath in abhängigkeit der gewählten JaCob-Karte
	m_sJaCobSystem.Format(szJaCobSystemFormat, m_nJaCobNr);
	m_sJaCobGeneral.Format(szJaCobGeneralFormat, m_nJaCobNr);
	m_sJaCobMD.Format(szJaCobMDFormat, m_nJaCobNr);
	m_sJaCobFrontend.Format(szJaCobFrontendFormat, m_nJaCobNr);
	m_sJaCobBackend.Format(szJaCobBackendFormat, m_nJaCobNr);
	m_sJaCobFps.Format(szJaCobFpsFormat, m_nJaCobNr);

	//{{AFX_DATA_INIT(CJaCobPage)
	m_bSyncCam					= FALSE;
	m_bUseGlobalColorSettings	= TRUE;
	m_bJaCob = FALSE;
	m_bUse2Fields				= FALSE;
	m_bMDOn = FALSE;
	m_iVideoNorm = 0;
	//}}AFX_DATA_INIT
	Create(IDD,pParent);
}

/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJaCobPage)
	DDX_Control(pDX, IDC_CHECK_MD_ON, m_checkMDOn);
	DDX_Control(pDX, IDC_CHECK_JACOB,				m_checkJaCob);
	DDX_Control(pDX, IDC_CHECK_JACOB_USE2FIELDS,	m_checkUse2Fields);
	DDX_Control(pDX, IDC_CHECK_JACOB_SYNC,			m_checkSync);
	DDX_Control(pDX, IDC_CHECK_JACOB_GLOBAL_COLORS,	m_checkUseGlobalColorSettings);
	DDX_Check(pDX, IDC_CHECK_JACOB_SYNC,			m_bSyncCam);
	DDX_Check(pDX, IDC_CHECK_JACOB_GLOBAL_COLORS,	m_bUseGlobalColorSettings);
	DDX_Check(pDX, IDC_CHECK_JACOB,					m_bJaCob);
	DDX_Check(pDX, IDC_CHECK_JACOB_USE2FIELDS,		m_bUse2Fields);
	DDX_Check(pDX, IDC_CHECK_MD_ON, m_bMDOn);
	DDX_Radio(pDX, IDC_RADIO_AUTO, m_iVideoNorm);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CJaCobPage, CSVPage)
	//{{AFX_MSG_MAP(CJaCobPage)
	ON_BN_CLICKED(IDC_CHECK_JACOB_SYNC,				OnCheckSync)
	ON_BN_CLICKED(IDC_CHECK_JACOB_GLOBAL_COLORS,	OnCheckUseGlobalColorSetting)
	ON_BN_CLICKED(IDC_CHECK_JACOB,					OnCheckJaCob)
	ON_BN_CLICKED(IDC_CHECK_JACOB_USE2FIELDS,		OnCheckUse2Fields)
	ON_BN_CLICKED(IDC_MD_CONFIG, OnMdConfig)
	ON_BN_CLICKED(IDC_CHECK_MD_ON, OnCheckMdOn)
	ON_BN_CLICKED(IDC_RADIO_AUTO, OnRadioAuto)
	ON_BN_CLICKED(IDC_RADIO_NTSC, OnRadioNtsc)
	ON_BN_CLICKED(IDC_RADIO_PAL, OnRadioPal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJaCobPage message handlers
BOOL CJaCobPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	// JaCob Einstellungen aus der Registry laden.
	CancelChanges();

	if (!IsLocal())
	{
		DisableAll();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
BOOL CJaCobPage::IsDataValid()
{
	UpdateData();			  
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::SaveChanges()
{
	WK_TRACE_USER(_T("JaCob-Einstellungen wurden geändert\n"));
	UpdateData(TRUE);

	// JaCob aktualisieren
	if ( m_bJaCob ) 
	{
		GetDlgJaCobData();
	}
	else 
	{
		// Gruppen loeschen
		CProcessList* pProcessList = m_pParent->GetDocument()->GetProcesses();
		CString sGroupname (_T("JaCob"));
		DeleteProcessesOfExtensionCards(pProcessList, m_pOutputList, sGroupname);	

		m_pInputList->DeleteGroup(m_sSM_JaCobInput);
		m_pInputList->DeleteGroup(m_sSM_JaCobInputMD);
		m_pOutputList->DeleteGroup(m_sSM_JaCobOutputCameras);
		m_pOutputList->DeleteGroup(m_sSM_JaCobOutputRelays);

	}

	// Einstellungen speichern
	SaveJaCobSettings();

	if (m_nJaCobNr == JACOB1)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_JACOBUNIT1,m_bJaCob ? szJaCobUnitExe : _T(""));
	else if(m_nJaCobNr == JACOB2)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_JACOBUNIT2,m_bJaCob ? szJaCobUnitExe : _T(""));
	else if(m_nJaCobNr == JACOB3)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_JACOBUNIT3,m_bJaCob ? szJaCobUnitExe : _T(""));
	else if(m_nJaCobNr == JACOB4)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_JACOBUNIT4,m_bJaCob ? szJaCobUnitExe : _T(""));

	SaveInputsAndOutputsAfterCheckingActivations();
	SetModified(FALSE,FALSE);
	m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, (1<<IMAGE_INPUT)|(1<<IMAGE_RELAY)|(1<<IMAGE_CAMERA));
//	m_pParent->OnInputChanged();
//	m_pParent->OnCameraChanged();
//	m_pParent->OnRelaisChanged();
}

/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::CancelChanges()
{
	LoadJaCobSettings();
	UpdateData(FALSE);
	EnableDlgControlsJaCob();

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
void CJaCobPage::OnCheckSync() 
{
	UpdateData(TRUE);
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::OnCheckUseGlobalColorSetting() 
{
	UpdateData(TRUE);
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::OnCheckJaCob() 
{
	UpdateData(TRUE);
	if (m_bJaCob)
	{
		SetModified();
	}
	else
	{
		if (AfxMessageBox(IDP_HARDWARE_GROUP_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO )
			m_bJaCob	= TRUE;
		else
			SetModified();
	}
	EnableDlgControlsJaCob();
}

/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::LoadJaCobSettings()
{
	// Ist eine JaCob im System eingetragen?
	if (m_nJaCobNr == JACOB1)
		m_bJaCob = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_JACOBUNIT1, _T("")) != _T(""));
	else if(m_nJaCobNr == JACOB2)
		m_bJaCob = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_JACOBUNIT2, _T("")) != _T(""));
	else if(m_nJaCobNr == JACOB3)
		m_bJaCob = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_JACOBUNIT3, _T("")) != _T(""));
	else if(m_nJaCobNr == JACOB4)
		m_bJaCob = (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_JACOBUNIT4, _T("")) != _T(""));
	
	
	m_bMDOn	 = FALSE;
	if (GetDongle().AllowMotionDetection())
	{
		m_bMDOn	 = (GetProfile().GetInt(m_sJaCobMD,_T("Activate"),0) != 0);
	}


	// synchrone und asynchrone Kameras
	m_bSyncCam					= GetProfile().GetInt(m_sJaCobGeneral, _T("CameraType"), FALSE);
	m_bUse2Fields				= GetProfile().GetInt(m_sJaCobGeneral, _T("StoreField"), FALSE);
	m_bUseGlobalColorSettings	= GetProfile().GetInt(m_sJaCobFrontend, _T("UseGlobalColorSetting"), TRUE);

	CString sNorm =	GetProfile().GetString(m_sJaCobFrontend,_T("VideoNormDetection"),_T("auto"));

	if (0==sNorm.CompareNoCase(_T("forcepal")))
	{
		m_iVideoNorm = 1;
	}
	else if (0==sNorm.CompareNoCase(_T("forcentsc")))
	{
		m_iVideoNorm = 2;
	}
}
					    
/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::SaveJaCobSettings()
{
	// Kameratype und GlobalColorSetting sichern
	GetProfile().WriteInt(m_sJaCobGeneral, _T("CameraType"), m_bSyncCam);
	GetProfile().WriteInt(m_sJaCobFrontend, _T("UseGlobalColorSetting"), m_bUseGlobalColorSettings);

	CString sNorm;
	switch (m_iVideoNorm)
	{
	case 0:
		sNorm = _T("auto");
		break;
	case 1:
		sNorm = _T("forcepal");
		break;
	case 2:
		sNorm = _T("forcentsc");
		break;
	}
	GetProfile().WriteString(m_sJaCobFrontend,_T("VideoNormDetection"),sNorm);
	
	if (m_bUse2Fields)
	{
		GetProfile().WriteInt(m_sJaCobGeneral, _T("StoreField"), 2);
		GetProfile().WriteInt(szSecurityServerFormat, m_sJaCobFps, 50);
	}
	else								   
	{
		GetProfile().WriteInt(m_sJaCobGeneral, _T("StoreField"), 0);
		GetProfile().WriteInt(szSecurityServerFormat, m_sJaCobFps, 25);
	}

	GetProfile().WriteInt(m_sJaCobMD, _T("Activate"), m_bMDOn);

	// JaCobUnit beenden
	if (m_bRestartJaCobUnit)
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
void CJaCobPage::EnableDlgControlsJaCob()
{
	// Enable/Disable Dialogcontrols
	m_checkUseGlobalColorSettings.EnableWindow(m_bJaCob);
	m_checkSync.EnableWindow(m_bJaCob);
	m_checkUse2Fields.EnableWindow(m_bJaCob);									 
	m_checkMDOn.EnableWindow(m_bJaCob && GetDongle().AllowMotionDetection());

	// Die 1. JaCob darf nicht ausgetragen werden, solange eine 2. JaCob eingetragen ist
	if (m_nJaCobNr == JACOB1)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(), WK_APP_NAME_JACOBUNIT2, _T("")) != _T(""))
			m_checkJaCob.EnableWindow(!m_bJaCob);
	}
	// Die 2. JaCob darf nicht ausgetragen werden, solange eine 3. JaCob eingetragen ist
	if (m_nJaCobNr == JACOB2)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(), WK_APP_NAME_JACOBUNIT3, _T("")) != _T(""))
			m_checkJaCob.EnableWindow(!m_bJaCob);
	}
	// Die 3. JaCob darf nicht ausgetragen werden, solange eine 4. JaCob eingetragen ist
	if (m_nJaCobNr == JACOB3)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(), WK_APP_NAME_JACOBUNIT4, _T("")) != _T(""))
			m_checkJaCob.EnableWindow(!m_bJaCob);
	}

	// Die 2. JACOB darf nur eingetragen werden, wenn die 1. JACOB ebenfalls eingetragen ist
	if (m_nJaCobNr == JACOB2)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_JACOBUNIT1, _T("")) == _T(""))
			m_checkJaCob.EnableWindow(FALSE);
	}
	// Die 3. JACOB darf nur eingetragen werden, wenn die 2. JACOB ebenfalls eingetragen ist
	if (m_nJaCobNr == JACOB3)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_JACOBUNIT2, _T("")) == _T(""))
			m_checkJaCob.EnableWindow(FALSE);
	}
	// Die 4. JACOB darf nur eingetragen werden, wenn die 3. JACOB ebenfalls eingetragen ist
	if (m_nJaCobNr == JACOB4)
	{
		if (GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_JACOBUNIT3, _T("")) == _T(""))
			m_checkJaCob.EnableWindow(FALSE);
	}

	BOOL bEnable = m_bJaCob && m_bMDOn && WK_IS_RUNNING(m_sAppName);
	GetDlgItem(IDC_MD_CONFIG)->EnableWindow(bEnable);	

	// gf Es werden keine Variablen verändert, warum also Controls updaten?
//	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::GetDlgJaCobData()
{
	CInputGroup* pInputGroup = NULL;
	CInputGroup* pInputGroupMD = NULL;
	COutputGroup* pOutputGroup = NULL;
	int iAnzahl = 0;

	// Anzahl der Inputs einlesen
	iAnzahl = NR_INPUTS_JACOB;
	
	// Daten der Inputgruppe aktualisieren, wenn bereits vorhanden...
	pInputGroup = m_pInputList->GetGroupBySMName(m_sSM_JaCobInput);
	if ( pInputGroup)
	{
		pInputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else
	{
		pInputGroup = m_pInputList->AddInputGroup(NULL, iAnzahl, m_sSM_JaCobInput);
	}

	if (m_bJaCob)
	{
		iAnzahl = NR_OUTPUTS_CAMERA_JACOB;
		if (GetDongle().RestrictedTo8CamerasPerBoard())
		{
			iAnzahl = NR_OUTPUTS_CAMERA_JACOB_RESTRICTED;
		}
	}

	if (m_bMDOn)
	{
		// Daten der MD-Inputgruppe aktualisieren, wenn bereits vorhanden...
		pInputGroupMD = m_pInputList->GetGroupBySMName(m_sSM_JaCobInputMD);
		if (pInputGroupMD)
		{
			pInputGroupMD->SetSize((WORD)iAnzahl);	// Nur die 16 Kameras der JaCob sollen MotionDetection unterstützen
		}
		// ...sonst neue Gruppe anlegen
		else
		{
			pInputGroupMD = m_pInputList->AddInputGroup(NULL, iAnzahl, m_sSM_JaCobInputMD);
		}
	}
	else
	{
		m_pInputList->DeleteGroup(m_sSM_JaCobInputMD);
	}

	// Daten der Kamera-Outputgruppe aktualisieren, wenn bereits vorhanden...
	pOutputGroup = m_pOutputList->GetGroupBySMName(m_sSM_JaCobOutputCameras);
	if ( pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(NULL, iAnzahl, m_sSM_JaCobOutputCameras);
	}

	// Anzahl der Relay Outputs einlesen
	if (m_bJaCob)
		iAnzahl = NR_OUTPUTS_RELAY_JACOB;
	
	// Daten der Relay-Outputgruppe aktualisieren, wenn bereits vorhanden...
	pOutputGroup = m_pOutputList->GetGroupBySMName( m_sSM_JaCobOutputRelays);
	if ( pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(NULL, iAnzahl, m_sSM_JaCobOutputRelays);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::OnCheckUse2Fields() 
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
void CJaCobPage::OnCheckMdOn() 
{
	UpdateData(TRUE);
	if (m_bMDOn == FALSE)
	{
		SetSelection(m_checkMDOn.GetDlgCtrlID());
		CString sMsg;
		sMsg.Format(IDP_MD_OFF_WARNING);
		if (IDYES == AfxMessageBox(sMsg, MB_YESNO|MB_DEFBUTTON2))
		{
			SetModified();
		}
		else
		{
			m_bMDOn = TRUE;
			UpdateData(FALSE);
		}
		SetSelection(0);
	}
	else
	{
		SetModified();
	}
}
/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::SetModified(BOOL bModified, BOOL bServerInit)
{
	// Basisklasse aufrufen
	CSVPage::SetModified(bModified, bServerInit);
	m_bRestartJaCobUnit	= TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::OnMdConfig() 
{
	if (WK_IS_RUNNING(m_sAppName))
	{
		CWnd *pWnd = FindWindow(NULL, m_sAppName);
		if (pWnd && IsWindow(pWnd->m_hWnd) && m_bMDOn)
		{
			pWnd->PostMessage(JACOB_OPEN_MD_DLG, 0, (LPARAM)this->m_hWnd);
			pWnd->SetWindowPos(&wndTop, 0,0,0,0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::OnRadioAuto() 
{
	UpdateData(TRUE);
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::OnRadioNtsc() 
{
	UpdateData(TRUE);
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::OnRadioPal() 
{
	UpdateData(TRUE);
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CJaCobPage::OnIdleSVPage()
{
	// some controls are dependent from the running module
	EnableDlgControlsJaCob();
}
