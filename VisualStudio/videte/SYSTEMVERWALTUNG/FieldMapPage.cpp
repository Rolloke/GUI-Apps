// FieldMapPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "FieldMapPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFieldMapPage dialog

CFieldMapPage::CFieldMapPage(CSVView* pParent) : CSVPage(CFieldMapPage::IDD)
{
	m_pParent = pParent;
	m_sVersion = GetProfile().GetString(_T("Version"),_T("Number"),_T(""));
	//{{AFX_DATA_INIT(CFieldMapPage)
	m_bKontoNr = FALSE;
	m_bBetrag = FALSE;
	m_bGANr = FALSE;
	m_bTANr = FALSE;
	m_bBLZ = FALSE;
	m_bDTPTime = FALSE;
	m_sKontoNr = _T("");
	m_sBetrag = _T("");
	m_sGANr = _T("");
	m_sTANr = _T("");
	m_sBLZ = _T("");
	m_sDTPTime = _T("");
	m_bDTPDate = FALSE;
	m_sDTPDate = _T("");
	//}}AFX_DATA_INIT
	Create(IDD,m_pParent);
}


void CFieldMapPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFieldMapPage)
	DDX_Control(pDX, IDC_EDIT_DTP_DATE, m_editDtpDate);
	DDX_Control(pDX, IDC_EDIT_TANR, m_editTANr);
	DDX_Control(pDX, IDC_EDIT_KTONR, m_editKtoNr);
	DDX_Control(pDX, IDC_EDIT_GANR, m_editGANr);
	DDX_Control(pDX, IDC_EDIT_DTP_TIME, m_editDTPTime);
	DDX_Control(pDX, IDC_EDIT_BLZ, m_editBLZ);
	DDX_Control(pDX, IDC_EDIT_BETRAG, m_editBetrag);
	DDX_Check(pDX, IDC_CHECK_KTONR, m_bKontoNr);
	DDX_Check(pDX, IDC_CHECK_BETRAG, m_bBetrag);
	DDX_Check(pDX, IDC_CHECK_GANR, m_bGANr);
	DDX_Check(pDX, IDC_CHECK_TANR, m_bTANr);
	DDX_Check(pDX, IDC_CHECK_BLZ, m_bBLZ);
	DDX_Check(pDX, IDC_CHECK_DTP_ZEIT, m_bDTPTime);
	DDX_Text(pDX, IDC_EDIT_KTONR, m_sKontoNr);
	DDX_Text(pDX, IDC_EDIT_BETRAG, m_sBetrag);
	DDX_Text(pDX, IDC_EDIT_GANR, m_sGANr);
	DDX_Text(pDX, IDC_EDIT_TANR, m_sTANr);
	DDX_Text(pDX, IDC_EDIT_BLZ, m_sBLZ);
	DDX_Text(pDX, IDC_EDIT_DTP_TIME, m_sDTPTime);
	DDX_Check(pDX, IDC_CHECK_DTP_DATE, m_bDTPDate);
	DDX_Text(pDX, IDC_EDIT_DTP_DATE, m_sDTPDate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFieldMapPage, CSVPage)
	//{{AFX_MSG_MAP(CFieldMapPage)
	ON_BN_CLICKED(IDC_CHECK_BETRAG, OnCheckBetrag)
	ON_BN_CLICKED(IDC_CHECK_BLZ, OnCheckBlz)
	ON_BN_CLICKED(IDC_CHECK_DTP_ZEIT, OnCheckDtpZeit)
	ON_BN_CLICKED(IDC_CHECK_GANR, OnCheckGanr)
	ON_BN_CLICKED(IDC_CHECK_KTONR, OnCheckKtonr)
	ON_BN_CLICKED(IDC_CHECK_TANR, OnCheckTanr)
	ON_BN_CLICKED(IDC_CHECK_DTP_DATE, OnCheckDtpDate)
	ON_BN_CLICKED(IDC_BUTTON_STANDARD, OnButtonStandard)
	ON_EN_CHANGE(IDC_EDIT_BETRAG, OnChange)
	ON_EN_CHANGE(IDC_EDIT_BLZ, OnChange)
	ON_EN_CHANGE(IDC_EDIT_DTP_TIME, OnChange)
	ON_EN_CHANGE(IDC_EDIT_GANR, OnChange)
	ON_EN_CHANGE(IDC_EDIT_KTONR, OnChange)
	ON_EN_CHANGE(IDC_EDIT_TANR, OnChange)
	ON_EN_CHANGE(IDC_EDIT_DTP_DATE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFieldMapPage message handlers
/////////////////////////////////////////////////////////////////////////////
BOOL CFieldMapPage::IsDataValid()
{
	UpdateData();
	if (   (m_bKontoNr && m_sKontoNr.IsEmpty()) 
		|| (m_bBetrag && m_sBetrag.IsEmpty()) 
		|| (m_bGANr && m_sGANr.IsEmpty()) 
		|| (m_bTANr && m_sTANr.IsEmpty()) 
		|| (m_bBLZ && m_sBLZ.IsEmpty()) 
		|| (m_bDTPTime && m_sDTPTime.IsEmpty()) 
		|| (m_bDTPDate && m_sDTPDate.IsEmpty()) 
	   )
	{
		AfxMessageBox(IDP_NOEMPTY_FIELDS,MB_OK|MB_ICONSTOP);
		return FALSE;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::SaveChanges()
{
	WK_TRACE_USER(_T("Suchmasken-Einstellungen wurden geändert\n"));
	CWK_Profile& wkp = GetProfile();

	wkp.WriteString(szFieldMapSection,CIPCField::m_sfKtNr,
					m_bKontoNr ? (LPCTSTR)m_sKontoNr : szDontUse);
	wkp.WriteString(szFieldMapSection,CIPCField::m_sfAmnt,
		m_bBetrag ? (LPCTSTR)m_sBetrag : szDontUse);
	wkp.WriteString(szFieldMapSection,CIPCField::m_sfGaNr,
		m_bGANr ? (LPCTSTR)m_sGANr : szDontUse);
	wkp.WriteString(szFieldMapSection,CIPCField::m_sfTaNr,
		m_bTANr ? (LPCTSTR)m_sTANr : szDontUse);
	wkp.WriteString(szFieldMapSection,CIPCField::m_sfBcNr,
		m_bBLZ ? (LPCTSTR)m_sBLZ : szDontUse);
	wkp.WriteString(szFieldMapSection,CIPCField::m_sfTime,
		m_bDTPTime ? (LPCTSTR)m_sDTPTime : szDontUse);
	wkp.WriteString(szFieldMapSection,CIPCField::m_sfDate,
		m_bDTPDate ? (LPCTSTR)m_sDTPDate : szDontUse);
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::CancelChanges()
{
	CWK_Profile& wkp = GetProfile();

	m_sKontoNr = wkp.GetString(szFieldMapSection,CIPCField::m_sfKtNr,_T(""));
	m_sBetrag = wkp.GetString(szFieldMapSection,CIPCField::m_sfAmnt,_T(""));
	m_sGANr = wkp.GetString(szFieldMapSection,CIPCField::m_sfGaNr,_T(""));
	m_sTANr = wkp.GetString(szFieldMapSection,CIPCField::m_sfTaNr,_T(""));
	m_sBLZ = wkp.GetString(szFieldMapSection,CIPCField::m_sfBcNr,_T(""));
	m_sDTPTime = wkp.GetString(szFieldMapSection,CIPCField::m_sfTime,_T(""));
	m_sDTPDate = wkp.GetString(szFieldMapSection,CIPCField::m_sfDate,_T(""));

	if (m_sKontoNr.IsEmpty())
		m_sKontoNr.LoadString(IDS_KTO_NR);
	if (m_sBetrag.IsEmpty())
		m_sBetrag.LoadString(IDS_VALUE);
	if (m_sGANr.IsEmpty())
		m_sGANr.LoadString(IDS_GA_NR);
	if (m_sTANr.IsEmpty())
		m_sTANr.LoadString(IDS_TA_NR);
	if (m_sBLZ.IsEmpty())
		m_sBLZ.LoadString(IDS_BANKLEITZAHL);
	if (m_sDTPTime.IsEmpty())
		m_sDTPTime.LoadString(IDS_DTP_TIME);
	if (m_sDTPDate.IsEmpty())
		m_sDTPDate.LoadString(IDS_DTP_DATE);

	m_bKontoNr = (m_sKontoNr != szDontUse);
	m_bBetrag  = (m_sBetrag != szDontUse);
	m_bGANr    = (m_sGANr != szDontUse);
	m_bTANr    = (m_sTANr != szDontUse);
	m_bBLZ     = (m_sBLZ != szDontUse);
	m_bDTPTime = (m_sDTPTime != szDontUse);
	m_bDTPDate = (m_sDTPDate != szDontUse);

	UpdateData(FALSE);
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::OnCheckBetrag() 
{
	UpdateData();
	if (!m_bBetrag)
	{
		m_sBetrag.Empty();
		UpdateData(FALSE);
	}
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::OnCheckBlz() 
{
	UpdateData();
	if (!m_bBLZ)
	{
		m_sBLZ.Empty();
		UpdateData(FALSE);
	}
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::OnCheckDtpZeit() 
{
	UpdateData();
	if (!m_bDTPTime)
	{
		m_sDTPTime.Empty();
		UpdateData(FALSE);
	}
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::OnCheckDtpDate() 
{
	UpdateData();
	if (!m_bDTPDate)
	{
		m_sDTPDate.Empty();
		UpdateData(FALSE);
	}
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::OnCheckGanr() 
{
	UpdateData();
	if (!m_bGANr)
	{
		m_sGANr.Empty();
		UpdateData(FALSE);
	}
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::OnCheckKtonr() 
{
	UpdateData();
	if (!m_bKontoNr)
	{
		m_sKontoNr.Empty();
		UpdateData(FALSE);
	}
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::OnCheckTanr() 
{
	UpdateData();
	if (!m_bTANr)
	{
		m_sTANr.Empty();
		UpdateData(FALSE);
	}
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::OnChange() 
{
	SetModified(TRUE,FALSE);
	m_pParent->AddInitApp(WK_APP_NAME_IDIP_CLIENT);
	EnableControls();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CFieldMapPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	CancelChanges();
	EnableControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::EnableControls()
{
	m_editTANr.EnableWindow(m_bTANr);
	m_editKtoNr.EnableWindow(m_bKontoNr);
	m_editGANr.EnableWindow(m_bGANr);
	m_editDTPTime.EnableWindow(m_bDTPTime);
	m_editDtpDate.EnableWindow(m_bDTPDate);
	m_editBLZ.EnableWindow(m_bBLZ);
	m_editBetrag.EnableWindow(m_bBetrag);
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::LoadStandard()
{
	m_sKontoNr.LoadString(IDS_KTO_NR);
	m_sBetrag.LoadString(IDS_VALUE);
	m_sGANr.LoadString(IDS_GA_NR);
	m_sTANr.LoadString(IDS_TA_NR);
	m_sBLZ.LoadString(IDS_BANKLEITZAHL);
	m_sDTPTime.LoadString(IDS_DTP_TIME);
	m_sDTPDate.LoadString(IDS_DTP_DATE);
}
/////////////////////////////////////////////////////////////////////////////
void CFieldMapPage::OnButtonStandard() 
{
	CWK_Dongle dongle;

	m_sKontoNr.Empty();
	m_sBetrag.Empty();
	m_sGANr.Empty();
	m_sTANr.Empty();
	m_sBLZ.Empty();
	m_sDTPTime.Empty();
	m_sDTPDate.Empty();

	if (dongle.RunSDIUnit() || dongle.IsReceiver())
	{
		LoadStandard();
	}

	m_bKontoNr = !m_sKontoNr.IsEmpty();
	m_bBetrag = !m_sBetrag.IsEmpty();
	m_bGANr = !m_sGANr.IsEmpty();
	m_bTANr = !m_sTANr.IsEmpty();
	m_bBLZ = !m_sBLZ.IsEmpty();
	m_bDTPTime = !m_sDTPTime.IsEmpty();
	m_bDTPDate = !m_sDTPDate.IsEmpty();

	UpdateData(FALSE);
	OnChange();
}
