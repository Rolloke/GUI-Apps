// LogOnOffSettings.cpp : implementation file
//

#include "stdafx.h"
#include "VideteTweak.h"
#include "LogOnOffSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define LOGON_SAVED	_T("SavedLongon")
/////////////////////////////////////////////////////////////////////////////
// CLogOnOffSettings property page

IMPLEMENT_DYNCREATE(CLogOnOffSettings, CPropertyPage)

CLogOnOffSettings::CLogOnOffSettings() : CPropertyPage(CLogOnOffSettings::IDD)
{
	//{{AFX_DATA_INIT(CLogOnOffSettings)
	m_bAutoAdminLogon		= FALSE;
	m_bDonDisplayLastUserName = FALSE;
	m_bDeleteRoamingCache = FALSE;
	m_nCachedLogonsCount = 0;
	m_bPowerDownAfterShutdown = FALSE;
	m_nSlowLinkTimeOut = 0;
	m_bSlowLinkDetectEnabled = FALSE;
	m_bNoPopUpsOnBoot = FALSE;
	//}}AFX_DATA_INIT
	m_strDefaultDomain.Empty();
	m_strDefaultPassword.Empty();
	m_strDefaultUserName.Empty();
	m_bChanged = false;
}

CLogOnOffSettings::~CLogOnOffSettings()
{
}

void CLogOnOffSettings::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogOnOffSettings)
	DDX_Check(pDX, IDC_CK_AUTOLOGON, m_bAutoAdminLogon);
	DDX_Text(pDX, IDC_EDT_DEFAULT_DOMAIN, m_strDefaultDomain);
	DDX_Text(pDX, IDC_EDT_DEFAULT_PASSWORD, m_strDefaultPassword);
	DDX_Text(pDX, IDC_EDT_DEFAULT_USERNAME, m_strDefaultUserName);
	DDX_Check(pDX, IDC_CK_DONT_DISPLAY_LAST_USERNAME, m_bDonDisplayLastUserName);
	DDX_Check(pDX, IDC_CK_DELETE_ROAMING_CACHE, m_bDeleteRoamingCache);
	DDX_Text(pDX, IDC_EDT_CACHED_LOGONS_COUNT, m_nCachedLogonsCount);
	DDV_MinMaxInt(pDX, m_nCachedLogonsCount, 0, 50);
	DDX_Check(pDX, IDC_CK_POWER_DOWN_AFTER_SHUTDOWN, m_bPowerDownAfterShutdown);
	DDX_Check(pDX, IDC_CK_ENABLE_SLOW_LINK_DETECTION, m_bSlowLinkDetectEnabled);
	DDX_Check(pDX, IDC_CK_NO_POPUPS_ON_BOOT, m_bNoPopUpsOnBoot);
	//}}AFX_DATA_MAP

	DDX_Text(pDX, IDC_EDT_SLOW_LINK_TIMEOUT, m_nSlowLinkTimeOut);
	if (m_nSlowLinkTimeOut)
	{
		DDV_MinMaxInt(pDX, m_nSlowLinkTimeOut, 1, 120);
	}
}


