// MESZPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "MESZPage.h"

#include "SVDoc.h"
#include "SVView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CMESZPage dialog

CMESZPage::CMESZPage(CSVView* pParent) : CSVPage(CMESZPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CMESZPage)
	m_bAutoMESZ = TRUE;
	m_monthMESZ = 3;
	m_monthMEZ = 10;
	m_bDCF = FALSE;
	m_sFunkUhr = _T("");
	//}}AFX_DATA_INIT
	Create(IDD,pParent);
}


void CMESZPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMESZPage)
	DDX_Control(pDX, IDC_EDIT_FUNKUHR, m_editFunkUhr);
	DDX_Control(pDX, IDC_CHECK_MEZ, m_btnAutoMESZ);
	DDX_Control(pDX, IDC_CHECK_DCF, m_btnDCF);
	DDX_Control(pDX, IDC_COMBO_SMONTH, m_cbMonthMEZ);
	DDX_Control(pDX, IDC_COMBO_WMONTH, m_cbMonthMESZ);
	DDX_Check(pDX, IDC_CHECK_MEZ, m_bAutoMESZ);
	DDX_CBIndex(pDX, IDC_COMBO_WMONTH, m_monthMESZ);
	DDX_CBIndex(pDX, IDC_COMBO_SMONTH, m_monthMEZ);
	DDX_Check(pDX, IDC_CHECK_DCF, m_bDCF);
	DDX_Text(pDX, IDC_EDIT_FUNKUHR, m_sFunkUhr);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMESZPage, CSVPage)
	//{{AFX_MSG_MAP(CMESZPage)
	ON_BN_CLICKED(IDC_CHECK_MEZ, OnCheckMesz)
	ON_CBN_SELCHANGE(IDC_COMBO_SMONTH, OnSelchangeMonthMEZ)
	ON_CBN_SELCHANGE(IDC_COMBO_WMONTH, OnSelchangeMonthMESZ)
	ON_BN_CLICKED(IDC_CHECK_DCF, OnCheckDcf)
	ON_EN_CHANGE(IDC_EDIT_FUNKUHR, OnChangeEditFunkuhr)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
void CMESZPage::SetOEMSpecificControls()
{
	CString sTextOld, sTextNew;

	sTextNew = COemGuiApi::GetApplicationName(WAI_LAUNCHER);
	m_btnAutoMESZ.GetWindowText(sTextOld);
	m_btnAutoMESZ.SetWindowText(sTextOld + _T(" ") + sTextNew);
}
/////////////////////////////////////////////////////////////////////////////
void CMESZPage::EnableDlgControls()
{

	if (theApp.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS) {
		m_btnAutoMESZ.EnableWindow(TRUE);
		m_cbMonthMEZ.EnableWindow(m_bAutoMESZ);
		m_cbMonthMESZ.EnableWindow(m_bAutoMESZ);
	}
	else {
		m_btnAutoMESZ.EnableWindow(FALSE);
		m_cbMonthMEZ.EnableWindow(FALSE);
		m_cbMonthMESZ.EnableWindow(FALSE);
	}
	if (m_bDCF)
		m_editFunkUhr.EnableWindow(TRUE);
	else
		m_editFunkUhr.EnableWindow(FALSE);

}

