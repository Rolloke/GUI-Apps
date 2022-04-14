// MiCoPage.cpp : implementation file
//
					 
#include "stdafx.h"
#include "systemverwaltung.h"
#include "MiCoPage.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szMiCoPCISystem[]		= _T("MiCoUnitPCI");
static TCHAR BASED_CODE szMiCoUnitPCIExe[]		= _T("MiCoUnitPCI.exe");

static TCHAR BASED_CODE szMiCoIOBase[]			= _T("MiCoIOBase");
static TCHAR BASED_CODE szMiCoMemBase[]			= _T("MemoryIOBase");
static TCHAR BASED_CODE szExtCard1IOBase[]		= _T("ExtCard1IOBase");
static TCHAR BASED_CODE szExtCard2IOBase[]		= _T("ExtCard2IOBase");
static TCHAR BASED_CODE szExtCard3IOBase[]		= _T("ExtCard3IOBase");
static TCHAR BASED_CODE szIRQ[]					= _T("IRQ");
static TCHAR BASED_CODE szMiCoBusType[]			= _T("BusType");

static TCHAR BASED_CODE szMiCoSystemFormat[]	= _T("MiCoUnitPCI\\Device%d\\System");
static TCHAR BASED_CODE szMiCoGeneralFormat[]	= _T("MiCoUnitPCI\\Device%d\\General");
static TCHAR BASED_CODE szMiCoFrontendFormat[]	= _T("MiCoUnitPCI\\Device%d\\FrontEnd");
static TCHAR BASED_CODE szMiCoBackendFormat[]	= _T("MiCoUnitPCI\\Device%d\\BackEnd");

static TCHAR BASED_CODE szAddressFormat[]		= _T("0x%03lx");

static TCHAR BASED_CODE szMiCoFpsFormat[] = _T("MiCoFPS%d");
static TCHAR BASED_CODE szSecurityServerFormat[] = _T("SecurityServer");

/////////////////////////////////////////////////////////////////////////////
// CMiCoPage dialog

