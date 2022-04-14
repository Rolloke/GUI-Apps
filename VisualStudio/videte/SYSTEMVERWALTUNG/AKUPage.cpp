// AKUPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "AKUPage.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szAkuSystem[]		= _T("AkuUnit\\System");
static TCHAR BASED_CODE szAddressFormat[]	= _T("0x%03lx");

/////////////////////////////////////////////////////////////////////////////
// CAKUPage dialog
CAKUPage::CAKUPage(CSVView* pParent): CSVPage(CAKUPage::IDD)
{
	m_pParent = pParent;
	m_bRestartAkuUnit	= FALSE;

	//{{AFX_DATA_INIT(CAKUPage)
	m_sAddressAku1	= _T("");
	m_sAddressAku2	= _T("");
	m_sAddressAku3	= _T("");
	m_sAddressAku4	= _T("");
	m_bAku1			= FALSE;
	m_bAku2			= FALSE;
	m_bAku3			= FALSE;
	m_bAku4			= FALSE;
	//}}AFX_DATA_INIT

	// Einige Referenzen zur Codevereinfachung
	m_pcbAddressAku[0]	= &m_cbAddressAku1;
	m_pcbAddressAku[1]	= &m_cbAddressAku2;
	m_pcbAddressAku[2]	= &m_cbAddressAku3;
	m_pcbAddressAku[3]	= &m_cbAddressAku4;
	m_psAddressAku[0]	= &m_sAddressAku1;
	m_psAddressAku[1]	= &m_sAddressAku2;
	m_psAddressAku[2]	= &m_sAddressAku3;
	m_psAddressAku[3]	= &m_sAddressAku4;
	m_pbAku[0]			= &m_bAku1;
	m_pbAku[1]			= &m_bAku2;
	m_pbAku[2]			= &m_bAku3;
	m_pbAku[3]			= &m_bAku4;
	
	m_pInputList	= pParent->GetDocument()->GetInputs();
	m_pOutputList	= pParent->GetDocument()->GetOutputs();
	Create(IDD,m_pParent);
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAKUPage)
	DDX_Control(pDX, IDC_CHECK_AKU4,	m_checkAku4);
	DDX_Control(pDX, IDC_CHECK_AKU3,	m_checkAku3);
	DDX_Control(pDX, IDC_CHECK_AKU2,	m_checkAku2);
	DDX_Control(pDX, IDC_CHECK_AKU1,	m_checkAku1);
	
	DDX_Control(pDX, IDC_ADDRESS_AKU4,	m_cbAddressAku4);
	DDX_Control(pDX, IDC_ADDRESS_AKU3,	m_cbAddressAku3);
	DDX_Control(pDX, IDC_ADDRESS_AKU2,	m_cbAddressAku2);
	DDX_Control(pDX, IDC_ADDRESS_AKU1,	m_cbAddressAku1);

	DDX_CBString(pDX, IDC_ADDRESS_AKU1,	m_sAddressAku1);
	DDX_CBString(pDX, IDC_ADDRESS_AKU2, m_sAddressAku2);
	DDX_CBString(pDX, IDC_ADDRESS_AKU3, m_sAddressAku3);
	DDX_CBString(pDX, IDC_ADDRESS_AKU4, m_sAddressAku4);
	
	DDX_Check(pDX, IDC_CHECK_AKU1,		m_bAku1);
	DDX_Check(pDX, IDC_CHECK_AKU2,		m_bAku2);
	DDX_Check(pDX, IDC_CHECK_AKU3,		m_bAku3);
	DDX_Check(pDX, IDC_CHECK_AKU4,		m_bAku4);
	//}}AFX_DATA_MAP
}

