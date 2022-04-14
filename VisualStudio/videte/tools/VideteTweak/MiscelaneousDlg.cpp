// MiscelaneousDlg.cpp : implementation file
//

#include "stdafx.h"
#include "videtetweak.h"
#include "MiscelaneousDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMiscelaneousDlg property page
#define MISCELANEOUS_SAVED "SavedMiscelaneous"

/*
Dr. Watson abschalten
[HKEY_Local_Maschine\Software\Microsoft\WindowsNT\CurrentVersion\AEDebug]
 */

IMPLEMENT_DYNCREATE(CMiscelaneousDlg, CPropertyPage)

CMiscelaneousDlg::CMiscelaneousDlg() : CPropertyPage(CMiscelaneousDlg::IDD)
{
	//{{AFX_DATA_INIT(CMiscelaneousDlg)
	m_nDriverSigning = -1;
	m_nMenuShowDelay = 0;
	m_bDevMgrShowDetails = FALSE;
	m_bNoInteractiveServices = FALSE;
	m_strRegisteredOrganisation = _T("");
	m_strRegisteredOwner = _T("");
	m_bRegDone = FALSE;
	m_strProductID = _T("");
	m_strLangID = _T("");
	//}}AFX_DATA_INIT
	m_bChanged = false;
}

CMiscelaneousDlg::~CMiscelaneousDlg()
{
}

void CMiscelaneousDlg::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMiscelaneousDlg)
	DDX_Control(pDX, IDC_COMBO_LANGUAGES, m_cLanguage);
	DDX_Radio(pDX, IDC_RD_DRIVER_SIGNING, m_nDriverSigning);
	DDX_Text(pDX, IDC_EDT_MENU_SHOW_DELAY, m_nMenuShowDelay);
	DDV_MinMaxInt(pDX, m_nMenuShowDelay, 0, 2000);
	DDX_Check(pDX, IDC_CK_DEVMGR_SHOW_DETAILS, m_bDevMgrShowDetails);
	DDX_Check(pDX, IDC_CK_NO_INTERACTIVE_SERVICES, m_bNoInteractiveServices);
	DDX_Text(pDX, IDC_EDT_REGISTERED_ORG, m_strRegisteredOrganisation);
	DDX_Text(pDX, IDC_EDT_REGISTERED_OWNER, m_strRegisteredOwner);
	DDX_Check(pDX, IDC_CK_REGISTERED, m_bRegDone);
	DDX_Text(pDX, IDC_EDT_PRODUCT_ID, m_strProductID);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMiscelaneousDlg, CPropertyPage)
	//{{AFX_MSG_MAP(CMiscelaneousDlg)
	ON_BN_CLICKED(IDC_BTN_SAVE, OnBtnSave)
	ON_BN_CLICKED(IDC_BTN_RESTORE, OnBtnRestore)
	ON_BN_CLICKED(IDC_RD_DRIVER_SIGNING, OnChange)
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_CBN_SELCHANGE(IDC_COMBO_LANGUAGES, OnSelchangeComboLanguages)
	ON_BN_CLICKED(IDC_RD_DRIVER_SIGNING2, OnChange)
	ON_BN_CLICKED(IDC_RD_DRIVER_SIGNING3, OnChange)
	ON_EN_CHANGE(IDC_EDT_MENU_SHOW_DELAY, OnChange)
	ON_BN_CLICKED(IDC_CK_NO_INTERACTIVE_SERVICES, OnChange)
	ON_EN_CHANGE(IDC_EDT_REGISTERED_ORG, OnChange)
	ON_EN_CHANGE(IDC_EDT_REGISTERED_OWNER, OnChange)
	ON_BN_CLICKED(IDC_CK_REGISTERED, OnChange)
	ON_EN_CHANGE(IDC_EDT_PRODUCT_ID, OnChange)
	ON_CBN_EDITCHANGE(IDC_COMBO_LANGUAGES, OnChange)
	ON_BN_CLICKED(IDC_BTN_SET, OnBtnSet)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiscelaneousDlg message handlers