BEGIN_MESSAGE_MAP(CLogOnOffSettings, CPropertyPage)
	//{{AFX_MSG_MAP(CLogOnOffSettings)
	ON_EN_CHANGE(IDC_EDT_DEFAULT_DOMAIN, OnChange)
	ON_BN_CLICKED(IDC_BTN_RESTORE2, OnBtnRestore2)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave2)
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_EN_CHANGE(IDC_EDT_DEFAULT_PASSWORD, OnChange)
	ON_EN_CHANGE(IDC_EDT_DEFAULT_USERNAME, OnChange)
	ON_BN_CLICKED(IDC_CK_AUTOLOGON, OnChange)
	ON_BN_CLICKED(IDC_CK_DONT_DISPLAY_LAST_USERNAME, OnChange)
	ON_BN_CLICKED(IDC_CK_DELETE_ROAMING_CACHE, OnChange)
	ON_EN_CHANGE(IDC_EDT_CACHED_LOGONS_COUNT, OnChange)
	ON_BN_CLICKED(IDC_CK_POWER_DOWN_AFTER_SHUTDOWN, OnChange)
	ON_EN_CHANGE(IDC_EDT_SLOW_LINK_TIMEOUT, OnChange)
	ON_BN_CLICKED(IDC_CK_ENABLE_SLOW_LINK_DETECTION, OnChange)
	ON_BN_CLICKED(IDC_CK_NO_POPUPS_ON_BOOT, OnChange)
	ON_BN_CLICKED(IDC_BTN_SET, OnBtnImageSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogOnOffSettings message handlers

void CLogOnOffSettings::OnChange() 
{
	SetModified();	
	m_bChanged = true;
}

BOOL CLogOnOffSettings::OnInitDialog() 
{
	HKEY  hSecKey = NULL;
	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_WINLOGON, KEY_READ);
	if (hSecKey)
	{
		m_bAutoAdminLogon			= theApp.RegReadInt(   hSecKey, AUTOADMINLOGON, 0);
		m_bDonDisplayLastUserName	= theApp.RegReadInt(   hSecKey, DONTDISPLAYLASTUSERNAME, 0);
		m_bPowerDownAfterShutdown	= theApp.RegReadInt(   hSecKey, POWERDOWNAFTERSHUTDOWN, 0);
		m_strDefaultDomain			= theApp.RegReadString(hSecKey, DEFAULTDOMAINNAME, NULL);
		m_strDefaultUserName		= theApp.RegReadString(hSecKey, DEFAULTUSERNAME, NULL);
		m_strDefaultPassword		= theApp.RegReadString(hSecKey, DEFAULTPASSWORD, NULL);
		m_bDeleteRoamingCache		= theApp.RegReadInt(   hSecKey, DELETEROAMINGCACHE, 0);
		m_bSlowLinkDetectEnabled	= theApp.RegReadInt(   hSecKey, SLOWLINKDETECTENABLED, 0) ? TRUE : FALSE;
		m_nSlowLinkTimeOut			= theApp.RegReadInt(   hSecKey, SLOWLINKTIMEOUT, 0) / 1000;
		m_nCachedLogonsCount		= theApp.RegReadInt(   hSecKey, CACHEDLOGONSCOUNT, 0);
	}
	REG_CLOSE_KEY(hSecKey);

	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_CONTROL_WINDOWS, KEY_READ);
	if (hSecKey)
	{
		m_bNoPopUpsOnBoot =  theApp.RegReadInt(hSecKey, NOPOPUPSONBOOT, 0);
	}
	
	REG_CLOSE_KEY(hSecKey);

	CPropertyPage::OnInitDialog();

	int nTest = theApp.GetProfileInt(LOGON_SAVED, AUTOADMINLOGON, -1);
	if (nTest==-1)
	{
		OnBtnSave2();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CLogOnOffSettings::OnApply() 
{
	if (m_bChanged)
	{
		HKEY  hSecKey = NULL;

		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_WINLOGON, KEY_WRITE|KEY_READ);
		if (hSecKey)
		{
			theApp.RegWriteStringInt(hSecKey, AUTOADMINLOGON, m_bAutoAdminLogon);
			
			if (m_bDonDisplayLastUserName)
			{
				RegSetValueEx(hSecKey, DONTDISPLAYLASTUSERNAME, NULL, REG_SZ, (BYTE*)_T("1"), 2);
			}
			else
			{
				RegDeleteValue(hSecKey, DONTDISPLAYLASTUSERNAME);
			}

			theApp.RegWriteStringInt(hSecKey, POWERDOWNAFTERSHUTDOWN, m_bPowerDownAfterShutdown);

			if (m_bDeleteRoamingCache)
			{
				theApp.RegWriteInt(hSecKey, DELETEROAMINGCACHE, m_bDeleteRoamingCache);
			}
			else
			{
				RegDeleteValue(hSecKey, DELETEROAMINGCACHE);
			}
			
			if (m_bSlowLinkDetectEnabled)
			{
				theApp.RegWriteInt(hSecKey, SLOWLINKDETECTENABLED, m_bSlowLinkDetectEnabled);
			}
			else
			{
				RegDeleteValue(hSecKey, SLOWLINKDETECTENABLED);
			}
			
			if (m_nSlowLinkTimeOut)
			{
				theApp.RegWriteInt(hSecKey, SLOWLINKTIMEOUT, m_nSlowLinkTimeOut * 1000);
			}
			else
			{
				RegDeleteValue(hSecKey, SLOWLINKTIMEOUT);
			}

			if (!m_strDefaultDomain.IsEmpty())
			{
				theApp.RegWriteString(hSecKey, DEFAULTDOMAINNAME, m_strDefaultDomain);
			}
			else
			{
				RegDeleteValue(hSecKey, DEFAULTDOMAINNAME);
			}
			if (!m_strDefaultUserName.IsEmpty())
			{
				theApp.RegWriteString(hSecKey, DEFAULTUSERNAME, m_strDefaultUserName);
			}
			else
			{
				RegDeleteValue(hSecKey, DEFAULTUSERNAME);
			}

			if (!m_strDefaultPassword.IsEmpty())
			{
				theApp.RegWriteString(hSecKey, DEFAULTPASSWORD, m_strDefaultPassword);
			}
			else
			{
				RegDeleteValue(hSecKey, DEFAULTPASSWORD);
			}

			theApp.RegWriteStringInt(hSecKey, CACHEDLOGONSCOUNT, m_nCachedLogonsCount);
		}
		REG_CLOSE_KEY(hSecKey);
		
		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_CONTROL_WINDOWS, KEY_WRITE|KEY_READ);
		if (hSecKey)
		{
			if (m_bNoPopUpsOnBoot)
			{
				theApp.RegWriteInt(hSecKey, NOPOPUPSONBOOT, m_bNoPopUpsOnBoot);
			}
			else
			{
				RegDeleteValue(hSecKey, NOPOPUPSONBOOT);
			}
		}
		REG_CLOSE_KEY(hSecKey);
		
		SetModified(FALSE);
		m_bChanged = false;
	}
	return CPropertyPage::OnApply();
}