/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CAKUPage, CSVPage)
	//{{AFX_MSG_MAP(CAKUPage)
	ON_BN_CLICKED(IDC_CHECK_AKU1,		OnCheckAku1)
	ON_BN_CLICKED(IDC_CHECK_AKU2,		OnCheckAku2)
	ON_BN_CLICKED(IDC_CHECK_AKU3,		OnCheckAku3)
	ON_BN_CLICKED(IDC_CHECK_AKU4,		OnCheckAku4)

	ON_CBN_SELCHANGE(IDC_ADDRESS_AKU1,	OnSelchangeAddressAku1)
	ON_CBN_SELCHANGE(IDC_ADDRESS_AKU2,	OnSelchangeAddressAku2)
	ON_CBN_SELCHANGE(IDC_ADDRESS_AKU3,	OnSelchangeAddressAku3)
	ON_CBN_SELCHANGE(IDC_ADDRESS_AKU4,	OnSelchangeAddressAku4)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAKUPage message handlers
BOOL CAKUPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	// Anhand der IO-Adressen wird bestimmt welche Akus derzeit im System eingetragen sind.
	m_bAku1 = (GetProfile().GetInt(szAkuSystem, _T("AkuIOBase0"), 0) != 0);
	m_bAku2 = (GetProfile().GetInt(szAkuSystem, _T("AkuIOBase1"), 0) != 0);
	m_bAku3 = (GetProfile().GetInt(szAkuSystem, _T("AkuIOBase2"), 0) != 0);
	m_bAku4 = (GetProfile().GetInt(szAkuSystem, _T("AkuIOBase3"), 0) != 0);

	LoadAkuAddresses();
	UpdateData(FALSE);
	EnableDlgControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::EnableDlgControls()
{
	if (!m_bAku1) 
	{
		m_sAddressAku1.Empty();
		m_cbAddressAku1.SetCurSel(-1);
	}

	m_cbAddressAku1.EnableWindow(m_bAku1);

	m_checkAku2.EnableWindow(m_bAku1);
	m_bAku2 = m_checkAku2.IsWindowEnabled() && m_bAku2;

	if (!m_bAku2)
	{
		m_sAddressAku2.Empty();
		m_cbAddressAku2.SetCurSel(-1);
	}
	
	m_cbAddressAku2.EnableWindow(m_bAku2);
	
	m_checkAku3.EnableWindow( m_bAku2 );
	m_bAku3 = m_checkAku3.IsWindowEnabled() && m_bAku3;
	
	if (!m_bAku3)
	{
		m_sAddressAku3.Empty();
		m_cbAddressAku3.SetCurSel(-1);
	}
	
	m_cbAddressAku3.EnableWindow( m_bAku3 );
	m_checkAku4.EnableWindow( m_bAku3 );
	
	m_bAku4 = m_checkAku4.IsWindowEnabled() && m_bAku4;
	
	if (!m_bAku4)
	{
		m_sAddressAku4.Empty();
		m_cbAddressAku4.SetCurSel(-1);
	}
	
	m_cbAddressAku4.EnableWindow( m_bAku4 );

	UpdateData(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
BOOL CAKUPage::IsDataValid()
{
	CString	sMsg		= _T("");
	CString	sFirstCard	= _T("");
	CString	sSecondCard	= _T("");
	
	UpdateData();

	//******************************************************************************
	// Sucht nach doppelt benutzten Aku-Basisadressen
	//******************************************************************************
	for (int nI = 0; nI <= 3; nI++)
	{
		for (int nJ = 0; nJ<= 3; nJ++)
		{
			if (nI != nJ)
			{
				// Sind CoCo und CoCo-CoVi1-Adressen identisch?
				if (*m_pbAku[nI] && *m_pbAku[nJ])
				{
					if (*m_psAddressAku[nI] == *m_psAddressAku[nJ])
					{
						sFirstCard.Format(_T("%d.RelaisPlus8"), nI+1);
						sSecondCard.Format(_T("%d.RelaisPlus8"), nJ+1);

						sMsg.Format(IDP_HARDWARE_ADDRESS_DOUBLE, sFirstCard, sSecondCard, *m_psAddressAku[nI]);
						AfxMessageBox(sMsg, MB_OK);					  
						return FALSE;
					}
				}
			}
		}
	}

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::CancelChanges()
{
	LoadAkuAddresses();

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
void CAKUPage::SaveChanges()
{
	WK_TRACE_USER(_T("AKU-Einstellungen wurden geändert\n"));
	UpdateData(TRUE);

	// Aku1 aktualisieren
	if (m_bAku1) 
	{
		GetDlgAkuData();
	}
	else 
	{
		// Gruppen loeschen
		m_pInputList->DeleteGroup(SM_AKU_INPUT);
		m_pOutputList->DeleteGroup(SM_AKU_OUTPUT);
	}

	SaveAkuAddresses();

	GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_AKUUNIT,m_bAku1 ? _T("AkuUnit.exe") : _T(""));

	SaveInputsAndOutputsAfterCheckingActivations();

	SetModified(FALSE,FALSE);
	m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, (1<<IMAGE_INPUT)|(1<<IMAGE_RELAY));
//	m_pParent->OnInputChanged();
//	m_pParent->OnRelaisChanged();
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::LoadAkuAddresses()
{
	CString sKey				= _T("");
	CString sAkuIoBaseXFormat	= _T("AkuIOBase%d");
	int iIndex = -1;
	DWORD dwT;

	for (int nI = 0; nI <= 3; nI++)
	{
		if (*m_pbAku[nI])
		{
			// IO-Adressen einlesen
			sKey.Format(sAkuIoBaseXFormat, nI);
			dwT = GetProfile().GetInt(szAkuSystem, sKey,0);
			if (dwT)
			{
				m_psAddressAku[nI]->Format(szAddressFormat, dwT);
			}
			else
			{
				m_psAddressAku[nI]->Empty();
			}

			if (!m_psAddressAku[nI]->IsEmpty()) 
			{
				iIndex = m_pcbAddressAku[nI]->FindStringExact(-1, *m_psAddressAku[nI]);
				m_pcbAddressAku[nI]->SetCurSel(iIndex);
			}
		}
		else 
		{
			m_pcbAddressAku[nI]->SetCurSel(-1);
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::SaveAkuAddresses()
{
	DWORD	dwT;
	
	// Aku Addressen sichern
	_stscanf(m_sAddressAku1,szAddressFormat,&dwT);
	GetProfile().WriteInt(szAkuSystem, _T("AkuIOBase0"), m_bAku1 ? dwT : 0);

	_stscanf(m_sAddressAku2,szAddressFormat,&dwT);
	GetProfile().WriteInt(szAkuSystem, _T("AkuIOBase1"), m_bAku2 ? dwT : 0);

	_stscanf(m_sAddressAku3,szAddressFormat,&dwT);
	GetProfile().WriteInt(szAkuSystem, _T("AkuIOBase2"), m_bAku3 ? dwT : 0);

	_stscanf(m_sAddressAku4,szAddressFormat,&dwT);
	GetProfile().WriteInt(szAkuSystem, _T("AkuIOBase3"), m_bAku4 ? dwT : 0);

	if (m_bRestartAkuUnit)
	{
		CString sMsg;	  
		sMsg.Format(IDP_RESTART_AKU);
		AfxMessageBox(sMsg, MB_OK);	 				  
		m_bRestartAkuUnit	= FALSE;
	}
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::GetDlgAkuData()
{
	CInputGroup* pInputGroup = NULL;
	COutputGroup* pOutputGroup = NULL;
	int iAnzahl = 0;

	// Anzahl der Inputs einlesen
	iAnzahl = NR_INPUTS_AKU;
	if (m_bAku2) 
	{
		iAnzahl = iAnzahl + NR_INPUTS_AKU;
		if (m_bAku3) 
		{
			iAnzahl = iAnzahl + NR_INPUTS_AKU;
			if (m_bAku4) 
			{
				iAnzahl = iAnzahl + NR_INPUTS_AKU;
			}
		}
	}

	// Daten der Inputgruppe aktualisieren, wenn bereits vorhanden...
	pInputGroup = m_pInputList->GetGroupBySMName(SM_AKU_INPUT);
	if (pInputGroup) 
	{
		pInputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pInputGroup = m_pInputList->AddInputGroup(NULL, iAnzahl, SM_AKU_INPUT);
	}

	// Anzahl der Relay Outputs einlesen
	iAnzahl = NR_OUTPUTS_AKU;
	if (m_bAku2) 
	{
		iAnzahl = iAnzahl + NR_OUTPUTS_AKU;
		if (m_bAku3) 
		{
			iAnzahl = iAnzahl + NR_OUTPUTS_AKU;
			if (m_bAku4) 
			{
				iAnzahl = iAnzahl + NR_OUTPUTS_AKU;
			}
		}
	}

	// Daten der Relay-Outputgruppe aktualisieren, wenn bereits vorhanden...
	pOutputGroup = m_pOutputList->GetGroupBySMName(SM_AKU_OUTPUT);
	if (pOutputGroup) 
	{
		pOutputGroup->SetSize((WORD)iAnzahl);
	}
	// ...sonst neue Gruppe anlegen
	else 
	{
		pOutputGroup = m_pOutputList->AddOutputGroup(NULL, iAnzahl,SM_AKU_OUTPUT);
//		pOutputGroup = m_pOutputList->AddOutputGroup(_T("Relais Plus 8"), iAnzahl,SM_AKU_OUTPUT);
	}
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::OnCheckAku1() 
{
	UpdateData(TRUE);
	if (m_bAku1) 
	{
		m_cbAddressAku1.SetCurSel(m_cbAddressAku1.FindStringExact(-1, _T("0x260")));
		SetModified();
	}
	else 
	{
		if (AfxMessageBox(IDP_HARDWARE_PARTIAL_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO) 
		{
			m_bAku1 = TRUE;
		}
		else 
		{
			SetModified();
		}
	}
	EnableDlgControls();
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::OnCheckAku2() 
{
	UpdateData(TRUE);
	if (m_bAku2) 
	{
		m_cbAddressAku2.SetCurSel(m_cbAddressAku2.FindStringExact(-1, _T("0x2A0")));
		SetModified();
	}
	else 
	{
		if (AfxMessageBox(IDP_HARDWARE_PARTIAL_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO) 
		{
			m_bAku2 = TRUE;
		}
		else 
		{
			SetModified();
		}
	}
	EnableDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
void CAKUPage::OnCheckAku3() 
{
	UpdateData(TRUE);
	if (m_bAku3) 
	{
		m_cbAddressAku3.SetCurSel(m_cbAddressAku3.FindStringExact(-1, _T("0x2C0")));
		SetModified();
	}
	else 
	{
		if (AfxMessageBox(IDP_HARDWARE_PARTIAL_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO) 
		{
			m_bAku3 = TRUE;
		}
		else 
		{
			SetModified();
		}
	}
	EnableDlgControls();
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::OnCheckAku4() 
{
	UpdateData(TRUE);
	if (m_bAku4) 
	{
		m_cbAddressAku4.SetCurSel(m_cbAddressAku4.FindStringExact(-1, _T("0x2E0")));
		SetModified();
	}
	else 
	{
		if (AfxMessageBox(IDP_HARDWARE_PARTIAL_DELETE, MB_YESNO|MB_DEFBUTTON2) == IDNO) 
		{
			m_bAku4 = TRUE;
		}
		else 
		{
			SetModified();
		}
	}
	EnableDlgControls();
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::OnSelchangeAddressAku1() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::OnSelchangeAddressAku2() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::OnSelchangeAddressAku3() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::OnSelchangeAddressAku4() 
{
	SetModified();
}

/////////////////////////////////////////////////////////////////////////////
void CAKUPage::SetModified(BOOL bModified, BOOL bServerInit)
{
	// Basisklasse aufrufen
	CSVPage::SetModified(bModified, bServerInit);
	m_bRestartAkuUnit	= TRUE;
}