void CMiscelaneousDlg::OnBtnSave() 
{
	if (UpdateData())
	{
		theApp.WriteProfileInt(MISCELANEOUS_SAVED, POLICY, m_nDriverSigning);
		theApp.WriteProfileInt(MISCELANEOUS_SAVED, MENU_SHOW_DELAY, m_nMenuShowDelay);
		theApp.WriteProfileInt(MISCELANEOUS_SAVED, SHOW_DETAILS_DEVMGR, m_bDevMgrShowDetails);
		theApp.WriteProfileInt(MISCELANEOUS_SAVED, NOINTERACTIVESERVICES, m_bNoInteractiveServices);
		theApp.WriteProfileString(MISCELANEOUS_SAVED, REGISTEREDORGANIZATION, m_strRegisteredOrganisation);
		theApp.WriteProfileString(MISCELANEOUS_SAVED, REGISTEREDOWNER, m_strRegisteredOwner);
		theApp.WriteProfileString(MISCELANEOUS_SAVED, PRODUCTID, m_strProductID);
		theApp.WriteProfileInt(MISCELANEOUS_SAVED, REGDONE, m_bRegDone);
	}
}

void CMiscelaneousDlg::OnBtnRestore() 
{
	m_nDriverSigning            = theApp.GetProfileInt(MISCELANEOUS_SAVED, POLICY, 1);
	m_nMenuShowDelay            = theApp.GetProfileInt(MISCELANEOUS_SAVED, MENU_SHOW_DELAY, 400);
	m_bDevMgrShowDetails        = theApp.GetProfileInt(MISCELANEOUS_SAVED, SHOW_DETAILS_DEVMGR, m_bDevMgrShowDetails);
	m_bNoInteractiveServices    = theApp.GetProfileInt(MISCELANEOUS_SAVED, NOINTERACTIVESERVICES, m_bNoInteractiveServices);
	m_strRegisteredOrganisation = theApp.GetProfileString(MISCELANEOUS_SAVED, REGISTEREDORGANIZATION, NULL);
	m_strRegisteredOwner        = theApp.GetProfileString(MISCELANEOUS_SAVED, REGISTEREDOWNER, NULL);
	m_strProductID              = theApp.GetProfileString(MISCELANEOUS_SAVED, PRODUCTID, NULL);
	m_bRegDone                  = theApp.GetProfileInt(MISCELANEOUS_SAVED, REGDONE, NULL);
	UpdateData(FALSE);
	OnChange();
}

void CMiscelaneousDlg::OnChange() 
{
	SetModified();	
	m_bChanged = true;
}

BOOL CMiscelaneousDlg::OnApply() 
{
	if (m_bChanged)
	{
		HKEY  hSecKey = NULL;

		hSecKey = theApp.RegOpenKey(HKEY_CURRENT_USER, HKCU_DRIVERSIGNING, KEY_READ|KEY_WRITE);
		if (hSecKey)
		{
			theApp.RegWriteInt(hSecKey, POLICY, m_nDriverSigning);
		}	
		REG_CLOSE_KEY(hSecKey);

		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_CONTROL_WINDOWS, KEY_READ|KEY_WRITE);
		if (hSecKey)
		{
			theApp.RegWriteInt(hSecKey, NOINTERACTIVESERVICES, m_bNoInteractiveServices);
		}	
		REG_CLOSE_KEY(hSecKey);

		hSecKey = theApp.RegOpenKey(HKEY_CURRENT_USER, HKCU_DESKTOP, KEY_READ|KEY_WRITE);
		if (hSecKey)
		{
			theApp.RegWriteStringInt(hSecKey, MENU_SHOW_DELAY, m_nMenuShowDelay);
		}	
		REG_CLOSE_KEY(hSecKey);

		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_SESSION_MANAGER, KEY_READ|KEY_WRITE);
		if (hSecKey)
		{
			theApp.RegWriteStringInt(hSecKey, SHOW_DETAILS_DEVMGR, m_bDevMgrShowDetails);
		}	
		REG_CLOSE_KEY(hSecKey);
		
		hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_CURRENTVERSION, KEY_READ|KEY_WRITE);
		if (hSecKey)
		{
			theApp.RegWriteString(hSecKey, REGISTEREDORGANIZATION, m_strRegisteredOrganisation);
			theApp.RegWriteString(hSecKey, REGISTEREDOWNER, m_strRegisteredOwner);
			theApp.RegWriteString(hSecKey, PRODUCTID, m_strProductID);
			theApp.RegWriteStringInt(hSecKey, REGDONE, m_bRegDone);
		}	
		REG_CLOSE_KEY(hSecKey);

		if (m_cLanguage.GetCount() > 1)
		{
			hSecKey = theApp.RegOpenKey(HKEY_CURRENT_USER, HKCU_DESKTOP, KEY_READ|KEY_WRITE);
			if (hSecKey)
			{
				theApp.RegWriteString(hSecKey, MUILANGUAGEPENDING, m_strLangID);
				theApp.RegWriteString(hSecKey, MULTIUILANGUAGEID, m_strLangID);
			}
			REG_CLOSE_KEY(hSecKey);
			
			CString str;
			str.Format("%s\\%s", HKU_DEFAULT, HKCU_DESKTOP);
			hSecKey = theApp.RegOpenKey(HKEY_USERS, str, KEY_READ|KEY_WRITE);
			if (hSecKey)
			{
				theApp.RegWriteString(hSecKey, MUILANGUAGEPENDING, m_strLangID);
				theApp.RegWriteString(hSecKey, MULTIUILANGUAGEID, m_strLangID);
			}
			REG_CLOSE_KEY(hSecKey);
		}
		SetModified(FALSE);
		m_bChanged = false;
	}
	return CPropertyPage::OnApply();
}