CMiCoPage::CMiCoPage(CSVView* pParent, int nMiCoNr) : CSVPage(CMiCoPage::IDD)
{
	m_pParent			= pParent;
	m_nMiCoNr			= nMiCoNr;
	m_pInputList		= pParent->GetDocument()->GetInputs();
	m_pOutputList		= pParent->GetDocument()->GetOutputs();
	m_bRestartMiCoUnit	= FALSE;
	m_bMiCo				= FALSE;

	// Einige Referenzen zur Codevereinfachung
	m_pcbAddressExtCard[0]	= &m_cbAddressMiCo;
	m_pcbAddressExtCard[1]	= &m_cbAddressExtCard1;
	m_pcbAddressExtCard[2]	= &m_cbAddressExtCard2;
	m_pcbAddressExtCard[3]	= &m_cbAddressExtCard3;
	m_psAddressExtCard[0]	= &m_sAddressMiCo;
	m_psAddressExtCard[1]	= &m_sAddressExtCard1;
	m_psAddressExtCard[2]	= &m_sAddressExtCard2;
	m_psAddressExtCard[3]	= &m_sAddressExtCard3;
	m_pbExtCard[0]			= &m_bMiCoISA;
	m_pbExtCard[1]			= &m_bExtCard1;
	m_pbExtCard[2]			= &m_bExtCard2;
	m_pbExtCard[3]			= &m_bExtCard3;
	
	CString sMelder,sKamera,sRelais;
	sMelder.LoadString(IDS_ACTIVATION_INPUT);
	sKamera.LoadString(IDS_ACTIVATION_OUTPUT);
	sRelais.LoadString(IDS_RELAY);

	// Richtigen SharedMemorynamen und Defaultnamen wählen
	if (nMiCoNr == MICO1)
	{
		m_sSM_MiCoOutputCameras = SM_MICO_OUTPUT_CAMERAS;
		m_sSM_MiCoOutputRelays	= SM_MICO_OUTPUT_RELAYS;
		m_sSM_MiCoInput			= SM_MICO_INPUT;
		m_sMelderDefaultName	= _T("MiCo1 ") + sMelder;
		m_sCameraDefaultname	= _T("MiCo1 ") + sKamera;
		m_sRelayDefaultName		= _T("MiCo1 ") + sRelais;
		m_sAppName				= WK_APP_NAME_MICOUNIT;
	}
	else if (nMiCoNr == MICO2)
	{
		m_sSM_MiCoOutputCameras = SM_MICO_OUTPUT_CAMERAS2;
		m_sSM_MiCoOutputRelays	= SM_MICO_OUTPUT_RELAYS2;
		m_sSM_MiCoInput			= SM_MICO_INPUT2;
		m_sMelderDefaultName	= _T("MiCo2 ") + sMelder;
		m_sCameraDefaultname	= _T("MiCo2 ") + sKamera;
		m_sRelayDefaultName		= _T("MiCo2 ") + sRelais;
		m_sAppName				= WK_APP_NAME_MICOUNIT2;
	}
	else if (nMiCoNr == MICO3)
	{
		m_sSM_MiCoOutputCameras = SM_MICO_OUTPUT_CAMERAS3;
		m_sSM_MiCoOutputRelays	= SM_MICO_OUTPUT_RELAYS3;
		m_sSM_MiCoInput			= SM_MICO_INPUT3;
		m_sMelderDefaultName	= _T("MiCo3 ") + sMelder;
		m_sCameraDefaultname	= _T("MiCo3 ") + sKamera;
		m_sRelayDefaultName		= _T("MiCo3 ") + sRelais;
		m_sAppName				= WK_APP_NAME_MICOUNIT3;
	}
	else if (nMiCoNr == MICO4)
	{
		m_sSM_MiCoOutputCameras = SM_MICO_OUTPUT_CAMERAS4;
		m_sSM_MiCoOutputRelays	= SM_MICO_OUTPUT_RELAYS4;
		m_sSM_MiCoInput			= SM_MICO_INPUT4;
		m_sMelderDefaultName	= _T("MiCo4 ") + sMelder;
		m_sCameraDefaultname	= _T("MiCo4 ") + sKamera;
		m_sRelayDefaultName		= _T("MiCo4 ") + sRelais;
		m_sAppName				= WK_APP_NAME_MICOUNIT4;
	}
	else
		WK_TRACE_ERROR(_T("Unknown MiCoNr (%u)\n"), nMiCoNr);

	// Registrypath in abhängigkeit der gewählten MiCo-Karte (1. oder 2. MiCo) 
	m_sMiCoSystem.Format(szMiCoSystemFormat, m_nMiCoNr);
	m_sMiCoGeneral.Format(szMiCoGeneralFormat, m_nMiCoNr);
	m_sMiCoFrontend.Format(szMiCoFrontendFormat, m_nMiCoNr);
	m_sMiCoBackend.Format(szMiCoBackendFormat, m_nMiCoNr);
	m_sMiCoFps.Format(szMiCoFpsFormat, m_nMiCoNr);

	//{{AFX_DATA_INIT(CMiCoPage)
	m_sIRQ						= _T("");
	m_sAddressMiCo				= _T("");
	m_sAddressExtCard1			= _T("");
	m_sAddressExtCard2			= _T("");
	m_sAddressExtCard3			= _T("");
	m_bExtCard1					= FALSE;
	m_bExtCard2					= FALSE;
	m_bExtCard3					= FALSE;
	m_bSyncCam					= FALSE;
	m_bUseGlobalColorSettings	= TRUE;
	m_bMiCoPCI					= FALSE;
	m_nRadioVideoFormat			= 0;
	m_bMiCoISA					= FALSE;
	m_bUse2Fields				= FALSE;
	//}}AFX_DATA_INIT
	Create(IDD,pParent);
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMiCoPage)
	DDX_Control(pDX, IDC_CHECK_USE2FIELDS, m_checkUse2Fields);
	DDX_Control(pDX, IDC_RADIO_NTSC_SQUARE,			m_radioNtscSquare);
	DDX_Control(pDX, IDC_RADIO_NTSC_CCIR,			m_radioNtscCCIR);
	DDX_Control(pDX, IDC_RADIO_PAL_SQUARE,			m_radioPalSquare);
	DDX_Control(pDX, IDC_RADIO_PAL_CCIR,			m_radioPalCCIR);
	DDX_Control(pDX, IDC_CHECK_MICO_PCI,			m_checkMiCoPCI);
	DDX_Control(pDX, IDC_CHECK_SYNC,				m_checkSync);
	DDX_Control(pDX, IDC_CHECK_GLOBAL_COLORS,	    m_checkUseGlobalColorSettings);
	DDX_Control(pDX, IDC_CHECK_EXTCARD_3,			m_checkExtCard3);
	DDX_Control(pDX, IDC_CHECK_EXTCARD_2,			m_checkExtCard2);
	DDX_Control(pDX, IDC_CHECK_EXTCARD_1,			m_checkExtCard1);
	DDX_Control(pDX, IDC_CHECK_MICO_ISA,			m_checkMiCoISA);
	DDX_Control(pDX, IDC_ADDRESS_EXTCARD_3,			m_cbAddressExtCard3);
	DDX_Control(pDX, IDC_ADDRESS_EXTCARD_2,			m_cbAddressExtCard2);
	DDX_Control(pDX, IDC_ADDRESS_EXTCARD_1,			m_cbAddressExtCard1);
	DDX_Control(pDX, IDC_ADDRESS_MICO,				m_cbAddressMiCo);
	DDX_Control(pDX, IDC_IRQ_MICO,					m_cbIRQ);
	DDX_CBString(pDX, IDC_IRQ_MICO,					m_sIRQ);
	DDX_CBString(pDX, IDC_ADDRESS_MICO,				m_sAddressMiCo);
	DDX_CBString(pDX, IDC_ADDRESS_EXTCARD_1,		m_sAddressExtCard1);
	DDX_CBString(pDX, IDC_ADDRESS_EXTCARD_2,		m_sAddressExtCard2);
	DDX_CBString(pDX, IDC_ADDRESS_EXTCARD_3,		m_sAddressExtCard3);
	DDX_Check(pDX, IDC_CHECK_MICO_ISA,				m_bMiCoISA);
	DDX_Check(pDX, IDC_CHECK_EXTCARD_1,				m_bExtCard1);
	DDX_Check(pDX, IDC_CHECK_EXTCARD_2,				m_bExtCard2);
	DDX_Check(pDX, IDC_CHECK_EXTCARD_3,				m_bExtCard3);
	DDX_Check(pDX, IDC_CHECK_SYNC,					m_bSyncCam);
	DDX_Check(pDX, IDC_CHECK_GLOBAL_COLORS,			m_bUseGlobalColorSettings);
	DDX_Check(pDX, IDC_CHECK_MICO_PCI,				m_bMiCoPCI);
	DDX_Radio(pDX, IDC_RADIO_PAL_CCIR,				m_nRadioVideoFormat);
	DDX_Check(pDX, IDC_CHECK_USE2FIELDS, m_bUse2Fields);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CMiCoPage, CSVPage)
	//{{AFX_MSG_MAP(CMiCoPage)
	ON_CBN_SELCHANGE(IDC_ADDRESS_MICO,				OnSelchangeAddressMiCo)
	ON_CBN_SELCHANGE(IDC_ADDRESS_EXTCARD_1,			OnSelchangeAddressExtCard1)
	ON_CBN_SELCHANGE(IDC_ADDRESS_EXTCARD_2,			OnSelchangeAddressExtCard2)
	ON_CBN_SELCHANGE(IDC_ADDRESS_EXTCARD_3,			OnSelchangeAddressExtCard3)
	ON_BN_CLICKED(IDC_CHECK_MICO_ISA,				OnCheckMiCoISA)
	ON_BN_CLICKED(IDC_CHECK_EXTCARD_1,				OnCheckExtCard1)
	ON_BN_CLICKED(IDC_CHECK_EXTCARD_2,				OnCheckExtCard2)
	ON_BN_CLICKED(IDC_CHECK_EXTCARD_3,				OnCheckExtCard3)
	ON_CBN_SELCHANGE(IDC_IRQ_MICO,					OnSelchangeIrq)
	ON_BN_CLICKED(IDC_CHECK_SYNC,					OnCheckSync)
	ON_BN_CLICKED(IDC_CHECK_GLOBAL_COLORS,			OnCheckUseGlobalColorSetting)
	ON_BN_CLICKED(IDC_CHECK_MICO_PCI,				OnCheckMicoPCI)
	ON_BN_CLICKED(IDC_RADIO_NTSC_CCIR,				OnRadioNtscCcir)
	ON_BN_CLICKED(IDC_RADIO_NTSC_SQUARE,			OnRadioNtscSquare)
	ON_BN_CLICKED(IDC_RADIO_PAL_CCIR,				OnRadioPalCcir)
	ON_BN_CLICKED(IDC_RADIO_PAL_SQUARE,				OnRadioPalSquare)
	ON_BN_CLICKED(IDC_CHECK_USE2FIELDS, OnCheckUse2Fields)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiCoPage message handlers
