// SMSPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "SMSPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TCHAR BASED_CODE szSection[]		= _T("SMSUnit");
static TCHAR BASED_CODE szD1[]			= _T("D1");
static TCHAR BASED_CODE szD2[]			= _T("D2");
static TCHAR BASED_CODE szEPlus[]		= _T("EPlus");
static TCHAR BASED_CODE szPrefix[]		= _T("Prefix");
static TCHAR BASED_CODE szOwnNumber[]	= _T("OwnNumber");

/////////////////////////////////////////////////////////////////////////////
// CSMSPage dialog
CSMSPage::CSMSPage(CSVView* pParent) : CSVPage(CSMSPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CSMSPage)
	m_bSMS = FALSE;
	m_sD1 = _T("01712521001");
	m_sD2 = _T("01722278010");
	m_sPrefix = _T("");
	m_sEplus = _T("01771167");
	m_sOwnNumber = _T("");
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)pParent);
}
/////////////////////////////////////////////////////////////////////////////
void CSMSPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSMSPage)
	DDX_Control(pDX, IDC_EDIT_SMS_OWN_NUMBER, m_editOwnNumber);
	DDX_Control(pDX, IDC_EDIT_EPLUS, m_editEplus);
	DDX_Control(pDX, IDC_EDIT_PREFIX, m_editPrefix);
	DDX_Control(pDX, IDC_EDIT_D2, m_editD2);
	DDX_Control(pDX, IDC_EDIT_D1, m_editD1);
	DDX_Check(pDX, IDC_CHECK_SMS, m_bSMS);
	DDX_Text(pDX, IDC_EDIT_D1, m_sD1);
	DDX_Text(pDX, IDC_EDIT_D2, m_sD2);
	DDX_Text(pDX, IDC_EDIT_PREFIX, m_sPrefix);
	DDX_Text(pDX, IDC_EDIT_EPLUS, m_sEplus);
	DDX_Text(pDX, IDC_EDIT_SMS_OWN_NUMBER, m_sOwnNumber);
	DDV_MaxChars(pDX, m_sOwnNumber, 16);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CSMSPage, CSVPage)
	//{{AFX_MSG_MAP(CSMSPage)
	ON_BN_CLICKED(IDC_CHECK_SMS, OnCheckSms)
	ON_EN_CHANGE(IDC_EDIT_D1, OnChangeEditD1)
	ON_EN_CHANGE(IDC_EDIT_D2, OnChangeEditD2)
	ON_EN_CHANGE(IDC_EDIT_PREFIX, OnChangeEditPrefix)
	ON_EN_CHANGE(IDC_EDIT_EPLUS, OnChangeEditEplus)
	ON_EN_CHANGE(IDC_EDIT_SMS_OWN_NUMBER, OnChangeEditOwnNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
BOOL CSMSPage::IsDataValid()
{
	if (m_bSMS && m_sOwnNumber.IsEmpty())
	{
		AfxMessageBox(IDP_SMS_NO_OWN_NUMBER, MB_OK|MB_ICONSTOP);
		m_editOwnNumber.SetFocus();
		return FALSE;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CSMSPage::ShowHide()
{
	m_editOwnNumber.EnableWindow(m_bSMS);
	m_editPrefix.EnableWindow(m_bSMS);
	m_editD2.EnableWindow(m_bSMS);
	m_editD1.EnableWindow(m_bSMS);
	m_editEplus.EnableWindow(m_bSMS);
}
/////////////////////////////////////////////////////////////////////////////
void CSMSPage::OnChange() 
{
	UpdateData();
	SetModified(TRUE,FALSE);
	ShowHide();
	m_pParent->AddInitApp(WK_APPNAME_SMSUNIT);
}
/////////////////////////////////////////////////////////////////////////////
void CSMSPage::SaveChanges()
{
	WK_TRACE_USER(_T("SMS_Modul-Einstellungen wurden geändert\n"));
	UpdateData();

	GetProfile().WriteString(theApp.GetModuleSection(),WK_APPNAME_SMSUNIT,m_bSMS ? _T("SMSUnit.exe") : _T(""));
	GetProfile().WriteString(szSection, szOwnNumber, m_sOwnNumber);
	GetProfile().WriteString(szSection, szPrefix, m_sPrefix);
	GetProfile().WriteString(szSection, szD1, m_sD1);
	GetProfile().WriteString(szSection, szD2, m_sD2);
	GetProfile().WriteString(szSection, szEPlus, m_sEplus);
}
/////////////////////////////////////////////////////////////////////////////
void CSMSPage::CancelChanges()
{
	CString sTemp;
	sTemp = GetProfile().GetString(theApp.GetModuleSection(),WK_APPNAME_SMSUNIT,_T(""));
	m_bSMS = (sTemp.CompareNoCase(_T("SMSUnit.exe")) == 0); 
	m_sOwnNumber = GetProfile().GetString(szSection, szOwnNumber, m_sOwnNumber);
	m_sPrefix = GetProfile().GetString(szSection, szPrefix, m_sPrefix);
	m_sD1 = GetProfile().GetString(szSection, szD1, m_sD1);
	m_sD2 = GetProfile().GetString(szSection, szD2, m_sD2);
	m_sEplus = GetProfile().GetString(szSection, szEPlus, m_sEplus);

	UpdateData(FALSE);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CSMSPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	CancelChanges();

	ShowHide();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CSMSPage::OnCheckSms() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CSMSPage::OnChangeEditOwnNumber() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CSMSPage::OnChangeEditPrefix() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CSMSPage::OnChangeEditD1() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CSMSPage::OnChangeEditD2() 
{
	OnChange();
}
/////////////////////////////////////////////////////////////////////////////
void CSMSPage::OnChangeEditEplus() 
{
	OnChange();
}