BOOL CALLBACK EnumUILanguagesProc(LPTSTR lpUILanguageString, LONG* lParam)
{
	SendMessage((HWND)lParam, CB_ADDSTRING, 0, (LPARAM)lpUILanguageString);
	return 0;
}

BOOL CMiscelaneousDlg::OnInitDialog() 
{
	HKEY  hSecKey = NULL;
	hSecKey = theApp.RegOpenKey(HKEY_CURRENT_USER, HKCU_DRIVERSIGNING, KEY_READ);

	if (hSecKey)
	{
		m_nDriverSigning = theApp.RegReadInt(hSecKey, POLICY, 0);
	}
	REG_CLOSE_KEY(hSecKey);

	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_CONTROL_WINDOWS, KEY_READ);
	if (hSecKey)
	{
		m_bNoInteractiveServices = theApp.RegReadInt(hSecKey, NOINTERACTIVESERVICES, 0);
	}
	REG_CLOSE_KEY(hSecKey);
	
	hSecKey = theApp.RegOpenKey(HKEY_CURRENT_USER, HKCU_DESKTOP, KEY_READ);
	if (hSecKey)
	{
		m_nMenuShowDelay = theApp.RegReadInt(hSecKey, MENU_SHOW_DELAY, 100);
	}
	REG_CLOSE_KEY(hSecKey);

	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_SESSION_MANAGER, KEY_READ);
	if (hSecKey)
	{
		m_bDevMgrShowDetails = theApp.RegReadInt(hSecKey, SHOW_DETAILS_DEVMGR, 0);
	}
	REG_CLOSE_KEY(hSecKey);

	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_CURRENTVERSION, KEY_READ);
	if (hSecKey)
	{
		m_strRegisteredOrganisation = theApp.RegReadString(hSecKey, REGISTEREDORGANIZATION, NULL);
		m_strRegisteredOwner		= theApp.RegReadString(hSecKey, REGISTEREDOWNER, NULL);
		m_strProductID				= theApp.RegReadString(hSecKey, PRODUCTID, NULL);
		m_bRegDone					= theApp.RegReadInt(   hSecKey, REGDONE, 0);
	}
	REG_CLOSE_KEY(hSecKey);

	hSecKey = theApp.RegOpenKey(HKEY_CURRENT_USER, HKCU_DESKTOP, KEY_READ);
	if (hSecKey)
	{
		m_strLangID = theApp.RegReadString(hSecKey, MUILANGUAGEPENDING, NULL);
	}
	REG_CLOSE_KEY(hSecKey);
	
	CPropertyPage::OnInitDialog();

	int     i, nCount = m_cLanguage.GetCount()-1;
	hSecKey = theApp.RegOpenKey(HKEY_LOCAL_MACHINE, HKLM_MUILANGUAGES, KEY_READ);
	if (hSecKey)
	{
		DWORD   dwIndex = 0, dwValueNameSize, dwType, dwValueSize;
		char    szValueName[64];
		char    szValue[32];
		CString strText;
		LRESULT lResult = ERROR_SUCCESS;
		while (lResult == ERROR_SUCCESS)
		{
			dwValueNameSize = 63;
			dwValueSize     = 31;
			lResult = RegEnumValue(hSecKey, dwIndex++,
				(LPSTR)szValueName, &dwValueNameSize, 0, &dwType,
				(LPBYTE)szValue, &dwValueSize);
			if ((lResult == ERROR_SUCCESS) && (dwType == REG_SZ) && (szValue[0] == '1'))
			{
				strText  = _T("0000");
				strText += _T(szValueName);
				int nSel = m_cLanguage.FindString(0, strText);
				if (nSel != CB_ERR)
				{
					m_cLanguage.GetLBText(nSel, strText);
					m_cLanguage.AddString(strText);
				}
			}
		};
	}	
	REG_CLOSE_KEY(hSecKey);

	for (i=0; i<nCount; i++)
	{
		m_cLanguage.DeleteString(1);
	}
	
	int nSel = m_cLanguage.FindString(0, m_strLangID);
	if (nSel == CB_ERR) nSel = 0;
	m_cLanguage.SetCurSel(nSel);

	int nTest = theApp.GetProfileInt(MISCELANEOUS_SAVED, POLICY, -1);
	if (nTest==-1)
	{
		OnBtnSave();
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

LRESULT CMiscelaneousDlg::OnHelp(WPARAM /*wParam*/, LPARAM lParam)
{
   HELPINFO *pHI = (HELPINFO*) lParam;
	CString str;

	switch (pHI->iCtrlId)
	{
		case IDC_RD_DRIVER_SIGNING:
		case IDC_RD_DRIVER_SIGNING2:
		case IDC_RD_DRIVER_SIGNING3:			str.Format("HKEY_CURRENT_USER\\%s:\n%s (REG_DWORD)", HKCU_DRIVERSIGNING, POLICY); break;
		case IDC_EDT_MENU_SHOW_DELAY:			str.Format("HKEY_CURRENT_USER\\%s:\n%s (REG_SZ)", HKCU_DESKTOP, MENU_SHOW_DELAY); break;
		case IDC_CK_DEVMGR_SHOW_DETAILS:		str.Format("HKEY_LOCAL_MACHINE\\%s:\n%s (REG_SZ)", HKLM_SESSION_MANAGER, SHOW_DETAILS_DEVMGR); break;
		case IDC_CK_NO_INTERACTIVE_SERVICES:str.Format("HKEY_LOCAL_MACHINE\\%s:\n%s (REG_DWORD)", HKLM_CONTROL_WINDOWS, NOINTERACTIVESERVICES); break;
		case IDC_EDT_REGISTERED_ORG:			str.Format("HKEY_LOCAL_MACHINE\\%s:\n%s (REG_SZ)", HKLM_CURRENTVERSION, REGISTEREDORGANIZATION); break;
		case IDC_EDT_REGISTERED_OWNER:		str.Format("HKEY_LOCAL_MACHINE\\%s:\n%s (REG_SZ)", HKLM_CURRENTVERSION, REGISTEREDOWNER); break;
		case IDC_EDT_PRODUCT_ID:				str.Format("HKEY_LOCAL_MACHINE\\%s:\n%s (REG_SZ)", HKLM_CURRENTVERSION, PRODUCTID); break;
		case IDC_CK_REGISTERED:					str.Format("HKEY_LOCAL_MACHINE\\%s:\n%s (REG_SZ)", HKLM_CURRENTVERSION, REGDONE); break;
		default:break;
	}
	if (!str.IsEmpty())
	{
		AfxMessageBox(str);
	}
	return 1;
}

void CMiscelaneousDlg::OnSelchangeComboLanguages() 
{
	int nSel = m_cLanguage.GetCurSel();
	if (nSel != CB_ERR)
	{
		m_cLanguage.GetLBText(nSel, m_strLangID);
		m_strLangID = m_strLangID.Left(8);
		OnChange();
	}
}


void CMiscelaneousDlg::OnBtnSet() 
{
	m_nDriverSigning = 1;
	m_nMenuShowDelay = 200;
	m_bDevMgrShowDetails = TRUE;
	m_bNoInteractiveServices = TRUE;
	m_bRegDone = TRUE;
	UpdateData(FALSE);
}