BOOL CMiCoPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	// MiCo Einstellungen aus der Registry laden.
	CancelChanges();

	if (!IsLocal())
	{
		DisableAll();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
BOOL CMiCoPage::IsDataValid()
{
	CString sMiCoCard[5][4];
	CString sMiCoIOBaseKey[4];
	CString sCoCoIOBaseKey[4];
	CString sCoCoCard[4];

	// Array der Fehlermeldungen bei Resourcenkonflikten
	sMiCoCard[1][0]		="1.MiCo";
	sMiCoCard[1][1]		="1.MiCo/1.VideoPlus8";
	sMiCoCard[1][2]		="1.MiCo/2.VideoPlus8";
	sMiCoCard[1][3]		="1.MiCo/3.VideoPlus8";

	sMiCoCard[2][0]		="2.MiCo";
	sMiCoCard[2][1]		="2.MiCo/1.VideoPlus8";
	sMiCoCard[2][2]		="2.MiCo/2.VideoPlus8";
	sMiCoCard[2][3]		="2.MiCo/3.VideoPlus8";

	sMiCoCard[3][0]		="3.MiCo";
	sMiCoCard[3][1]		="3.MiCo/1.VideoPlus8";
	sMiCoCard[3][2]		="3.MiCo/2.VideoPlus8";
	sMiCoCard[3][3]		="3.MiCo/3.VideoPlus8";
	
	sMiCoCard[4][0]		="4.MiCo";
	sMiCoCard[4][1]		="4.MiCo/1.VideoPlus8";
	sMiCoCard[4][2]		="4.MiCo/2.VideoPlus8";
	sMiCoCard[4][3]		="4.MiCo/3.VideoPlus8";

	sCoCoCard[0]		="CoCo";
	sCoCoCard[1]		="CoCo/1.VideoPlus8";
	sCoCoCard[2]		="CoCo/2.VideoPlus8";
	sCoCoCard[3]		="CoCo/3.VideoPlus8";

	sMiCoIOBaseKey[0]	= "MiCoIOBase";
	sMiCoIOBaseKey[1]	= "ExtCard1IOBase";
	sMiCoIOBaseKey[2]	= "ExtCard2IOBase";
	sMiCoIOBaseKey[3]	= "ExtCard3IOBase";

	sCoCoIOBaseKey[0]	= "IOBase";
	sCoCoIOBaseKey[1]	= "ExtCard1";
	sCoCoIOBaseKey[2]	= "ExtCard2";
	sCoCoIOBaseKey[3]	= "ExtCard3";

	CString sAddr		= "";
	CString sIRQ		= "";
	CString sMsg		= "";
	TCHAR szBuffer[256] = {0};

	UpdateData();
					  
	//*******************************************
	// Sucht nach doppelt benutzten Basisadressen
	//*******************************************
	for (int nI = 0; nI <= 3; nI++)
	{
		if (*m_pbExtCard[nI]) 
		{
			for (int nJ = nI+1; nJ <= 3; nJ++)
			{
				if (*m_pbExtCard[nJ]) 
				{
					if (*m_psAddressExtCard[nI] == *m_psAddressExtCard[nJ])
					{
						sMsg.Format(IDP_HARDWARE_ADDRESS_DOUBLE, sMiCoCard[m_nMiCoNr][nI], sMiCoCard[m_nMiCoNr][nJ], *m_psAddressExtCard[nI]);
						AfxMessageBox(sMsg, MB_OK);					  
						return FALSE;
					}
				}

			}

			for (nJ = 0; nJ <= 3; nJ++)
			{									  
				for (int nK = 1; nK <= 4; nK++)
				{
					if (nK != m_nMiCoNr)
					{
						// Vergleich von MiCo1-CoVi1 mit MiCo1-CoVi1...MiCo2-CoVi3
						CString sOtherMiCoSystem;
						
						sOtherMiCoSystem.Format(_T("MiCoUnitPCI\\Device%u\\System"), nK);

						sAddr.Format(_T("0x%x"), GetProfile().GetInt(sOtherMiCoSystem, sMiCoIOBaseKey[nJ], 0));

						if (*m_psAddressExtCard[nI] == sAddr)
						{
							sMsg.Format(IDP_HARDWARE_ADDRESS_DOUBLE, sMiCoCard[m_nMiCoNr][nI], sMiCoCard[nK][nJ], sAddr);
							AfxMessageBox(sMsg, MB_OK);
							return FALSE;
						}
						
						// Vergleich von MiCo-CoVi1 mit CoCo-CoVi1...CoCo-CoVi3
						GetPrivateProfileString(_T("CoCoISA0"), sCoCoIOBaseKey[nJ], _T("0"), szBuffer, sizeof(szBuffer), theApp.GetSytemIni());	
						sAddr.Format(_T("0x%s"), szBuffer);
						if (*m_psAddressExtCard[nI] == sAddr) 
						{
							sMsg.Format(IDP_HARDWARE_ADDRESS_DOUBLE, sMiCoCard[m_nMiCoNr][nI], sCoCoCard[nJ], sAddr);
							AfxMessageBox(sMsg, MB_OK);
							return FALSE;
						}
					}
				}
			}
		}
	}

	//************************************
	// Sucht nach doppelt vergebenen IRQ's
	//************************************
	if (m_cbIRQ.GetCurSel() != CB_ERR )
	{
		for (int nK = 1; nK <= 4; nK++)
		{
			// Nicht mit sich selber testen
			if (nK != m_nMiCoNr)
			{
				CString sOtherMiCoSystem;
				
				sOtherMiCoSystem.Format(_T("MiCoUnitPCI\\Device%u\\System"), nK);

				sIRQ.Format(_T("%d"), GetProfile().GetInt(sOtherMiCoSystem, szIRQ, 0));

				if (m_sIRQ == sIRQ)
				{
					sMsg.Format(IDP_HARDWARE_IRQ_DOUBLE, sMiCoCard[m_nMiCoNr][0], sMiCoCard[nK][0], sIRQ);
					AfxMessageBox(sMsg, MB_OK);
					return FALSE;
				}
			}
		}
		
		GetPrivateProfileString(_T("CoCoISA0"), _T("AVPIRQ"), _T(""), szBuffer, sizeof(szBuffer), theApp.GetSytemIni());
		sIRQ = szBuffer;
		if (m_sIRQ == sIRQ)
		{
			sMsg.Format(IDP_HARDWARE_IRQ_DOUBLE, sMiCoCard[m_nMiCoNr][0], sCoCoCard[0], sIRQ);
			AfxMessageBox(sMsg, MB_OK);
			return FALSE;
		}
	}
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::SaveChanges()
{
	WK_TRACE_USER(_T("MiCo-Einstellungen wurden geändert\n"));
	UpdateData(TRUE);

	// MiCo aktualisieren
	if ( m_bMiCo ) 
	{
		GetDlgMiCoData();
	}
	else 
	{
		// Gruppen loeschen
		CProcessList* pProcessList = m_pParent->GetDocument()->GetProcesses();
		CString sGroupname (_T("MiCo"));
		DeleteProcessesOfExtensionCards(pProcessList, m_pOutputList, sGroupname);
		
		m_pInputList->DeleteGroup(m_sSM_MiCoInput);
		m_pOutputList->DeleteGroup(m_sSM_MiCoOutputCameras);
		m_pOutputList->DeleteGroup(m_sSM_MiCoOutputRelays);
	}

	// Einstellunge speichern
	SaveMiCoSettings();

	if (m_nMiCoNr == MICO1)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_MICOUNIT,m_bMiCo ? szMiCoUnitPCIExe : _T(""));
	else if(m_nMiCoNr == MICO2)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_MICOUNIT2,m_bMiCo ? szMiCoUnitPCIExe : _T(""));
	else if(m_nMiCoNr == MICO3)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_MICOUNIT3,m_bMiCo ? szMiCoUnitPCIExe : _T(""));
	else if(m_nMiCoNr == MICO4)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_MICOUNIT4,m_bMiCo ? szMiCoUnitPCIExe : _T(""));

	SaveInputsAndOutputsAfterCheckingActivations();
	SetModified(FALSE,FALSE);
	m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, (1<<IMAGE_INPUT)|(1<<IMAGE_RELAY)|(1<<IMAGE_CAMERA));