void CLogOnOffSettings::OnBtnRestore2() 
{
	m_bAutoAdminLogon = theApp.GetProfileInt(LOGON_SAVED, AUTOADMINLOGON, 0);
	m_bDonDisplayLastUserName = theApp.GetProfileInt(LOGON_SAVED, DONTDISPLAYLASTUSERNAME, 0);
	m_bPowerDownAfterShutdown = theApp.GetProfileInt(LOGON_SAVED, POWERDOWNAFTERSHUTDOWN, 0);
	m_bDeleteRoamingCache = theApp.GetProfileInt(LOGON_SAVED, DELETEROAMINGCACHE, 0);
	m_bSlowLinkDetectEnabled = theApp.GetProfileInt(LOGON_SAVED, SLOWLINKDETECTENABLED, 0);
	m_nSlowLinkTimeOut = theApp.GetProfileInt(LOGON_SAVED, SLOWLINKTIMEOUT, 0);

	m_strDefaultDomain = theApp.GetProfileString(LOGON_SAVED, DEFAULTDOMAINNAME, NULL);
	m_strDefaultUserName = theApp.GetProfileString(LOGON_SAVED, DEFAULTUSERNAME, NULL);
	m_strDefaultPassword = theApp.GetProfileString(LOGON_SAVED, DEFAULTPASSWORD, NULL);

	m_nCachedLogonsCount = theApp.GetProfileInt(LOGON_SAVED, CACHEDLOGONSCOUNT, 0);
	m_bNoPopUpsOnBoot = theApp.GetProfileInt(LOGON_SAVED, NOPOPUPSONBOOT, 0);
	UpdateData(FALSE);
	OnChange();
}

