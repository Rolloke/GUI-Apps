// CoCoPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "CoCoPage.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szMiCoSystemFormat[]	= _T("MiCoUnitPCI\\Device%d\\System");

/////////////////////////////////////////////////////////////////////////////
// CCoCoPage dialog

CCoCoPage::CCoCoPage(CSVView* pParent) : CSVPage(CCoCoPage::IDD)
{
	m_pParent			= pParent;
	m_pInputList		= pParent->GetDocument()->GetInputs();
	m_pOutputList		= pParent->GetDocument()->GetOutputs();
	m_bRestartCoCoUnit	= FALSE;
	m_bRestartWindows	= FALSE;

	//{{AFX_DATA_INIT(CCoCoPage)
	m_bCoCo				= FALSE;
	m_bExtCard1			= FALSE;
	m_bExtCard2			= FALSE;
	m_bExtCard3			= FALSE;
	m_sIRQCoCo			= _T("");
	m_sAddressCoCo		= _T("");
	m_sAddressExtCard1	= _T("");
	m_sAddressExtCard2	= _T("");
	m_sAddressExtCard3	= _T("");
	m_bCoCoSoft = FALSE;
	//}}AFX_DATA_INIT

	// Einige Referenzen für den einfacheren Zugriff.
	m_pcbAddressExtCard[0]	= &m_cbAddressCoCo;
	m_pcbAddressExtCard[1]	= &m_cbAddressExtCard1;
	m_pcbAddressExtCard[2]	= &m_cbAddressExtCard2;
	m_pcbAddressExtCard[3]	= &m_cbAddressExtCard3;
	m_psAddressExtCard[0]	= &m_sAddressCoCo;
	m_psAddressExtCard[1]	= &m_sAddressExtCard1;
	m_psAddressExtCard[2]	= &m_sAddressExtCard2;
	m_psAddressExtCard[3]	= &m_sAddressExtCard3;
	m_pbExtCard[0]			= &m_bCoCo;
	m_pbExtCard[1]			= &m_bExtCard1;
	m_pbExtCard[2]			= &m_bExtCard2;
	m_pbExtCard[3]			= &m_bExtCard3;
	
	Create(IDD,(CWnd*)pParent);
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCoCoPage)
	DDX_Control(pDX, IDC_IRQ_COCO,			m_cbIRQCoCo);
	DDX_Control(pDX, IDC_ADDRESS_COCO3,		m_cbAddressExtCard3);
	DDX_Control(pDX, IDC_ADDRESS_COCO2,		m_cbAddressExtCard2);
	DDX_Control(pDX, IDC_ADDRESS_COCO1,		m_cbAddressExtCard1);
	DDX_Control(pDX, IDC_ADDRESS_COCO,		m_cbAddressCoCo);
	DDX_Control(pDX, IDC_CHECK_COCO3,		m_checkExtCard3);
	DDX_Control(pDX, IDC_CHECK_COCO2,		m_checkExtCard2);
	DDX_Control(pDX, IDC_CHECK_COCO1,		m_checkExtCard1);
	DDX_Control(pDX, IDC_CHECK_COCO,		m_checkCoCo);
	DDX_Check(pDX, IDC_CHECK_COCO,			m_bCoCo);
	DDX_Check(pDX, IDC_CHECK_COCO1,			m_bExtCard1);
	DDX_Check(pDX, IDC_CHECK_COCO2,			m_bExtCard2);
	DDX_Check(pDX, IDC_CHECK_COCO3,			m_bExtCard3);
	DDX_CBString(pDX, IDC_IRQ_COCO,			m_sIRQCoCo);
	DDX_CBString(pDX, IDC_ADDRESS_COCO,		m_sAddressCoCo);
	DDX_CBString(pDX, IDC_ADDRESS_COCO1,	m_sAddressExtCard1);
	DDX_CBString(pDX, IDC_ADDRESS_COCO2,	m_sAddressExtCard2);
	DDX_CBString(pDX, IDC_ADDRESS_COCO3,	m_sAddressExtCard3);
	DDX_Check(pDX, IDC_CHECK_COCO_SOFT, m_bCoCoSoft);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CCoCoPage, CSVPage)
	//{{AFX_MSG_MAP(CCoCoPage)
	ON_BN_CLICKED(IDC_CHECK_COCO,		OnCheckCoCo)
	ON_BN_CLICKED(IDC_CHECK_COCO1,		OnCheckExtCard1)
	ON_BN_CLICKED(IDC_CHECK_COCO2,		OnCheckExtCard2)
	ON_BN_CLICKED(IDC_CHECK_COCO3,		OnCheckExtCard3)
	ON_CBN_SELCHANGE(IDC_ADDRESS_COCO,	OnSelchangeAddressCoCo)
	ON_CBN_SELCHANGE(IDC_ADDRESS_COCO1, OnSelchangeAddressExtCard1)
	ON_CBN_SELCHANGE(IDC_ADDRESS_COCO2, OnSelchangeAddressExtCard2)
	ON_CBN_SELCHANGE(IDC_ADDRESS_COCO3, OnSelchangeAddressExtCard3)
	ON_CBN_SELCHANGE(IDC_IRQ_COCO,		OnSelchangeIrqCoCo)
	ON_BN_CLICKED(IDC_CHECK_COCO_SOFT, OnCheckCocoSoft)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CCoCoPage::IsDataValid()
{
	CString sMiCoCard[3][4];
	CString sCoCoCard[4];

	sMiCoCard[1][0]		= _T("1.MiCo");
	sMiCoCard[1][1]		= _T("1.MiCo/1.VideoPlu8");
	sMiCoCard[1][2]		= _T("1.MiCo/2.VideoPlu8");
	sMiCoCard[1][3]		= _T("1.MiCo/3.VideoPlu8");

	sMiCoCard[2][0]		= _T("2.MiCo");
	sMiCoCard[2][1]		= _T("2.MiCo/1.VideoPlu8");
	sMiCoCard[2][2]		= _T("2.MiCo/2.VideoPlu8");
	sMiCoCard[2][3]		= _T("2.MiCo/3.VideoPlu8");

	sCoCoCard[0]		= _T("CoCo");
	sCoCoCard[1]		= _T("CoCo/1.VideoPlu8");
	sCoCoCard[2]		= _T("CoCo/2.VideoPlu8");
	sCoCoCard[3]		= _T("CoCo/3.VideoPlu8");

	CString sAddr		= _T("");
	CString sIRQ		= _T("");
	CString sKey		= _T("");
	CString sMsg		= _T("");
	CString sFirstCard	= _T("");
	CString sSecondCard	= _T("");
	CString sSection	= _T("");

	UpdateData();

	for (int nI = 0; nI <= 3; nI++)
	{
		// Sucht nach doppelt benutzten CoVi-Basisadressen innerhalb einer CoCos
		for (int nJ = nI+1; nJ<= 3; nJ++)
		{
			// Sind CoCo und CoCo-CoVi1-Adressen identisch?
			if (*m_pbExtCard[nI] && *m_pbExtCard[nJ])
			{
				if (*m_psAddressExtCard[nI] == *m_psAddressExtCard[nJ])
				{
					sMsg.Format(IDP_HARDWARE_ADDRESS_DOUBLE, sCoCoCard[nI], sCoCoCard[nJ], *m_psAddressExtCard[nI]);
					AfxMessageBox(sMsg, MB_OK);					  
					return FALSE;
				}
			}
		}

		// Überprüfe ob ein CoCo oder ein CoCo/CoVi die gleiche Basisadresse
		// wie ein MiCo/CoVi besitzt. Und ob die IRQs von CoCo und MiCo 
		// unterschiedlich sind
		if (*m_pbExtCard[nI])
		{
			for (int nMiCo = 1; nMiCo <= 2; nMiCo++)
			{
				for (int nJ = 1; nJ <= 3; nJ++)
				{
					sKey.Format(szMiCoSystemFormat, nMiCo);
					sSection.Format(_T("ExtCard%dIOBase"), nJ);
					sAddr.Format(_T("0x%x"), GetProfile().GetInt(sKey, sSection, 0));
					if (*m_psAddressExtCard[nI] == sAddr)
					{
						sMsg.Format(IDP_HARDWARE_ADDRESS_DOUBLE, sCoCoCard[nI], sMiCoCard[nMiCo][nJ], sAddr);
						AfxMessageBox(sMsg, MB_OK);					  
						return FALSE;
					}
				}
				sIRQ.Format(_T("%d"), GetProfile().GetInt(sKey, _T("IRQ"), 0));
				if (m_sIRQCoCo == sIRQ)
				{
					sMsg.Format(IDP_HARDWARE_IRQ_DOUBLE, sCoCoCard[0], sMiCoCard[nMiCo][0], sIRQ);
					AfxMessageBox(sMsg, MB_OK);
					return FALSE;
				}
			}
		}

	
	}

	return TRUE;

}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::SaveChanges()
{
	WK_TRACE_USER(_T("CoCo-Einstellungen wurden geändert\n"));
	UpdateData();

	// CoCo aktualisieren
	if (m_bCoCo) 
	{
		GetDlgCoCoData();
	}
	else 
	{
		// Gruppen loeschen
		m_pInputList->DeleteGroup(SM_COCO_INPUT0);
		m_pOutputList->DeleteGroup(SM_COCO_OUTPUT_CAM0);
		m_pOutputList->DeleteGroup(SM_COCO_OUTPUT_RELAY0);
	}

	// Einstellungen des CoCos sichern
	SaveCoCoSettings();
	
	GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_COCOUNIT,m_bCoCo ? _T("CoCoUnit.exe") : _T(""));

	SaveInputsAndOutputsAfterCheckingActivations();
	SetModified(FALSE,FALSE);
	
	m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, (1<<IMAGE_INPUT)|(1<<IMAGE_RELAY)|(1<<IMAGE_CAMERA));
