// SimPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SimPage.h"

#include "SVDoc.h"
#include "SVView.h"

#include "ICPCONConfigPage.h"
#include <ICPCONDll\ICP7000Module.h>
#include ".\simpage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


#define SWITCH_PANEL_SETTINGS	REG_LOC_UNIT SETTINGS_SECTION
/////////////////////////////////////////////////////////////////////////////
// CSimPage dialog


CSimPage::CSimPage(CSVView* pParent) : CSVPage(CSimPage::IDD)
{
	m_pParent = pParent;
	m_pInputList	= pParent->GetDocument()->GetInputs();
	m_pOutputList	= pParent->GetDocument()->GetOutputs();
	//{{AFX_DATA_INIT(CSimPage)
	m_bSimUnit = FALSE;
	m_bSwitchPanel = FALSE;
	m_nButtonWidth = 100;
	m_nButtonHeight = 75;
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)pParent);
}


void CSimPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSimPage)
	DDX_Check(pDX, IDC_CHECK_SIMUNIT, m_bSimUnit);
	DDX_Check(pDX, IDC_CK_ENABLE_SWITCH_PANEL, m_bSwitchPanel);
	DDX_Text(pDX, IDC_EDT_BTN_CX, m_nButtonWidth);
	DDX_Text(pDX, IDC_EDT_BTN_CY, m_nButtonHeight);
	DDX_Control(pDX, IDC_IPC_COMBO_MINIMIZE, m_comboMinimize);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSimPage, CSVPage)
	//{{AFX_MSG_MAP(CSimPage)
	ON_BN_CLICKED(IDC_CHECK_SIMUNIT, OnCheckSimunit)
	ON_BN_CLICKED(IDC_CK_ENABLE_SWITCH_PANEL, OnBnClickedCkEnableSwitchPanel)
	ON_EN_CHANGE(IDC_EDT_BTN_CX, OnChange)
	ON_EN_CHANGE(IDC_EDT_BTN_CY, OnChange)
	ON_CBN_SELCHANGE(IDC_IPC_COMBO_MINIMIZE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimPage message handlers
BOOL CSimPage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSimPage::SaveChanges()
{
	WK_TRACE_USER(_T("SIM-Modul-Einstellungen wurden geändert\n"));
	UpdateData();
	GetDlgSimUnitData();

	SaveInputsAndOutputsAfterCheckingActivations();
	SetModified(FALSE,FALSE);

	GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_SIMUNIT,m_bSimUnit ? _T("SimUnit.exe") : _T(""));

	if (theApp.m_bProfessional)
	{
		CInputList  *pInputList = m_pParent->GetDocument()->GetInputs();
		CString sIcpConModules = REG_LOC_UNIT REG_LOC_MODULE_UNIT;
		CStringArray sa;
		GetProfile().EnumRegKeys(sIcpConModules, sa);
		if (sa.GetSize() && m_bSwitchPanel)
		{
			GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_ICPCON_UNIT, WK_APP_NAME_ICPCON_UNIT _T(".exe"));
		}
		else
		{
			GetProfile().DeleteEntry(theApp.GetModuleSection(),WK_APP_NAME_ICPCON_UNIT);
		}
		GetProfile().WriteInt(SWITCH_PANEL_SETTINGS, ICP_SWITCH_PANEL, m_bSwitchPanel);
		if (m_bSwitchPanel)
		{
			CInputGroup * pInputGroup = pInputList->GetGroupBySMName(SM_ICPCONUnitInput);
			if (pInputGroup)
			{
				pInputList = NULL;					// no saving nessesary
			}
			else
			{
				CString strName;
				strName.LoadString(IDS_SWITCHES);
				pInputGroup = pInputList->AddInputGroup(strName, 32, SM_ICPCONUnitInput);
				strName.LoadString(IDS_SWITCH_PANEL);
				pInputGroup->SetName(strName);
			}

			GetProfile().WriteInt(SWITCH_PANEL_SETTINGS, ICP_BUTTON_DX, m_nButtonWidth);
			GetProfile().WriteInt(SWITCH_PANEL_SETTINGS, ICP_BUTTON_DY, m_nButtonHeight);
			CString sMinPos;
			switch(m_comboMinimize.GetCurSel())
			{
				case 1:  sMinPos = MINPOS_LEFTTOP;     break;
				case 2:  sMinPos = MINPOS_LEFTBOTTOM;  break;
				case 3:  sMinPos = MINPOS_RIGHTTOP;    break;
				case 4:  sMinPos = MINPOS_RIGHTBOTTOM; break;
				default: sMinPos = MINPOS_NONE;        break;
			}
			GetProfile().WriteString(SWITCH_PANEL_SETTINGS, ICP_MINIMIZE_POSITION, sMinPos);
		}
		else
		{
			CInputGroup * pInputGroup = pInputList->GetGroupBySMName(SM_ICPCONUnitInput);
			if (pInputGroup)
			{
				CheckActivationsWithInputGroup(pInputGroup->GetID(), SVP_CO_CHECK_SILENT);
				pInputList->DeleteGroup(pInputGroup->GetSMName());
			}
			else
			{
				pInputList = NULL;						// no saving nessesary
			}
		}
		if (pInputList)
		{
			pInputList->Save(GetProfile(), TRUE);
		}
	}
	m_pParent->PostMessage(WM_USER, USER_MSG_UPDATE_PARENT_TREE, (1<<IMAGE_INPUT)|(1<<IMAGE_RELAY));
}
/////////////////////////////////////////////////////////////////////////////
void CSimPage::CancelChanges()
{
	CInputGroup* pInputGroup = NULL;

	// clear checkboxes
	m_bSimUnit = FALSE;
	// Die SimUnit hat immer Inputs und Outputs,
	// daher wird z.Z. nur nach der Inputgruppe gesucht OOPS 
	pInputGroup = m_pInputList->GetGroupBySMName(SM_SIMUNIT_INPUT);
	// Anzahl der Inputs einlesen
	if ( pInputGroup ) 
	{
		m_bSimUnit = TRUE;
	}

	if (theApp.m_bProfessional)
	{
		m_bSwitchPanel  = GetProfile().GetInt(SWITCH_PANEL_SETTINGS, ICP_SWITCH_PANEL, 0);
		m_nButtonWidth  = GetProfile().GetInt(SWITCH_PANEL_SETTINGS, ICP_BUTTON_DX, m_nButtonWidth);
		m_nButtonHeight = GetProfile().GetInt(SWITCH_PANEL_SETTINGS, ICP_BUTTON_DY, m_nButtonHeight);
		CString sMinPos = GetProfile().GetString(SWITCH_PANEL_SETTINGS, ICP_MINIMIZE_POSITION, NULL);
		if (sMinPos.IsEmpty())
		{
			m_comboMinimize.SetCurSel(0);
		}
		else
		{
			sMinPos.MakeLower();
			if      (sMinPos == MINPOS_LEFTTOP    ) m_comboMinimize.SetCurSel(1);
			else if (sMinPos == MINPOS_LEFTBOTTOM ) m_comboMinimize.SetCurSel(2);
			else if (sMinPos == MINPOS_RIGHTTOP   ) m_comboMinimize.SetCurSel(3);
			else if (sMinPos == MINPOS_RIGHTBOTTOM) m_comboMinimize.SetCurSel(4);
			else                                    m_comboMinimize.SetCurSel(0);
		}
		m_comboMinimize.EnableWindow(m_bSwitchPanel);
		GetDlgItem(IDC_EDT_BTN_CX)->EnableWindow(m_bSwitchPanel);
		GetDlgItem(IDC_EDT_BTN_CY)->EnableWindow(m_bSwitchPanel);
	}
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CSimPage::OnCheckSimunit() 
{
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CSimPage::GetDlgSimUnitData() 
{
	CInputGroup* pInputGroup = NULL;
	COutputGroup* pOutputGroup = NULL;
	int iAnzahlMelder = 16;
	int iAnzahlRelais = 4;

	if ( m_bSimUnit ) 
	{
		// Daten der Inputgruppe aktualisieren, wenn bereits vorhanden...
		pInputGroup = m_pInputList->GetGroupBySMName(SM_SIMUNIT_INPUT);
		if ( pInputGroup) 
		{
			pInputGroup->SetSize((WORD)iAnzahlMelder);
		}
		// ...sonst neue Gruppe anlegen
		else 
		{
			pInputGroup = m_pInputList->AddInputGroup(NULL, iAnzahlMelder, SM_SIMUNIT_INPUT);
		}
		// Daten der Outputgruppe aktualisieren, wenn bereits vorhanden...
		pOutputGroup = m_pOutputList->GetGroupBySMName(SM_SIMUNIT_OUTPUT);
		if ( pOutputGroup) 
		{
			pOutputGroup->SetSize((WORD)iAnzahlRelais);
		}
		// ...sonst neue Gruppe anlegen
		else 
		{
			pOutputGroup = m_pOutputList->AddOutputGroup(NULL, iAnzahlRelais, SM_SIMUNIT_OUTPUT);
		}
	}
	else 
	{
		// Gruppen loeschen, wenn keine SimUnit ausgewaehlt
		m_pInputList->DeleteGroup(SM_SIMUNIT_INPUT);
		m_pOutputList->DeleteGroup(SM_SIMUNIT_OUTPUT);
	}
}
///////////////////////////////////////////////////////////////////
BOOL CSimPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	if (theApp.m_bProfessional)
	{
		CString sLeft, sTop, sRight, sBottom, sNothing;
		sNothing.LoadString(IDS_NOTHING);
		sLeft.LoadString(IDS_LEFT);
		sRight.LoadString(IDS_RIGHT);
		sTop.LoadString(IDS_TOP);
		sBottom.LoadString(IDS_BOTTOM);
		
		m_comboMinimize.AddString(sNothing);
		sNothing = _T(", ");
		m_comboMinimize.AddString(sLeft + sNothing + sTop);
		m_comboMinimize.AddString(sLeft + sNothing + sBottom);
		m_comboMinimize.AddString(sRight + sNothing + sTop);
		m_comboMinimize.AddString(sRight + sNothing + sBottom);

		GetDlgItem(IDC_CK_ENABLE_SWITCH_PANEL)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TXT_BTN_SIZE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_LINE_THREE)->ShowWindow(SW_SHOW);

		GetDlgItem(IDC_EDT_BTN_CX)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EDT_BTN_CY)->ShowWindow(SW_SHOW);
		m_comboMinimize.ShowWindow(SW_SHOW);

		GetDlgItem(IDC_EDT_BTN_CX)->EnableWindow(m_bSwitchPanel);
		GetDlgItem(IDC_EDT_BTN_CY)->EnableWindow(m_bSwitchPanel);
		m_comboMinimize.EnableWindow(m_bSwitchPanel);
	}
	CancelChanges();

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
///////////////////////////////////////////////////////////////////
void CSimPage::OnBnClickedCkEnableSwitchPanel()
{
	CDataExchange dx(this, TRUE);
	DDX_Check(&dx, IDC_CK_ENABLE_SWITCH_PANEL, m_bSwitchPanel);
	if (!m_bSwitchPanel)
	{
		CInputList  *pInputList  = m_pParent->GetDocument()->GetInputs();
		CInputGroup *pInputGroup = pInputList->GetGroupBySMName(SM_ICPCONUnitInput);
		if (pInputGroup)
		{
			BOOL bNoActivations = CheckActivationsWithInputGroup(pInputGroup->GetID());
			if (!bNoActivations)
			{
				dx.m_bSaveAndValidate = FALSE;
				m_bSwitchPanel = TRUE;
				DDX_Check(&dx, IDC_CK_ENABLE_SWITCH_PANEL, m_bSwitchPanel);
				return;
			}
		}
	}

	SetModified(TRUE, FALSE);
	GetDlgItem(IDC_EDT_BTN_CX)->EnableWindow(m_bSwitchPanel);
	GetDlgItem(IDC_EDT_BTN_CY)->EnableWindow(m_bSwitchPanel);
	m_comboMinimize.EnableWindow(m_bSwitchPanel);
}
//////////////////////////////////////////////////////////////////
void CSimPage::OnChange()
{
	SetModified(TRUE, FALSE);
}