void CLogOnOffSettings::OnBtnSave2() 
{
	if (UpdateData())
	{
		theApp.WriteProfileInt(LOGON_SAVED, AUTOADMINLOGON, m_bAutoAdminLogon);
		theApp.WriteProfileInt(LOGON_SAVED, DONTDISPLAYLASTUSERNAME, m_bDonDisplayLastUserName);
		theApp.WriteProfileInt(LOGON_SAVED, POWERDOWNAFTERSHUTDOWN, m_bPowerDownAfterShutdown);
		theApp.WriteProfileInt(LOGON_SAVED, DELETEROAMINGCACHE, m_bDeleteRoamingCache);
		theApp.WriteProfileInt(LOGON_SAVED, SLOWLINKDETECTENABLED, m_bSlowLinkDetectEnabled);
		theApp.WriteProfileInt(LOGON_SAVED, SLOWLINKTIMEOUT, m_nSlowLinkTimeOut);

		theApp.WriteProfileString(LOGON_SAVED, DEFAULTDOMAINNAME, m_strDefaultDomain);
		theApp.WriteProfileString(LOGON_SAVED, DEFAULTUSERNAME, m_strDefaultUserName);
		theApp.WriteProfileString(LOGON_SAVED, DEFAULTPASSWORD, m_strDefaultPassword);

		theApp.WriteProfileInt(LOGON_SAVED, CACHEDLOGONSCOUNT, m_nCachedLogonsCount);
		theApp.WriteProfileInt(LOGON_SAVED, NOPOPUPSONBOOT, m_bNoPopUpsOnBoot);
	}
}

LRESULT CLogOnOffSettings::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;
	CString str;
	switch (pHI->iCtrlId)
	{
		case IDC_CK_AUTOLOGON: str.Format(_T("HKLM\\%s:\n%s (REG_DWORD)"), HKLM_WINLOGON, AUTOADMINLOGON); break;
		case IDC_CK_DONT_DISPLAY_LAST_USERNAME: str.Format(_T("HKLM\\%s:\n%s (REG_DWORD)"), HKLM_WINLOGON, DONTDISPLAYLASTUSERNAME); break;
		case IDC_CK_POWER_DOWN_AFTER_SHUTDOWN: str.Format(_T("HKLM\\%s:\n%s (REG_DWORD)"), HKLM_WINLOGON, POWERDOWNAFTERSHUTDOWN); break;
		case IDC_CK_DELETE_ROAMING_CACHE: str.Format(_T("HKLM\\%s:\n%s (REG_DWORD)"), HKLM_WINLOGON, DELETEROAMINGCACHE); break;
		case IDC_CK_ENABLE_SLOW_LINK_DETECTION: str.Format(_T("HKLM\\%s:\n%s (REG_DWORD)"), HKLM_WINLOGON, SLOWLINKDETECTENABLED); break;
		case IDC_EDT_SLOW_LINK_TIMEOUT: str.Format(_T("HKLM\\%s:\n%s (REG_DWORD)"), HKLM_WINLOGON, SLOWLINKTIMEOUT); break;
		case IDC_EDT_DEFAULT_DOMAIN: str.Format(_T("HKLM\\%s:\n%s (REG_DWORD)"), HKLM_WINLOGON, DEFAULTDOMAINNAME); break;
		case IDC_EDT_DEFAULT_USERNAME: str.Format(_T("HKLM\\%s:\n%s (REG_DWORD)"), HKLM_WINLOGON, DEFAULTUSERNAME); break;
		case IDC_EDT_DEFAULT_PASSWORD: str.Format(_T("HKLM\\%s:\n%s (REG_DWORD)"), HKLM_WINLOGON, DEFAULTPASSWORD); break;
		case IDC_EDT_CACHED_LOGONS_COUNT: str.Format(_T("HKLM\\%s:\n%s (REG_DWORD)"), HKLM_WINLOGON, CACHEDLOGONSCOUNT); break;
		case IDC_CK_NO_POPUPS_ON_BOOT: str.Format(_T("HKLM\\%s:\n%s (REG_DWORD)"), HKLM_CONTROL_WINDOWS, NOPOPUPSONBOOT); break;
		default:break;
	}
	if (!str.IsEmpty())
	{
		AfxMessageBox(str);
	}
	return 1;
}

void CLogOnOffSettings::OnBtnImageSettings() 
{
	m_bPowerDownAfterShutdown = FALSE;
	m_bNoPopUpsOnBoot = FALSE;
	UpdateData(FALSE);
	OnChange();
}