//	m_pParent->OnInputChanged();
//	m_pParent->OnCameraChanged();
//	m_pParent->OnRelaisChanged();
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::CancelChanges()
{
	LoadCoCoSettings();
	
	UpdateData(FALSE);

	if (IsDataValid()) 
	{
		SetModified(FALSE,FALSE);
	}
	else 
	{
		SetModified();
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCoCoPage message handlers
void CCoCoPage::LoadCoCoSettings()
{
	TCHAR szBuffer[256] = {0};
	int iIndex = -1;

	if (m_bCoCo)
	{
		GetPrivateProfileString(_T("CoCoISA0"), _T("IOBase"), _T(""), szBuffer, sizeof(szBuffer), theApp.GetSytemIni());
		m_sAddressCoCo = _T("0x");
		m_sAddressCoCo += szBuffer;
		if (!m_sAddressCoCo.IsEmpty()) 
		{
			iIndex = m_cbAddressCoCo.FindStringExact(-1, m_sAddressCoCo);
			m_cbAddressCoCo.SetCurSel(iIndex);
		}
	}
	else 
	{
		m_cbAddressCoCo.SetCurSel(-1);
	}

	if (m_bExtCard1)
	{
		GetPrivateProfileString(_T("CoCoISA0"), _T("ExtCard1"), _T(""), szBuffer, sizeof(szBuffer), theApp.GetSytemIni());
		m_sAddressExtCard1 = _T("0x");
		m_sAddressExtCard1 += szBuffer;
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


	if (m_bExtCard2)
	{
		GetPrivateProfileString(_T("CoCoISA0"), _T("ExtCard2"), _T(""), szBuffer, sizeof(szBuffer), theApp.GetSytemIni());
		m_sAddressExtCard2 = _T("0x");
		m_sAddressExtCard2 += szBuffer;
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

	if (m_bExtCard3)
	{
		GetPrivateProfileString(_T("CoCoISA0"), _T("ExtCard3"), _T(""), szBuffer, sizeof(szBuffer), theApp.GetSytemIni());
		m_sAddressExtCard3 = _T("0x");
		m_sAddressExtCard3 += szBuffer;
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

	if (m_bCoCo)
	{
		GetPrivateProfileString(_T("CoCoISA0"), _T("AVPIRQ"), _T(""), szBuffer, sizeof(szBuffer), theApp.GetSytemIni());
		m_sIRQCoCo = szBuffer;
		m_sIRQCoCo.SpanIncluding(_T("0123456789"));
		if (!m_sIRQCoCo.IsEmpty()) 
		{
			iIndex = m_cbIRQCoCo.FindStringExact(-1, m_sIRQCoCo);
			m_cbIRQCoCo.SetCurSel(iIndex);
		}
	}
	else 
	{
		m_cbIRQCoCo.SetCurSel(-1);
	}

	m_bCoCoSoft = GetProfile().GetInt(REG_KEY_CLIENT_SETTINGS, _T("CoCoSoft"),TRUE); 
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::SaveCoCoSettings()
{																		 
	GetProfile().WriteInt(REG_KEY_CLIENT_SETTINGS,_T("CoCoSoft"),m_bCoCoSoft); 

	WritePrivateProfileString(_T("CoCoISA0"), _T("IOBase"),   m_bCoCo     ? (LPCTSTR)m_sAddressCoCo.Mid(2)	   : NULL, theApp.GetSytemIni());
	WritePrivateProfileString(_T("CoCoISA0"), _T("ExtCard1"), m_bExtCard1 ? (LPCTSTR)m_sAddressExtCard1.Mid(2) : NULL, theApp.GetSytemIni());
	WritePrivateProfileString(_T("CoCoISA0"), _T("ExtCard2"), m_bExtCard2 ? (LPCTSTR)m_sAddressExtCard2.Mid(2) : NULL, theApp.GetSytemIni());
	WritePrivateProfileString(_T("CoCoISA0"), _T("ExtCard3"), m_bExtCard3 ? (LPCTSTR)m_sAddressExtCard3.Mid(2) : NULL, theApp.GetSytemIni());

	// flush the ini file, because it's cached by Windows 95
	WritePrivateProfileString(NULL,NULL,NULL,theApp.GetSytemIni());

	WritePrivateProfileString(_T("CoCoISA0"), _T("AVPIRQ"), m_bCoCo ? (LPCTSTR)m_sIRQCoCo : NULL, theApp.GetSytemIni());
	// flush the ini file, because it's cached by Windows 95
	WritePrivateProfileString(NULL,NULL,NULL,theApp.GetSytemIni());


	if (m_bRestartWindows)
	{
		CString sMsg;	  
		sMsg.Format(IDP_RESTART_WINDOWS);
		AfxMessageBox(sMsg, MB_OK);	 				  
		m_bRestartWindows	= FALSE;
	}
	else
	{
		if (m_bRestartCoCoUnit)
		{
			CString sMsg;	  
			sMsg.Format(IDP_RESTART_COCO);
			AfxMessageBox(sMsg, MB_OK);	 				  
			m_bRestartCoCoUnit	= FALSE;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::GetDlgCoCoData()
{
	CInputGroup* pInputGroup = NULL;
	COutputGroup* pOutputGroup = NULL;
	int iAnzahl = 0;

	// Anzahl der Inputs einlesen
	iAnzahl = NR_INPUTS_COCO;
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
	pInputGroup = m_pInputList->GetGroupBySMName(SM_COCO_INPUT0);
	if (pInputGroup) 
	{
		pInputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pInputGroup = m_pInputList->AddInputGroup(NULL, iAnzahl, SM_COCO_INPUT0);
	}

	// Anzahl der Kamera Outputs einlesen
	iAnzahl = NR_OUTPUTS_CAMERA_COCO;
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
	pOutputGroup = m_pOutputList->GetGroupBySMName(SM_COCO_OUTPUT_CAM0);
	if (pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(NULL, iAnzahl,SM_COCO_OUTPUT_CAM0);
	}

	// Anzahl der Relay Outputs einlesen
	iAnzahl = NR_OUTPUTS_RELAY_COCO;
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
	pOutputGroup = m_pOutputList->GetGroupBySMName(SM_COCO_OUTPUT_RELAY0);
	if (pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(NULL, iAnzahl,SM_COCO_OUTPUT_RELAY0);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::OnCheckCoCo() 
{
	UpdateData(TRUE);
	if (m_bCoCo)
	{
		m_cbAddressCoCo.SetCurSel(m_cbAddressExtCard1.FindStringExact(-1, _T("0x220")));
		m_cbIRQCoCo.SetCurSel(m_cbIRQCoCo.FindStringExact(-1, _T("11")));
		SetModified();
		m_bRestartWindows = TRUE;
	}
	else
	{
		if (AfxMessageBox(IDP_HARDWARE_GROUP_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO)
		{
			m_bCoCo = TRUE;
		}
		else
		{
			SetModified();
			m_bRestartWindows = TRUE;
		}
	}
	
	EnableDlgControls();
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::OnCheckExtCard1() 
{
	UpdateData(TRUE);
	if (m_bExtCard1)
	{
		m_cbAddressExtCard1.SetCurSel(m_cbAddressExtCard1.FindStringExact(-1, _T("0x240")));
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
	
	EnableDlgControls();
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::OnCheckExtCard2() 
{
	UpdateData(TRUE);
	if (m_bExtCard2)
	{
		m_cbAddressExtCard2.SetCurSel(m_cbAddressExtCard2.FindStringExact(-1, _T("0x320")));
		SetModified();
	}
	else
	{
		if (AfxMessageBox(IDP_HARDWARE_PARTIAL_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO)
		{
			m_bExtCard2 = TRUE;
		}
		else
		{
			SetModified();
		}
	}
	
	EnableDlgControls();
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::OnCheckExtCard3() 
{
	UpdateData(TRUE);
	if (m_bExtCard3)
	{
		m_cbAddressExtCard3.SetCurSel(m_cbAddressExtCard3.FindStringExact(-1, _T("0x340")));
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
	
	EnableDlgControls();
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::OnSelchangeAddressCoCo() 
{
	SetModified();
	m_bRestartWindows = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::OnSelchangeAddressExtCard1() 
{
	SetModified();
}

////////////////////////////////////////////////////////////////////////////
void CCoCoPage::OnSelchangeAddressExtCard2() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::OnSelchangeAddressExtCard3() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::OnSelchangeIrqCoCo() 
{
	SetModified();
	m_bRestartWindows = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::EnableDlgControls()
{
	// m_checkCoCo is always enabled
	if (!m_bCoCo)
	{
		m_sAddressCoCo.Empty();
		m_cbAddressCoCo.SetCurSel(-1);
		m_sIRQCoCo.Empty();
		m_cbIRQCoCo.SetCurSel(-1);
	}
	
	m_cbAddressCoCo.EnableWindow(m_bCoCo);
	m_cbIRQCoCo.EnableWindow(m_bCoCo);
	
	// m_checkExtCard1 only enabled, if m_checkCoCo is checked
	m_checkExtCard1.EnableWindow(m_bCoCo);
	m_bExtCard1 = m_checkExtCard1.IsWindowEnabled() && m_bExtCard1;
	
	if (!m_bExtCard1)
	{
		m_sAddressExtCard1.Empty();
		m_cbAddressExtCard1.SetCurSel(-1);
	}
	
	m_cbAddressExtCard1.EnableWindow(m_bExtCard1);

	// m_checkExtCard2 only enabled, if m_checkExtCard1 is checked
	m_checkExtCard2.EnableWindow( m_bExtCard1 );
	m_bExtCard2 = m_checkExtCard2.IsWindowEnabled() && m_bExtCard2;
	
	if (!m_bExtCard2)
	{
		m_sAddressExtCard2.Empty();
		m_cbAddressExtCard2.SetCurSel(-1);
	}
	
	m_cbAddressExtCard2.EnableWindow( m_bExtCard2 );
	// m_checkExtCard3 only enabled, if m_checkExtCard2 is checked
	m_checkExtCard3.EnableWindow( m_bExtCard2 );
	m_bExtCard3 = m_checkExtCard3.IsWindowEnabled() && m_bExtCard3;
	
	if (!m_bExtCard3)
	{
		m_sAddressExtCard3.Empty();
		m_cbAddressExtCard3.SetCurSel(-1);
	}
	m_cbAddressExtCard3.EnableWindow( m_bExtCard3 );

	UpdateData(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
BOOL CCoCoPage::OnInitDialog() 
{
	TCHAR szBuffer[256] = {0};

	CSVPage::OnInitDialog();
	
	// Anhand der Basisadressen ermitteln, welche karten im System eingetragen sind.
	GetPrivateProfileString(_T("CoCoISA0"), _T("IOBase"), _T(""), szBuffer, sizeof(szBuffer), theApp.GetSytemIni());
	m_sAddressCoCo		= szBuffer;
	m_bCoCo				= !m_sAddressCoCo.IsEmpty();

	GetPrivateProfileString(_T("CoCoISA0"), _T("ExtCard1"), _T(""), szBuffer, sizeof(szBuffer), theApp.GetSytemIni());
	m_sAddressExtCard1	= szBuffer;
	m_bExtCard1			= !m_sAddressExtCard1.IsEmpty();

	GetPrivateProfileString(_T("CoCoISA0"), _T("ExtCard2"), _T(""), szBuffer, sizeof(szBuffer), theApp.GetSytemIni());
	m_sAddressExtCard2	= szBuffer;
	m_bExtCard2			= !m_sAddressExtCard2.IsEmpty();

	GetPrivateProfileString(_T("CoCoISA0"), _T("ExtCard3"), _T(""), szBuffer, sizeof(szBuffer), theApp.GetSytemIni());
	m_sAddressExtCard3	= szBuffer;
	m_bExtCard3			= !m_sAddressExtCard3.IsEmpty();
	
	// CoCo Einstellungen aus der Ini-Datei laden.
	LoadCoCoSettings();

	EnableDlgControls();

	if (!IsLocal())
	{
		DisableAll();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::SetModified(BOOL bModified, BOOL bServerInit)
{
	// Basisklasse aufrufen
	CSVPage::SetModified(bModified, bServerInit);
	m_bRestartCoCoUnit	= TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CCoCoPage::OnCheckCocoSoft() 
{
	CSVPage::SetModified(TRUE, FALSE);
}
