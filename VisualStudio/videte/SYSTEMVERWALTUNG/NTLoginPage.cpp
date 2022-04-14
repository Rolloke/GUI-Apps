// NTLoginPage.cpp : implementation file
//

#include "stdafx.h"
#include "systemverwaltung.h"
#include "NTLoginPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNTLoginPage dialog


CNTLoginPage::CNTLoginPage(CSVView* pParent) : CSVPage(CNTLoginPage::IDD)
{
	m_pParent = pParent;
	//{{AFX_DATA_INIT(CNTLoginPage)
	m_sDomain = _T("");
	m_sPassword = _T("");
	m_sUser = _T("");
	m_bAutoLogin = FALSE;
	//}}AFX_DATA_INIT
	Create(IDD,(CWnd*)pParent);
}


void CNTLoginPage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNTLoginPage)
	DDX_Control(pDX, IDC_EDIT_USER, m_editUser);
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_editPassword);
	DDX_Control(pDX, IDC_EDIT_DOMAIN, m_editDomain);
	DDX_Text(pDX, IDC_EDIT_DOMAIN, m_sDomain);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_sPassword);
	DDX_Text(pDX, IDC_EDIT_USER, m_sUser);
	DDX_Check(pDX, IDC_CHECK_LOGIN, m_bAutoLogin);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNTLoginPage, CSVPage)
	//{{AFX_MSG_MAP(CNTLoginPage)
	ON_BN_CLICKED(IDC_CHECK_LOGIN, OnCheckOn)
	ON_EN_CHANGE(IDC_EDIT_DOMAIN, OnChangeEditDomain)
	ON_EN_CHANGE(IDC_EDIT_PASSWORD, OnChangeEditPassword)
	ON_EN_CHANGE(IDC_EDIT_USER, OnChangeEditUser)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNTLoginPage message handlers
/////////////////////////////////////////////////////////////////////////////
void CNTLoginPage::EnableDlgControls()
{
	m_editUser.EnableWindow(m_bAutoLogin);
	m_editDomain.EnableWindow(m_bAutoLogin);
	m_editPassword.EnableWindow(m_bAutoLogin);
}
/////////////////////////////////////////////////////////////////////////////
BOOL CNTLoginPage::IsDataValid()
{
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
void CNTLoginPage::SaveChanges()
{
	WK_TRACE_USER(_T("Automatisches NTLogin-Einstellungen wurden geändert\n"));
	if (m_hWnd)
	{
		UpdateData();
	}

	if (theApp.IsNT())
	{
		CWK_Profile wkProfile(CWK_Profile::WKP_NTLOGON,NULL, REGKEY_LOGON_NT,_T(""));
		wkProfile.WriteString(REGKEY_WINLOGON, REGKEY_AUTOADMINLOGON,m_bAutoLogin ? _T("1") : _T("0"));
		if (m_bAutoLogin)
		{
			wkProfile.WriteString(REGKEY_WINLOGON,REGKEY_DEFAULTUSERNAME,m_sUser);
			wkProfile.WriteString(REGKEY_WINLOGON,REGKEY_DEFAULTDOMAINNAME,m_sDomain);
			wkProfile.WriteString(REGKEY_WINLOGON,REGKEY_DEFAULTPASSWORD,m_sPassword);
		}
	}
	else
	{
		CWK_Profile wkProfile(CWK_Profile::WKP_NTLOGON,NULL, REGKEY_LOGON_95,_T(""));
		wkProfile.WriteString(REGKEY_WINLOGON, REGKEY_AUTOADMINLOGON,m_bAutoLogin ? _T("1") : NULL);
		if (m_bAutoLogin)
		{
			wkProfile.WriteString(REGKEY_WINLOGON,REGKEY_DEFAULTUSERNAME,m_sUser);
			wkProfile.WriteString(REGKEY_WINLOGON,REGKEY_DEFAULTPASSWORD,m_sPassword);
			wkProfile.WriteString(REGKEY_WINLOGON,REGKEY_DONTDISPLAYLASTUSERNAME,_T("0"));
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CNTLoginPage::CancelChanges()
{
	CString sSection;
	if (theApp.IsNT())
	{
		sSection = REGKEY_LOGON_NT;
	}
	else
	{
		sSection = REGKEY_LOGON_95;
	}
	CWK_Profile wkProfile(CWK_Profile::WKP_NTLOGON,NULL,sSection,_T(""));

	m_bAutoLogin = wkProfile.GetString(REGKEY_WINLOGON, REGKEY_AUTOADMINLOGON   ,_T("0")) == _T("1");
	m_sUser      = wkProfile.GetString(REGKEY_WINLOGON, REGKEY_DEFAULTUSERNAME  ,_T(""));
	m_sDomain    = wkProfile.GetString(REGKEY_WINLOGON, REGKEY_DEFAULTDOMAINNAME,_T(""));
	m_sPassword  = wkProfile.GetString(REGKEY_WINLOGON, REGKEY_DEFAULTPASSWORD  ,_T(""));
	
	if (m_hWnd)
	{
		UpdateData(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CNTLoginPage::OnCheckOn() 
{
	UpdateData();
	SetModified(TRUE,FALSE);
	EnableDlgControls();
}
/////////////////////////////////////////////////////////////////////////////
BOOL CNTLoginPage::OnInitDialog() 
{
	CSVPage::OnInitDialog();
	
	CancelChanges();
	EnableDlgControls();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CNTLoginPage::OnChangeEditDomain() 
{
	SetModified(TRUE,FALSE);
}

void CNTLoginPage::OnChangeEditPassword() 
{
	SetModified(TRUE,FALSE);
}

void CNTLoginPage::OnChangeEditUser() 
{
	SetModified(TRUE,FALSE);
}