//	m_pParent->OnInputChanged();
//	m_pParent->OnCameraChanged();
//	m_pParent->OnRelaisChanged();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::CancelChanges()
{
	LoadMiCoSettings();

	UpdateData(FALSE);

	EnableDlgControlsMiCo();

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
void CMiCoPage::OnSelchangeIrq() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnSelchangeAddressMiCo() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnSelchangeAddressExtCard1() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnSelchangeAddressExtCard2() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnSelchangeAddressExtCard3() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnCheckSync() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnCheckUseGlobalColorSetting() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnCheckMiCoISA() 
{
	UpdateData(TRUE);
	if (m_bMiCoISA)
	{
		m_bMiCoPCI	= FALSE;
		if (m_nMiCoNr == MICO1)
		{
			m_cbAddressMiCo.SetCurSel(m_cbAddressMiCo.FindStringExact(-1, _T("0x200")));
			m_cbIRQ.SetCurSel(m_cbIRQ.FindStringExact(-1, _T("10")));
		}
		else  
		{
			m_cbAddressMiCo.SetCurSel(m_cbAddressMiCo.FindStringExact(-1, _T("0x280")));
			m_cbIRQ.SetCurSel(m_cbIRQ.FindStringExact(-1, _T("12")));
		}

		SetModified();
	}
	else
	{
		if (AfxMessageBox(IDP_HARDWARE_GROUP_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO )
		{
			m_bMiCoISA	= TRUE;
			m_bMiCoPCI	= FALSE;
		}
		else
		{
			SetModified();
		}
	}
	m_bMiCo		= m_bMiCoISA | m_bMiCoPCI;
	EnableDlgControlsMiCo();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnCheckMicoPCI() 
{
	UpdateData(TRUE);
	if (m_bMiCoPCI)
	{
		m_bMiCoISA	= FALSE;
		// PCI kann z.Z. kein Square Pixel
		m_nRadioVideoFormat = 0;
		SetModified();
	}
	else
	{
		if (AfxMessageBox(IDP_HARDWARE_GROUP_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO )
		{
			m_bMiCoPCI	= TRUE;
			m_bMiCoISA	= FALSE;
		}
		else
		{
			SetModified();
		}
	}
	m_bMiCo		= m_bMiCoISA | m_bMiCoPCI;
	EnableDlgControlsMiCo();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnCheckExtCard1() 
{
	UpdateData(TRUE);
	if (m_bExtCard1)
	{
		m_cbAddressExtCard1.SetCurSel(m_cbAddressExtCard1.FindStringExact(-1, _T("0x220")));
		SetModified();
	}
	else
	{
		if (AfxMessageBox(IDP_HARDWARE_PARTIAL_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO)
		{
			m_bExtCard1 = TRUE;
		}
		else
		{
			SetModified();
		}
	}
	EnableDlgControlsMiCo();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnCheckExtCard2() 
{
	UpdateData(TRUE);
	if (m_bExtCard2)
	{
		m_cbAddressExtCard2.SetCurSel(m_cbAddressExtCard2.FindStringExact(-1, _T("0x240")));
		SetModified();
	}
	else
	{
		if (AfxMessageBox(IDP_HARDWARE_PARTIAL_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO)
		{
			m_bExtCard2 = TRUE;
		}
		else {
			SetModified();
		}
	}
	EnableDlgControlsMiCo();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnCheckExtCard3() 
{
	UpdateData(TRUE);
	if (m_bExtCard3)
	{
		m_cbAddressExtCard3.SetCurSel(m_cbAddressExtCard3.FindStringExact(-1, _T("0x320")));
		SetModified();
	}
	else
	{
		if (AfxMessageBox(IDP_HARDWARE_PARTIAL_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO)
		{
			m_bExtCard3 = TRUE;
		}
		else
		{
			SetModified();
		}
	}
	EnableDlgControlsMiCo();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::LoadMiCoSettings()
{
	// Ist eine MiCoISA, MiCoPCI oder gar keine MiCo im System eingetragen?
	m_bMiCoISA = (GetProfile().GetString(m_sMiCoSystem,szMiCoBusType,_T("Unknown")) == _T("ISA"));
	m_bMiCoPCI = (GetProfile().GetString(m_sMiCoSystem,szMiCoBusType,_T("Unknown")) == _T("PCI"));

	// Ist überhaupt eine MiCo eingetragen?
	m_bMiCo = m_bMiCoISA | m_bMiCoPCI;

	// Anhand der IO-Adressen wird bestimmt welche CoVis derzeit im System eingetragen sind.
	m_bExtCard1 = (GetProfile().GetInt(m_sMiCoSystem,szExtCard1IOBase,0) != 0);
	m_bExtCard2 = (GetProfile().GetInt(m_sMiCoSystem,szExtCard2IOBase,0) != 0);
	m_bExtCard3 = (GetProfile().GetInt(m_sMiCoSystem,szExtCard3IOBase,0) != 0);
	
	int iIndex = -1;
	DWORD dwT;

	// MiCo
	if (m_bMiCoISA)
	{
		// IO-Adressen einlesen
		dwT = GetProfile().GetInt(m_sMiCoSystem,szMiCoIOBase,0);
		if (dwT)
		{
			m_sAddressMiCo.Format(szAddressFormat,dwT);
		}
		else
		{
			m_sAddressMiCo.Empty();
		}

		if (!m_sAddressMiCo.IsEmpty()) 
		{
			iIndex = m_cbAddressMiCo.FindStringExact(-1, m_sAddressMiCo);
			m_cbAddressMiCo.SetCurSel(iIndex);
		}
	}
	else 
	{
		m_cbAddressMiCo.SetCurSel(-1);
	}

	// 1. CoVi8
	if (m_bExtCard1)
	{
		// IO-Adressen einlesen
		dwT = GetProfile().GetInt(m_sMiCoSystem,szExtCard1IOBase,0);
		if (dwT)
		{
			m_sAddressExtCard1.Format(szAddressFormat,dwT);
		}
		else
		{
			m_sAddressExtCard1.Empty();
		}
		if (!m_sAddressExtCard1.IsEmpty()) 
		{
			iIndex = m_cbAddressExtCard1.FindStringExact(-1, m_sAddressExtCard1);
			m_cbAddressExtCard1.SetCurSel(iIndex);
		}
	}
	else 
	{
		m_cbAddressExtCard1.SetCurSel(-1);
	}

	// 2. CoVi8
	if (m_bExtCard2)
	{
		// IO-Adressen einlesen
		dwT = GetProfile().GetInt(m_sMiCoSystem,szExtCard2IOBase,0);
		if (dwT)
		{
			m_sAddressExtCard2.Format(szAddressFormat,dwT);
		}
		else
		{
			m_sAddressExtCard2.Empty();
		}
		if (!m_sAddressExtCard2.IsEmpty()) 
		{
			iIndex = m_cbAddressExtCard2.FindStringExact(-1, m_sAddressExtCard2);
			m_cbAddressExtCard2.SetCurSel(iIndex);
		}
	}
	else 
	{
		m_cbAddressExtCard2.SetCurSel(-1);
	}

	// 3. CoVi8
	if (m_bExtCard3)
	{
		// IO-Adressen einlesen
		dwT = GetProfile().GetInt(m_sMiCoSystem,szExtCard3IOBase,0);
		if (dwT)
		{
			m_sAddressExtCard3.Format(szAddressFormat,dwT);
		}
		else
		{
			m_sAddressExtCard3.Empty();
		}
		if (!m_sAddressExtCard3.IsEmpty()) 
		{
			iIndex = m_cbAddressExtCard3.FindStringExact(-1, m_sAddressExtCard3);
			m_cbAddressExtCard3.SetCurSel(iIndex);
		}
	}
	else 
	{
		m_cbAddressExtCard3.SetCurSel(-1);
	}

	// IRQ einlesen
	dwT = GetProfile().GetInt(m_sMiCoSystem,szIRQ,0);
	if (dwT)
	{
		m_sIRQ.Format(_T("%d"),dwT);
	}
	else
	{
		m_sIRQ.Empty();
	}

	if (!m_sIRQ.IsEmpty()) 
	{
		iIndex = m_cbIRQ.FindStringExact(-1, m_sIRQ);
		if (iIndex != CB_ERR)
			m_cbIRQ.SetCurSel(iIndex);
	}
	else 
	{
		m_cbIRQ.SetCurSel(-1);
	}

	// synchrone und asynchrone Kameras
	m_bSyncCam					= GetProfile().GetInt(m_sMiCoGeneral, _T("CameraType"), FALSE);
	m_bUse2Fields				= GetProfile().GetInt(m_sMiCoGeneral, _T("StoreField"), FALSE);
	m_bUseGlobalColorSettings	= GetProfile().GetInt(m_sMiCoFrontend, _T("UseGlobalColorSetting"), TRUE);

	if (GetProfile().GetString(m_sMiCoFrontend,_T("Videoformat"),_T("Unknown")) == _T("PAL_CCIR"))
		m_nRadioVideoFormat = 0;
	if (GetProfile().GetString(m_sMiCoFrontend,_T("Videoformat"),_T("Unknown")) == _T("PAL_SQUARE"))
		m_nRadioVideoFormat = 1;
	if (GetProfile().GetString(m_sMiCoFrontend,_T("Videoformat"),_T("Unknown")) == _T("NTSC_CCIR"))
		m_nRadioVideoFormat = 2;
	if (GetProfile().GetString(m_sMiCoFrontend,_T("Videoformat"),_T("Unknown")) == _T("NTSC_SQUARE"))
		m_nRadioVideoFormat = 3;
}
					    
/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::SaveMiCoSettings()
{
	DWORD dwT = 0;

	// MiCo Address
	_stscanf(m_sAddressMiCo,szAddressFormat,&dwT);
	GetProfile().WriteInt(m_sMiCoSystem, szMiCoIOBase, m_bMiCoISA ? dwT : 0);
	
	// MiCo IRQ
	dwT = _ttoi(m_sIRQ);
// gf IRQ muss ausgetragen werden!!!
//	if (dwT != 0)
		GetProfile().WriteInt(m_sMiCoSystem, szIRQ, m_bMiCoISA ? dwT : 0);

	// 1.CoVi Addresse
	_stscanf(m_sAddressExtCard1,szAddressFormat,&dwT);
	GetProfile().WriteInt(m_sMiCoSystem, szExtCard1IOBase, m_bExtCard1 ? dwT : 0);

	// 2.CoVi Addresse
	_stscanf(m_sAddressExtCard2,szAddressFormat,&dwT);
	GetProfile().WriteInt(m_sMiCoSystem, szExtCard2IOBase, m_bExtCard2 ? dwT : 0);

	// 3.CoVi Addresse
	_stscanf(m_sAddressExtCard3,szAddressFormat,&dwT);
	GetProfile().WriteInt(m_sMiCoSystem, szExtCard3IOBase, m_bExtCard3 ? dwT : 0);

	// Kameratype und GlobalColorSetting sichern
	GetProfile().WriteInt(m_sMiCoGeneral, _T("CameraType"), m_bSyncCam);
	GetProfile().WriteInt(m_sMiCoFrontend, _T("UseGlobalColorSetting"), m_bUseGlobalColorSettings);
	
	if (m_bUse2Fields)
	{
		GetProfile().WriteInt(m_sMiCoGeneral, _T("StoreField"), 2);
		GetProfile().WriteInt(szSecurityServerFormat, m_sMiCoFps, 50);
	}
	else
	{
		GetProfile().WriteInt(m_sMiCoGeneral, _T("StoreField"), 0);
		GetProfile().WriteInt(szSecurityServerFormat, m_sMiCoFps, 25);
	}

	// Gewählter Bustype sichern
	if (m_bMiCoISA)
		GetProfile().WriteString(m_sMiCoSystem, szMiCoBusType, _T("ISA"));
	else if (m_bMiCoPCI)
		GetProfile().WriteString(m_sMiCoSystem, szMiCoBusType, _T("PCI"));
	else
		GetProfile().WriteString(m_sMiCoSystem, szMiCoBusType, _T("Unknown"));

	// Gewählte Videonorm sichern
	switch (m_nRadioVideoFormat)
	{
		case 0:
			GetProfile().WriteString(m_sMiCoFrontend,_T("Videoformat"),_T("PAL_CCIR"));
			GetProfile().WriteString(m_sMiCoBackend,_T("Videoformat"),_T("PAL_CCIR"));
			break;
		case 1:
			GetProfile().WriteString(m_sMiCoFrontend,_T("Videoformat"),_T("PAL_SQUARE"));
			GetProfile().WriteString(m_sMiCoBackend,_T("Videoformat"),_T("PAL_SQUARE"));
			break;
		case 2:
			GetProfile().WriteString(m_sMiCoFrontend,_T("Videoformat"),_T("NTSC_CCIR"));
			GetProfile().WriteString(m_sMiCoBackend,_T("Videoformat"),_T("NTSC_CCIR"));
			break;
		case 3:
			GetProfile().WriteString(m_sMiCoFrontend,_T("Videoformat"),_T("NTSC_SQUARE"));
			GetProfile().WriteString(m_sMiCoBackend,_T("Videoformat"),_T("NTSC_SQUARE"));
			break;
	}

	// MiCoUnit beenden
	if (m_bRestartMiCoUnit)
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
void CMiCoPage::EnableDlgControlsMiCo()
{
	// m_checkMiCo is always enabled
	if (!m_bMiCoISA)
	{
		m_sAddressMiCo.Empty();
		m_cbAddressMiCo.SetCurSel(-1);
		m_sIRQ.Empty();
		m_cbIRQ.SetCurSel(-1);
	}
	
	// Enable/Disable Dialogcontrols
	m_checkUseGlobalColorSettings.EnableWindow(m_bMiCo);
	m_checkSync.EnableWindow(m_bMiCo);
	m_checkUseGlobalColorSettings.EnableWindow(m_bMiCo);
	m_radioPalCCIR.EnableWindow(m_bMiCo);
	m_radioPalSquare.EnableWindow(m_bMiCo);
	m_radioNtscCCIR.EnableWindow(m_bMiCo);
	m_radioNtscSquare.EnableWindow(m_bMiCo);
	m_cbAddressMiCo.EnableWindow(m_bMiCoISA);
	m_cbIRQ.EnableWindow(m_bMiCoISA);
	m_checkUse2Fields.EnableWindow(m_bMiCo);									 

	// m_checkExtCard1 only enabled, if m_checkMiCo is checked
	m_checkExtCard1.EnableWindow(m_bMiCo);
	m_bExtCard1 = m_checkExtCard1.IsWindowEnabled() && m_bExtCard1;
	
	if (!m_bExtCard1)
	{
		m_sAddressExtCard1.Empty();
		m_cbAddressExtCard1.SetCurSel(-1);
	}
	
	m_cbAddressExtCard1.EnableWindow(m_bExtCard1);
	
	// m_checkExtCard2 only enabled, if m_checkExtCard1 is checked
	m_checkExtCard2.EnableWindow(m_bExtCard1);
	m_bExtCard2 = m_checkExtCard2.IsWindowEnabled() && m_bExtCard2;
	if (!m_bExtCard2)
	{
		m_sAddressExtCard2.Empty();
		m_cbAddressExtCard2.SetCurSel(-1);
	}
	
	m_cbAddressExtCard2.EnableWindow(m_bExtCard2);
	
	// m_checkExtCard3 only enabled, if m_checkExtCard2 is checked
	m_checkExtCard3.EnableWindow(m_bExtCard2);
	m_bExtCard3 = m_checkExtCard3.IsWindowEnabled() && m_bExtCard3;
	if (!m_bExtCard3)
	{
		m_sAddressExtCard3.Empty();
		m_cbAddressExtCard3.SetCurSel(-1);
	}
	
	m_cbAddressExtCard3.EnableWindow(m_bExtCard3);

	CString sBusType;
	// Die 1. MiCo darf nicht ausgetragen werden, solange eine 2. MiCo eingetragen ist
	if (m_nMiCoNr == MICO1)
	{
		sBusType = GetProfile().GetString(_T("MiCoUnitPCI\\Device2\\System"),szMiCoBusType,_T("Unknown"));
		if (sBusType != _T("Unknown"))
		{
			m_checkMiCoISA.EnableWindow(!m_bMiCoISA);
			m_checkMiCoPCI.EnableWindow(!m_bMiCoPCI);
			if (sBusType == _T("PCI"))
			{
				m_checkMiCoISA.EnableWindow(FALSE);
			}
		}
	}
	// Die 2. MiCo darf nicht ausgetragen werden, solange eine 3. MiCo eingetragen ist
	if (m_nMiCoNr == MICO2)
	{
		sBusType = GetProfile().GetString(_T("MiCoUnitPCI\\Device3\\System"),szMiCoBusType,_T("Unknown"));
		if (sBusType != _T("Unknown"))
		{
			m_checkMiCoISA.EnableWindow(!m_bMiCoISA);
			m_checkMiCoPCI.EnableWindow(!m_bMiCoPCI);
			if (sBusType == _T("PCI"))
			{
				m_checkMiCoISA.EnableWindow(FALSE);
			}
		}
	}
	// Die 3. MiCo darf nicht ausgetragen werden, solange eine 4. MiCo eingetragen ist
	if (m_nMiCoNr == MICO3)
	{
		sBusType = GetProfile().GetString(_T("MiCoUnitPCI\\Device4\\System"),szMiCoBusType,_T("Unknown"));
		if (sBusType != _T("Unknown"))
		{
			m_checkMiCoISA.EnableWindow(!m_bMiCoISA);
			m_checkMiCoPCI.EnableWindow(!m_bMiCoPCI);
			if (sBusType == _T("PCI"))
			{
				m_checkMiCoISA.EnableWindow(FALSE);
			}
		}
	}

	// Die 2. MiCoPCI darf nur eingetragen werden, wenn die 1. MiCoPCI ebenfalls eingetragen ist
	if (m_nMiCoNr == MICO2)
	{
		sBusType = GetProfile().GetString(_T("MiCoUnitPCI\\Device1\\System"),szMiCoBusType,_T("Unknown"));
		// Eine 2. MiCo darf nur eingetragen werden, wenn eine 1. MiCo vorhanden ist.
		if (sBusType == _T("Unknown"))
		{
			m_checkMiCoPCI.EnableWindow(FALSE);
			m_checkMiCoISA.EnableWindow(FALSE);
		}
		else if (sBusType == _T("ISA"))
		{
			m_checkMiCoPCI.EnableWindow(FALSE);
		}
	}
	// Die 3. MiCoPCI darf nur eingetragen werden, wenn die 2. MiCoPCI ebenfalls eingetragen ist
	if (m_nMiCoNr == MICO3)
	{
		sBusType = GetProfile().GetString(_T("MiCoUnitPCI\\Device2\\System"),szMiCoBusType,_T("Unknown"));
		// Eine 3. MiCo darf nur eingetragen werden, wenn eine 2. MiCo vorhanden ist.
		if (sBusType == _T("Unknown"))
		{
			m_checkMiCoPCI.EnableWindow(FALSE);
			m_checkMiCoISA.EnableWindow(FALSE);
		}
		else if (sBusType == _T("ISA"))
		{
			m_checkMiCoPCI.EnableWindow(FALSE);
		}
	}
	// Die 4. MiCoPCI darf nur eingetragen werden, wenn die 3. MiCoPCI ebenfalls eingetragen ist
	if (m_nMiCoNr == MICO4)
	{
		sBusType = GetProfile().GetString(_T("MiCoUnitPCI\\Device3\\System"),szMiCoBusType,_T("Unknown"));
		// Eine 4. MiCo darf nur eingetragen werden, wenn eine 3. MiCo vorhanden ist.
		if (sBusType == _T("Unknown"))
		{
			m_checkMiCoPCI.EnableWindow(FALSE);
			m_checkMiCoISA.EnableWindow(FALSE);
		}
		else if (sBusType == _T("ISA"))
		{
			m_checkMiCoPCI.EnableWindow(FALSE);
		}
	}

	// NTSC können wir derzeit noch nicht.
	m_radioNtscCCIR.EnableWindow(FALSE);
	m_radioNtscSquare.EnableWindow(FALSE);

	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::GetDlgMiCoData()
{
	CInputGroup* pInputGroup = NULL;
	COutputGroup* pOutputGroup = NULL;
	int iAnzahl = 0;

	// Anzahl der Inputs einlesen
	if (m_bMiCoISA)
		iAnzahl = NR_INPUTS_MICO_ISA;
	else if (m_bMiCoPCI)
		iAnzahl = NR_INPUTS_MICO_PCI;
	
	if (m_bExtCard1)
	{
		iAnzahl = iAnzahl + NR_INPUTS_COVI;
		if (m_bExtCard2)
		{
			iAnzahl = iAnzahl + NR_INPUTS_COVI;
			if (m_bExtCard3)
			{
				iAnzahl = iAnzahl + NR_INPUTS_COVI;
			}
		}
	}

	// Daten der Inputgruppe aktualisieren, wenn bereits vorhanden...
	pInputGroup = m_pInputList->GetGroupBySMName(m_sSM_MiCoInput);
	if ( pInputGroup)
	{
		pInputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else
	{
		pInputGroup = m_pInputList->AddInputGroup(m_sMelderDefaultName, iAnzahl, m_sSM_MiCoInput);
	}

	// Anzahl der Kamera Outputs einlesen
	if (m_bMiCoISA)
		iAnzahl = NR_OUTPUTS_CAMERA_MICO_ISA;
	else if (m_bMiCoPCI)
	{
		// Wenn eine CoVi vorhanden ist, hat die MiCoPCI nur 8 eigene Videoeingänge
		if (m_bExtCard1 || m_bExtCard2 || m_bExtCard3)
			iAnzahl = NR_OUTPUTS_CAMERA_MICO_ISA;
		else
			iAnzahl = NR_OUTPUTS_CAMERA_MICO_PCI;
	}
	if (m_bExtCard1)
	{
		iAnzahl = iAnzahl + NR_OUTPUTS_CAMERA_COVI;
		if (m_bExtCard2)
		{
			iAnzahl = iAnzahl + NR_OUTPUTS_CAMERA_COVI;
			if (m_bExtCard3)
			{
				iAnzahl = iAnzahl + NR_OUTPUTS_CAMERA_COVI;
			}
		}
	}

	// Daten der Kamera-Outputgruppe aktualisieren, wenn bereits vorhanden...
	pOutputGroup = m_pOutputList->GetGroupBySMName(m_sSM_MiCoOutputCameras);
	if ( pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(m_sCameraDefaultname, iAnzahl,m_sSM_MiCoOutputCameras);
	}

	// Anzahl der Relay Outputs einlesen
	if (m_bMiCoISA)
		iAnzahl = NR_OUTPUTS_RELAY_MICO_ISA;
	else if (m_bMiCoPCI)
		iAnzahl = NR_OUTPUTS_RELAY_MICO_PCI;
	
	
	if (m_bExtCard1)
	{
		iAnzahl = iAnzahl + NR_OUTPUTS_RELAY_COVI;
		if (m_bExtCard2)
		{
			iAnzahl = iAnzahl + NR_OUTPUTS_RELAY_COVI;
			if (m_bExtCard3)
			{
				iAnzahl = iAnzahl + NR_OUTPUTS_RELAY_COVI;
			}
		}
	}
	// Daten der Relay-Outputgruppe aktualisieren, wenn bereits vorhanden...
	pOutputGroup = m_pOutputList->GetGroupBySMName( m_sSM_MiCoOutputRelays);
	if ( pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(m_sRelayDefaultName, iAnzahl,m_sSM_MiCoOutputRelays);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnRadioNtscCcir() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnRadioNtscSquare() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnRadioPalCcir() 
{
	SetModified();
}
  
/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnRadioPalSquare() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::OnCheckUse2Fields() 
{
	SetModified();
	UpdateData(TRUE);
	if (m_bUse2Fields)
	{
		CString sMsg;
		sMsg.Format(IDP_2FIELDS_WARNING);
		AfxMessageBox(sMsg, MB_OK);					  
	}
}

/////////////////////////////////////////////////////////////////////////////
void CMiCoPage::SetModified(BOOL bModified, BOOL bServerInit)
{
	// Basisklasse aufrufen
	CSVPage::SetModified(bModified, bServerInit);
	m_bRestartMiCoUnit	= TRUE;
}
