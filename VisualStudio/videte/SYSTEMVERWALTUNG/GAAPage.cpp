// GAAPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "GAAPage.h"
						 
#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szModules[]   = _T("SecurityLauncher\\Modules");

/////////////////////////////////////////////////////////////////////////////
// CGAAPage dialog


CGAAPage::CGAAPage(CSVView* pParent) : CSVPage(CGAAPage::IDD)
{
	m_pParent = pParent;
	m_pInputList	= pParent->GetDocument()->GetInputs();
	//{{AFX_DATA_INIT(CGAAPage)
	m_bGAA = FALSE;
	m_sComGAA = _T("");
	m_sCurrency = _T("");
	//}}AFX_DATA_INIT
	Create(IDD,m_pParent);
}


void CGAAPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGAAPage)
	DDX_Control(pDX, IDC_COMBO_CURRENCY, m_cbCurrency);
	DDX_Control(pDX, IDC_COM_GAA, m_cbComGAA);
	DDX_Control(pDX, IDC_CHECK_GAA, m_checkGAA);
	DDX_Check(pDX, IDC_CHECK_GAA, m_bGAA);
	DDX_CBString(pDX, IDC_COM_GAA, m_sComGAA);
	DDX_CBString(pDX, IDC_COMBO_CURRENCY, m_sCurrency);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGAAPage, CSVPage)
	//{{AFX_MSG_MAP(CGAAPage)
	ON_CBN_SELCHANGE(IDC_COM_GAA, OnSelchangeComGaa)
	ON_BN_CLICKED(IDC_CHECK_GAA, OnCheckGAA)
	ON_CBN_EDITCHANGE(IDC_COMBO_CURRENCY, OnEditchangeComboCurrency)
	ON_CBN_SELCHANGE(IDC_COMBO_CURRENCY, OnSelchangeComboCurrency)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGAAPage message handlers

BOOL CGAAPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	int i;
	DWORD dwComMask;
	DWORD dwBit = 1;
	CString sFill;
	
	dwComMask = m_pParent->GetDocument()->GetCommPortInfo();

	CInputGroup* pInputGroup = NULL;
	CString sSMName, sCom;
	BOOL bCanBeUsed = TRUE;
	int	iCom232 = GetProfile().GetInt(WK_APP_NAME_COMMUNIT,_T("COM"),0);

	for (dwBit = 1,i=1; dwBit && dwBit<=dwComMask; dwBit<<=1,i++)
	{
		if (dwBit & dwComMask)
		{
			bCanBeUsed = TRUE;
			// Pruefen, ob diese Com-Schnittstelle bereits bei SDIUnit vergeben ist
			sCom.Format(_T("%d"), i);
			sSMName = SM_SDIUNIT_INPUT + sCom;
			pInputGroup = m_pInputList->GetGroupBySMName(sSMName);
			if ((pInputGroup) || (i==iCom232))
			{
				// COM bereits belegt
				bCanBeUsed = FALSE;
			}
			// Pruefen, ob diese Com-Schnittstelle bereits von der Maus belegt ist
			// todo

			// Wenn Com nicht belegt, dann in Listbox eintragen
			if (bCanBeUsed) {
				sFill.Format(_T("COM%d"),i);
				m_cbComGAA.AddString(sFill);
			}
		}
	}	

	CancelChanges();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
// Overrides
/////////////////////////////////////////////////////////////////////////////
BOOL CGAAPage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CGAAPage::SaveChanges()
{
	WK_TRACE_USER(_T("GAA-Einstellungen wurden geändert\n"));
	UpdateData();

	if (m_bGAA) 
	{
		CInputGroup* pInputGroup = NULL;
		// Daten der Inputgruppe aktualisieren, wenn bereits vorhanden...
		pInputGroup = m_pInputList->GetGroupBySMName(SM_GAUNIT_INPUT);
		if (pInputGroup) 
		{
			pInputGroup->SetSize(NR_INPUTS_GAA);
		}
		// ...sonst neue Gruppe anlegen
		else 
		{
			CString sMelder;
			sMelder.LoadString(IDS_ACTIVATION_INPUT);
			pInputGroup = m_pInputList->AddInputGroup(_T("DTP ")+sMelder, NR_INPUTS_GAA, SM_GAUNIT_INPUT);
		}

		int i;
		sscanf(m_sComGAA,_T("COM%d"),&i);
		GetProfile().WriteInt(_T("GAA"), _T("COM"),i);
		GetProfile().WriteString(szModules,WK_APP_NAME_GAUUNIT,_T("GAUnit.exe"));
		GetProfile().WriteString(_T("GAA"),_T("Currency"),m_sCurrency);
	}
	else 
	{
		m_pInputList->DeleteGroup(SM_GAUNIT_INPUT);
		GetProfile().WriteInt(_T("GAA"), _T("COM"), NULL);
		GetProfile().WriteString(_T("GAA"),_T("Currency"),NULL);
		GetProfile().WriteString(szModules,WK_APP_NAME_GAUUNIT,_T(""));
	}
	m_pInputList->Save(GetProfile(),IsLocal());
	SetModified(FALSE,FALSE);
	m_pParent->OnInputChanged();
}
/////////////////////////////////////////////////////////////////////////////
void CGAAPage::CancelChanges()
{
	int iIndex = -1;
	int i;

	CInputGroup* pInputGroup = NULL;
	m_bGAA = FALSE;
	pInputGroup = m_pInputList->GetGroupBySMName(SM_GAUNIT_INPUT);
	if ( pInputGroup ) 
	{
		m_bGAA = TRUE;
	}
	if (m_bGAA)
	{
		i = GetProfile().GetInt(_T("GAA"), _T("COM"), 2);
		m_sComGAA.Format(_T("COM%d"),i);
		if (!m_sComGAA.IsEmpty()) {
			m_bGAA = TRUE;
			iIndex = m_cbComGAA.FindStringExact(-1, m_sComGAA);
			m_cbComGAA.SetCurSel(iIndex);
		}
		else {
			m_cbComGAA.SetCurSel(-1);
		}
		m_sCurrency = GetProfile().GetString(_T("GAA"),_T("Currency"),_T(""));
	}

	UpdateData(FALSE);
	EnableDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
void CGAAPage::OnCheckGAA() 
{
	UpdateData(TRUE);
	if (!m_bGAA ) 
	{
		m_cbComGAA.SetCurSel(-1);
	}
	SetModified();
	EnableDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
void CGAAPage::EnableDlgControls()
{
	m_cbComGAA.EnableWindow(m_bGAA);
	m_cbCurrency.EnableWindow(m_bGAA);
}
/////////////////////////////////////////////////////////////////////////////
void CGAAPage::OnSelchangeComGaa() 
{
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CGAAPage::OnEditchangeComboCurrency() 
{
	UpdateData();
	if (m_sCurrency.GetLength() > 3) {
		MessageBeep((UINT)-1);
		m_sCurrency = m_sCurrency.Left(3);
		m_cbCurrency.SetWindowText(m_sCurrency);
		m_cbCurrency.SetEditSel( m_sCurrency.GetLength(), -1 );
	}
	SetModified();
}
/////////////////////////////////////////////////////////////////////////////
void CGAAPage::OnSelchangeComboCurrency() 
{
	UpdateData();
	SetModified();
}