/////////////////////////////////////////////////////////////////////////////
BOOL CMESZPage::IsDataValid()
{
	UpdateData(TRUE);

	if (m_bAutoMESZ && m_bDCF)
	{
		AfxMessageBox(IDP_MESZ_DCF,MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	if (m_bDCF && m_sFunkUhr.IsEmpty())
	{
		AfxMessageBox(IDS_NO_RC_MODULE,MB_OK|MB_ICONSTOP);
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CMESZPage::SaveChanges()
{
	WK_TRACE_USER(_T("Automatische Zeitumstellung-Einstellungen wurden geändert\n"));
//	UpdateData();

	GetProfile().WriteInt(theApp.GetLauncherSection(),_T("AutoMESZ"),m_bAutoMESZ);

	if (m_bAutoMESZ)
	{
		GetProfile().WriteInt(theApp.GetLauncherSection(),_T("MonthMESZ"),m_monthMESZ+1);
		GetProfile().WriteInt(theApp.GetLauncherSection(),_T("MonthMEZ"),m_monthMEZ + 7);

		SYSTEMTIME st;

		GetLocalTime(&st);

		if ( (st.wMonth>=m_monthMESZ+1) &&
			 (st.wMonth<=m_monthMEZ+7) )
		{
			GetProfile().WriteString(theApp.GetLauncherSection(),_T("Time"),_T("MESZ"));
		}
		else
		{
			GetProfile().WriteString(theApp.GetLauncherSection(),_T("Time"),_T("MEZ"));
		}
	}
	GetProfile().WriteInt(theApp.GetLauncherSection(),_T("DCFSync"),m_bDCF);
	GetProfile().WriteString(theApp.GetModuleSection(),_T("WinClk"), m_bDCF ? m_sFunkUhr : _T(""));
	GetProfile().WriteString(theApp.GetModuleSection(),_T("RCClockDefault"), m_sFunkUhr);
}
/////////////////////////////////////////////////////////////////////////////
void CMESZPage::CancelChanges()
{
	m_bAutoMESZ = GetProfile().GetInt(theApp.GetLauncherSection(),_T("AutoMESZ"),TRUE);
	m_monthMESZ = GetProfile().GetInt(theApp.GetLauncherSection(),_T("MonthMESZ"),3)-1;
	m_monthMEZ = GetProfile().GetInt(theApp.GetLauncherSection(),_T("MonthMEZ"),10)-7;

	m_bDCF = GetProfile().GetInt(theApp.GetLauncherSection(),_T("DCFSync"),FALSE);

	m_sFunkUhr = GetProfile().GetString(theApp.GetModuleSection(),_T("RCClockDefault"), _T("WinClk.exe"));
	
	EnableDlgControls();
	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
// CMESZPage message handlers
/////////////////////////////////////////////////////////////////////////////
void CMESZPage::OnCheckMesz() 
{
	UpdateData();
	m_bDCF = FALSE;
	UpdateData(FALSE);
	SetModified(TRUE,FALSE);
	EnableDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
void CMESZPage::OnSelchangeMonthMEZ() 
{
	SetModified(TRUE,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CMESZPage::OnSelchangeMonthMESZ() 
{
	SetModified(TRUE,FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CMESZPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();

	CString sFill;
	sFill.LoadString(IDS_JAN);
	m_cbMonthMESZ.AddString(sFill);
	sFill.LoadString(IDS_FEB);
	m_cbMonthMESZ.AddString(sFill);
	sFill.LoadString(IDS_MAR);
	m_cbMonthMESZ.AddString(sFill);
	sFill.LoadString(IDS_APR);
	m_cbMonthMESZ.AddString(sFill);
	sFill.LoadString(IDS_MAY);
	m_cbMonthMESZ.AddString(sFill);
	sFill.LoadString(IDS_JUN);
	m_cbMonthMESZ.AddString(sFill);
	sFill.LoadString(IDS_JUL);
	m_cbMonthMEZ.AddString(sFill);
	sFill.LoadString(IDS_AUG);
	m_cbMonthMEZ.AddString(sFill);
	sFill.LoadString(IDS_SEP);
	m_cbMonthMEZ.AddString(sFill);
	sFill.LoadString(IDS_OCT);
	m_cbMonthMEZ.AddString(sFill);
	sFill.LoadString(IDS_NOV);
	m_cbMonthMEZ.AddString(sFill);
	sFill.LoadString(IDS_DEC);
	m_cbMonthMEZ.AddString(sFill);

	SetOEMSpecificControls();
	CancelChanges();
	EnableDlgControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CMESZPage::OnCheckDcf() 
{
	UpdateData();
	m_bAutoMESZ = FALSE;
	m_sFunkUhr = GetProfile().GetString(theApp.GetModuleSection(),_T("RCClockDefault"), _T("WinClk.exe"));
	UpdateData(FALSE);
	SetModified(TRUE,FALSE);
	EnableDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
void CMESZPage::OnChangeEditFunkuhr() 
{
	UpdateData(TRUE);
	SetModified(TRUE,FALSE);
}
