/* GlobalReplace: WK_WriteProfileInt --> GetProfile().WriteInt */
/* GlobalReplace: GetProfile().WriteInt --> GetProfile().WriteInt */
/* GlobalReplace: WK_GetProfileString --> GetProfile().GetString */
/* GlobalReplace: WK_GetProfileInt --> GetProfile().GetInt */
// PTUnitPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "PTUnitPage.h"

#include "SVDoc.h"
#include "SVView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szSection1[] = _T("PTUnit");
static TCHAR BASED_CODE szSection2[] = _T("PTUnit2");

static TCHAR BASED_CODE szProcessorUsage[] = _T("ProcessorUsage"); // int 1(High)..3(Low)
static TCHAR BASED_CODE szEnableKnockKnockBox[] = _T("EnableKnockKnockBox"); // int 1(On)
static TCHAR BASED_CODE szOwnNumber[] = _T("OwnNumber"); // string
static TCHAR BASED_CODE szNrBChannels[] = _T("NrBChannels"); // int 1,2
static TCHAR BASED_CODE szSupportMSN[] = _T("SupportMSN"); // int 0,1


/////////////////////////////////////////////////////////////////////////////
// CPTUnitPage dialog



CPTUnitPage::CPTUnitPage(CSVView* pParent, int nPtNr) : CSVPage(CPTUnitPage::IDD)
{
	CString sTemp;
	m_pParent	= pParent;
	m_nPtNr		= nPtNr;		// Nummer der PT-Unit (1, 2)

	if (m_nPtNr == PT1)
	{
		m_sSection = szSection1;
		m_sAppName = WK_APP_NAME_PTUNIT;
		sTemp = GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_PTUNIT2,_T(""));
		m_bPTUnitOther = sTemp.CompareNoCase(_T("PTUnit.exe"))==0; 
	}		 
	else
	{
		m_sSection = szSection2;
		m_sAppName = WK_APP_NAME_PTUNIT2;
		sTemp = GetProfile().GetString(theApp.GetModuleSection(),WK_APP_NAME_PTUNIT,_T(""));
		m_bPTUnitOther = sTemp.CompareNoCase(_T("PTUnit.exe"))==0; 
	}

	//{{AFX_DATA_INIT(CPTUnitPage)
	m_bPTUnit = FALSE;
	m_sRufNummer = _T("A");
	m_bKnock = TRUE;
	m_bSupportMSN = FALSE;
	m_iSystem = 0;
	m_iAccept = 0;
	//}}AFX_DATA_INIT
	Create(IDD,pParent);
}
void CPTUnitPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPTUnitPage)
	DDX_Control(pDX, IDC_CHECK_PTUNIT, m_btnPTUnit);
	DDX_Control(pDX, IDC_RADIO_NR, m_btnNr);
	DDX_Control(pDX, IDC_RADIO_ALL, m_btnAll);
	DDX_Control(pDX, IDC_RADIO_NOTHING, m_btnNothing);
	DDX_Control(pDX, IDC_USAGE_LOW, m_btnLow);
	DDX_Control(pDX, IDC_USAGE_MEDIUM, m_btnMedium);
	DDX_Control(pDX, IDC_USAGE_HIGH, m_btnHigh);
	DDX_Control(pDX, IDC_ENABLE_KNOCK_BOX, m_btnKnock);
	DDX_Control(pDX, IDC_RUFNUMMER, m_editRufnummer);
	DDX_Text(pDX, IDC_RUFNUMMER, m_sRufNummer);
	DDX_Check(pDX, IDC_ENABLE_KNOCK_BOX, m_bKnock);
	DDX_Check(pDX, IDC_CHECK_MSN, m_bSupportMSN);
	DDX_Radio(pDX, IDC_USAGE_HIGH, m_iSystem);
	DDX_Radio(pDX, IDC_RADIO_NR, m_iAccept);
	DDX_Check(pDX, IDC_CHECK_PTUNIT, m_bPTUnit);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPTUnitPage, CSVPage)
	//{{AFX_MSG_MAP(CPTUnitPage)
	ON_BN_CLICKED(IDC_CHECK_PTUNIT, OnCheckPTUnit)
	ON_EN_CHANGE(IDC_RUFNUMMER, OnChangeRufnummer)
	ON_BN_CLICKED(IDC_ENABLE_KNOCK_BOX, OnChange)
	ON_BN_CLICKED(IDC_USAGE_HIGH, OnChange)
	ON_BN_CLICKED(IDC_USAGE_LOW, OnChange)
	ON_BN_CLICKED(IDC_USAGE_MEDIUM, OnChange)
	ON_BN_CLICKED(IDC_RADIO_ALL, OnChange)
	ON_BN_CLICKED(IDC_RADIO_NOTHING, OnChange)
	ON_BN_CLICKED(IDC_RADIO_NR, OnChange)
	ON_BN_CLICKED(IDC_RADIO_B1, OnChange)
	ON_BN_CLICKED(IDC_RADIO_B2, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPTUnitPage message handlers

BOOL CPTUnitPage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CPTUnitPage::SaveChanges()
{
	WK_TRACE_USER(_T("PT-Modul-Einstellungen wurden geändert\n"));
	UpdateData();
	if (m_nPtNr == PT1)
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_PTUNIT,m_bPTUnit  ? _T("PtUnit.exe") : _T(""));
	else
		GetProfile().WriteString(theApp.GetModuleSection(),WK_APP_NAME_PTUNIT2,m_bPTUnit ? _T("PtUnit.exe") : _T(""));

	GetProfile().WriteInt(m_sSection,szEnableKnockKnockBox,m_bKnock);
	GetProfile().WriteInt(m_sSection,szProcessorUsage,m_iSystem+1);

	CString sTemp;
	sTemp = GetProfile().GetString(theApp.GetModuleSection(),m_sAppName,_T(""));
	m_bPTUnit = sTemp.CompareNoCase(_T("PTUnit.exe"))==0; 

	switch (m_iAccept)
	{
	case 0:
		sTemp = m_sRufNummer;
		break;
	case 1:
		sTemp = _T("A");
		break;
	case 2:
		sTemp = _T("R");
		break;
	default:
		sTemp = _T("A");
	}

	CWK_Profile::Encode(sTemp);
#ifdef _UNICODE
	GetProfile().WriteString(m_sSection,szOwnNumber,sTemp,TRUE);
#else
	GetProfile().WriteString(m_sSection,szOwnNumber,sTemp);
#endif
}
/////////////////////////////////////////////////////////////////////////////
void CPTUnitPage::CancelChanges()
{
	CString sTemp;
	sTemp = GetProfile().GetString(theApp.GetModuleSection(),m_sAppName,_T(""));
	m_bPTUnit = sTemp.CompareNoCase(_T("PTUnit.exe"))==0; 

	m_bKnock = GetProfile().GetInt(m_sSection,szEnableKnockKnockBox,TRUE);
	m_bSupportMSN = GetProfile().GetInt(m_sSection,szSupportMSN,FALSE);
	m_iSystem = GetProfile().GetInt(m_sSection,szProcessorUsage,2)-1;

	CString sTmp;
#ifdef _UNICODE
	sTmp = GetProfile().GetString(m_sSection, szOwnNumber, _T(""), TRUE);
#else
	sTmp = GetProfile().GetString(m_sSection, szOwnNumber, _T(""));
#endif	
	if (!sTmp.IsEmpty())
	{
		CWK_Profile::Decode(sTmp);
		sTmp.TrimRight();
		
		if ((sTmp==_T("A")) || (sTmp==_T("a")))
		{
			m_sRufNummer = _T("");
			m_iAccept = 1;
		}
		else if ((sTmp==_T("R")) || (sTmp==_T("r")))
		{
			m_sRufNummer = _T("");
			m_iAccept = 2;
		}
		else
		{
			m_sRufNummer = sTmp;
			m_iAccept = 0;
		}
	}
	else
	{
		m_sRufNummer = _T("");
		m_iAccept = 1;
	}

	UpdateData(FALSE);

	ShowHide();

}
/////////////////////////////////////////////////////////////////////////////
void CPTUnitPage::OnCheckPTUnit() 
{
	SetModified();
	ShowHide();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CPTUnitPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	CString sTemp;
	sTemp = GetProfile().GetString(theApp.GetModuleSection(),m_sAppName,_T(""));
	m_bPTUnit = sTemp.CompareNoCase(_T("PTUnit.exe"))==0; 

	if (m_nPtNr == PT1)
	{
		if (m_bPTUnitOther)
			m_btnPTUnit.EnableWindow(FALSE);
	}
	else
	{
		if (!m_bPTUnitOther)
			m_btnPTUnit.EnableWindow(FALSE);
	}

	UpdateData(FALSE);

	CancelChanges();
	
	m_btnNothing.ShowWindow(SW_HIDE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CPTUnitPage::ShowHide()
{
	UpdateData();

	m_btnNr.EnableWindow(m_bPTUnit);
	m_btnAll.EnableWindow(m_bPTUnit);
	m_btnNothing.EnableWindow(m_bPTUnit);
	m_btnLow.EnableWindow(m_bPTUnit);
	m_btnMedium.EnableWindow(m_bPTUnit);
	m_btnHigh.EnableWindow(m_bPTUnit);
	m_btnKnock.EnableWindow(m_bPTUnit);
	m_editRufnummer.EnableWindow(m_bPTUnit && (m_iAccept==0));

}
/////////////////////////////////////////////////////////////////////////////
void CPTUnitPage::OnChangeRufnummer() 
{
	UpdateData();
	CString sTmp;

	int i,c;
	BOOL bValid = TRUE;

	c = m_sRufNummer.GetLength();
	for (i=0;i<c;i++)
	{
		if (isdigit(m_sRufNummer[i]) || 
			(m_sRufNummer[i]==_T(' ')) || 
			(m_sRufNummer[i]==_T('-')) || 
			(m_sRufNummer[i]==_T('/')))
		{
			sTmp += m_sRufNummer[i];
		}
		else
		{
			bValid = FALSE;
			break;
		}
	}

	if (!bValid)
	{
		m_sRufNummer = sTmp;
		UpdateData(FALSE);
		m_editRufnummer.SetSel(i,i);
	}
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CPTUnitPage::OnChange() 
{
	SetModified(TRUE,FALSE);
	ShowHide();
	m_pParent->AddInitApp(WK_APP_NAME_PTUNIT);
}
