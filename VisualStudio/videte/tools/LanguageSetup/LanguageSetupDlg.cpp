// LanguageSetupDlg.cpp : implementation file
//

#include "stdafx.h"
#include "LanguageSetup.h"
#include "LanguageSetupDlg.h"

#define IsBetween(val, lowerval, upperval) ((val>=lowerval) && (val<= upperval))

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

struct {
	LPCTSTR sRegKey;
    LCTYPE  lcType;
} g_International[] = 
{
	{ _T("iCountry"), LOCALE_ICOUNTRY },
	{ _T("iCurrDigits"), LOCALE_ICURRDIGITS },
	{ _T("iCurrency"), LOCALE_ICURRENCY },
	{ _T("iDate"), LOCALE_IDATE },
	{ _T("iDigits"), LOCALE_IDIGITS },
	{ _T("iLZero"), LOCALE_ILZERO },
	{ _T("iMeasure"), LOCALE_IMEASURE },
	{ _T("iNegCurr"), LOCALE_INEGCURR },
	{ _T("iTime"), LOCALE_ITIME},
	{ _T("iTLZero"), LOCALE_ITLZERO },
	{ _T("iTimePrefix"), LOCALE_ITIMEMARKPOSN },
	{ _T("iNegNumber"), LOCALE_INEGNUMBER },
	{ _T("iCalendarType"), LOCALE_ICALENDARTYPE },
	{ _T("iFirstDayOfWeek"), LOCALE_IFIRSTDAYOFWEEK },
	{ _T("iFirstWeekOfYear"), LOCALE_IFIRSTWEEKOFYEAR },
	{ _T("s1159"), LOCALE_S1159},
	{ _T("s2359"), LOCALE_S2359},
	{ _T("sCountry"), LOCALE_SCOUNTRY },
	{ _T("sCurrency"), LOCALE_SCURRENCY},
	{ _T("sDate"), LOCALE_SDATE },
	{ _T("sDecimal"), LOCALE_SDECIMAL },
	{ _T("sLanguage"), LOCALE_SLANGUAGE},
	{ _T("sList"), LOCALE_SLIST },
	{ _T("sLongDate"), LOCALE_SLONGDATE},
	{ _T("sShortDate"), LOCALE_SSHORTDATE}, 
	{ _T("sThousand"), LOCALE_STHOUSAND },
	{ _T("sTime"), LOCALE_STIME },
	{ _T("sTimeFormat"), LOCALE_STIMEFORMAT },
	{ _T("sMonDecimalSep"), LOCALE_SMONDECIMALSEP },
	{ _T("sMonThousandSep"), LOCALE_SMONTHOUSANDSEP },
	{ _T("sNativeDigits"), LOCALE_SNATIVEDIGITS },
	{ _T("sGrouping"), LOCALE_SGROUPING },
	{ _T("sMonGrouping"), LOCALE_SMONGROUPING },
	{ _T("sPositiveSign"), LOCALE_SPOSITIVESIGN },
	{ _T("sNegativeSign"), LOCALE_SNEGATIVESIGN },
	{ NULL, 0}
};

#define HKCU_INTERNATIONAL_SECTION	_T("Control Panel\\International")
#define INTERNATIONAL_LOCALE	_T("Locale")
#define INTERNATIONAL_NUMSHAPE	_T("NumShape")

#define HKLM_NLS_CODEPAGE	_T("SYSTEM\\CurrentControlSet\\Control\\Nls\\CodePage")
#define OEM_CODEPAGE		_T("OEMCP")
#define ANSI_CODEPAGE		_T("ACP")

/////////////////////////////////////////////////////////////////////////////
// CLanguageSetupDlg dialog
CLanguageSetupDlg::CLanguageSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLanguageSetupDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLanguageSetupDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nSelection   = CB_ERR;
	m_nKBSelection = CB_ERR;
}
/////////////////////////////////////////////////////////////////////////////
void CLanguageSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLanguageSetupDlg)
	DDX_Control(pDX, IDC_COMBO_LANGUAGES2, m_cKBLanguage);
	DDX_Control(pDX, IDC_COMBO_LANGUAGES, m_cLanguage);
	//}}AFX_DATA_MAP
}
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CLanguageSetupDlg, CDialog)
	//{{AFX_MSG_MAP(CLanguageSetupDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_CBN_SELCHANGE(IDC_COMBO_LANGUAGES, OnSelchangeComboLanguages)
	ON_CBN_SELCHANGE(IDC_COMBO_LANGUAGES2, OnSelchangeComboKBLanguage)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLanguageSetupDlg message handlers
BOOL CLanguageSetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	HKEY  hSubKey, hSecKey = NULL, hRfc1766 = NULL;
	LONG  lResult;
	int     i, nCount = 1;
	CStringArray sArr;
	CString strText, strKey;
	DWORD   dwIndex, dwValueNameSize, dwType, dwValueSize;
	_TCHAR    szValueName[64];
	_TCHAR    szValue[32];
	
	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, HKCU_DESKTOP, 0, KEY_READ, &hSecKey);
	if (lResult == ERROR_SUCCESS)
	{
		ReadRegistryString(hSecKey, MUILANGUAGEPENDING, m_strLangID);
	}
	REG_CLOSE_KEY(hSecKey);

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER, HKCU_KEYBORD_LAYOUT, 0, KEY_READ, &hSecKey);
	if (lResult == ERROR_SUCCESS)
	{
		ReadRegistryString(hSecKey, "1", m_strKBLangID);
	}
	REG_CLOSE_KEY(hSecKey);
	

	lResult = RegOpenKeyEx(HKEY_CLASSES_ROOT, _T("Mime\\Database\\Rfc1766"), 0, KEY_READ, &hRfc1766);
	if (lResult != ERROR_SUCCESS)
	{
		hRfc1766 = NULL;
	}

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, HKLM_MUILANGUAGES, 0, KEY_READ, &hSecKey);
	m_cLanguage.GetLBText(0, strText);
	sArr.Add(strText);
	if (lResult == ERROR_SUCCESS)
	{
		dwIndex =  0;
		while (lResult == ERROR_SUCCESS)
		{
			dwValueNameSize = 63;
			dwValueSize     = 31;
			lResult = RegEnumValue(hSecKey, dwIndex++,
				(LPSTR)szValueName, &dwValueNameSize, 0, &dwType,
				(LPBYTE)szValue, &dwValueSize);
			if ((lResult == ERROR_SUCCESS) && (dwType == REG_SZ) && (szValue[0] == '1'))
			{
				CString str;
				strText.Format("0000%s", szValueName);
				_TCHAR szLCdata[128];
				LCID lcid = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
				_stscanf(strText, _T("%x"), &lcid);

				if (hRfc1766)
				{
					lResult = ReadRegistryString(hRfc1766, szValueName, str);
					if (lResult == ERROR_SUCCESS)
					{
						int nFind = str.Find(_T(";"));
						strText += _T(": ");
						strText += str.Mid(nFind+1);
						sArr.Add(strText);
						nCount++;
						continue;
					}
				}

				if (GetLocaleInfo(lcid, LOCALE_SENGLANGUAGE, szLCdata, 128) > 0)
				{
					strText += _T(": ");
					strText += szLCdata;
					GetLocaleInfo(lcid, LOCALE_SNATIVELANGNAME, szLCdata, 128);
					if (!IsBetween(szLCdata[0], 1, 255))
					{
						GetLocaleInfo(lcid, LOCALE_SLANGUAGE, szLCdata, 128);
					}
					strText += _T(" (");
					strText += szLCdata;
					strText += _T(")");
					sArr.Add(strText);
					nCount++;
					continue;
				}
				else
				{
					int nSel = m_cLanguage.FindString(0, strText);
					if (nSel != CB_ERR)
					{
						m_cLanguage.GetLBText(nSel, strText);
						sArr.Add(strText);
						nCount++;
					}
				}
			}
		};
	}	
	REG_CLOSE_KEY(hRfc1766);
	REG_CLOSE_KEY(hSecKey);
	
	m_cLanguage.ResetContent();
	for (i=0; i<nCount; i++)
	{
		m_cLanguage.AddString(sArr.GetAt(i));
		m_cKBLanguage.AddString(sArr.GetAt(i));
	}

	lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, HKLM_KEYBORD_LAYOUTS, 0, KEY_READ, &hSecKey);
	if (lResult == ERROR_SUCCESS)
	{
		dwIndex =  0;
		while (lResult == ERROR_SUCCESS)
		{
			dwValueNameSize = 63;
			dwValueSize     = 31;
			lResult = RegEnumKey(hSecKey, dwIndex++, (LPTSTR)szValueName, dwValueNameSize);
			if (lResult == ERROR_SUCCESS)
			{
				int nSel = m_cLanguage.FindString(0, szValueName);
				if (nSel != CB_ERR) continue;
				strKey.Format("%s\\%s", HKLM_KEYBORD_LAYOUTS, szValueName);
				lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strKey, 0, KEY_READ, &hSubKey);
				if (lResult == ERROR_SUCCESS)
				{
					lResult = ReadRegistryString(hSubKey, LAYOUT_TEXT, strText);
				}
				if (lResult == ERROR_SUCCESS)
				{
					strKey.Format("%s: %s", szValueName, strText);
					m_cKBLanguage.AddString(strKey);
				}
			}
		};
	}
	int nSel = m_cLanguage.FindString(0, m_strLangID);
	if (nSel == CB_ERR) nSel = 0;
	m_nSelection = m_cLanguage.SetCurSel(nSel);
	OnSelchangeComboLanguages();

	nSel = m_cKBLanguage.FindString(0, m_strKBLangID);
	if (nSel == CB_ERR) nSel = 0;
	m_nKBSelection = m_cKBLanguage.SetCurSel(nSel);
	OnSelchangeComboKBLanguage();

	return TRUE;  // return TRUE  unless you set the focus to a control
}
/////////////////////////////////////////////////////////////////////////////
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void CLanguageSetupDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}
/////////////////////////////////////////////////////////////////////////////
// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CLanguageSetupDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}
/////////////////////////////////////////////////////////////////////////////
void CLanguageSetupDlg::OnSelchangeComboLanguages() 
{
	int nSel = m_cLanguage.GetCurSel();
	if (nSel != CB_ERR)
	{
		if (nSel != m_nSelection)
		{
			GetDlgItem(IDOK)->EnableWindow();
		}
		m_cLanguage.GetLBText(nSel, m_strLangID);
		m_strLangID = m_strLangID.Left(8);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLanguageSetupDlg::OnSelchangeComboKBLanguage() 
{
	int nSel = m_cKBLanguage.GetCurSel();
	if (nSel != CB_ERR)
	{
		if (nSel != m_nKBSelection)
		{
			GetDlgItem(IDOK)->EnableWindow();
		}
		m_cKBLanguage.GetLBText(nSel, m_strKBLangID);
		m_strKBLangID = m_strKBLangID.Left(8);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLanguageSetupDlg::OnOK() 
{
	int   i, nResult = 0;

	if ( (m_cLanguage.GetCount()   > 0) ||
		 (m_cKBLanguage.GetCount() > 0) )
	{
		if ((m_strLangID.GetAt(6) != m_strKBLangID.GetAt(6)) ||
			(m_strLangID.GetAt(7) != m_strKBLangID.GetAt(7)))
		{
			nResult = AfxMessageBox(IDS_REQUEST_DIFFERENT_LANGID, MB_OKCANCEL|MB_ICONINFORMATION);
		}
		if (nResult != IDCANCEL)
		{
			nResult = AfxMessageBox(IDS_REQUEST_REBOOT, MB_YESNOCANCEL|MB_ICONQUESTION);
		}
		if (nResult != IDCANCEL)
		{

			DWORD   dw, dwCount;
			HKEY    hSecKey = NULL;
			LONG    lResult = 0;
			CString str;

			if (m_cLanguage.GetCount()   > 0)
			{
				lResult = RegCreateKeyEx(HKEY_CURRENT_USER, HKCU_DESKTOP, 0, REG_NONE,
												REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
				if (lResult == ERROR_SUCCESS)
				{
					WriteRegistryString(hSecKey, MUILANGUAGEPENDING, m_strLangID);
					WriteRegistryString(hSecKey, MULTIUILANGUAGEID, m_strLangID);
				}
				REG_CLOSE_KEY(hSecKey);
				
				str.Format("%s\\%s", HKU_DEFAULT, HKCU_DESKTOP);
				lResult = RegCreateKeyEx(HKEY_USERS, str, 0, REG_NONE,
												REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
				if (lResult == ERROR_SUCCESS)
				{
					dwCount = m_strLangID.GetLength()+1;
					WriteRegistryString(hSecKey, MUILANGUAGEPENDING, m_strLangID);
					WriteRegistryString(hSecKey, MULTIUILANGUAGEID, m_strLangID);
				}
				REG_CLOSE_KEY(hSecKey);
			}


			lResult = RegCreateKeyEx(HKEY_CURRENT_USER, HKCU_KEYBORD_LAYOUT, 0, REG_NONE,
											REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
			if (lResult == ERROR_SUCCESS)
			{
				int i, j;
				CString strKB;
				WriteRegistryString(hSecKey, _T("1"), m_strKBLangID);
				for (i=0, j=2; i<m_cLanguage.GetCount(); i++)
				{
					m_cLanguage.GetLBText(i, strKB);
					strKB = strKB.Left(8);
					if (strKB != m_strKBLangID)
					{
						str.Format(_T("%d"), j++);
						WriteRegistryString(hSecKey, str, strKB);
					}
				}
			}
			REG_CLOSE_KEY(hSecKey);

			lResult = RegCreateKeyEx(HKEY_CURRENT_USER, HKCU_KEYBORD_TOGGLE, 0, REG_NONE,
											REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
			if (lResult == ERROR_SUCCESS)
			{
				WriteRegistryString(hSecKey, HOTKEY, _T("1"));
				WriteRegistryString(hSecKey, LANGUAGE_HOTKEY, _T("1"));
				WriteRegistryString(hSecKey, LAYOUT_HOTKEY, _T("2"));
			}

			if (m_strKBLangID != _T("00000419"))
			{
				REG_CLOSE_KEY(hSecKey);
				DWORD   dwIndex=0;
				_TCHAR    szValueName[MAX_PATH];
				while (lResult == ERROR_SUCCESS)
				{
					lResult = ::RegEnumKey(HKEY_USERS, dwIndex++, (LPSTR)szValueName, MAX_PATH);
					if (lResult == ERROR_SUCCESS && CString(szValueName).Find(_T("_Classes")) == -1)
					{
						str.Format("%s\\%s", szValueName, HKCU_KEYBORD_LAYOUT);
						lResult = RegCreateKeyEx(HKEY_USERS, str, 0, REG_NONE,
														REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
						if (lResult == ERROR_SUCCESS)
						{
							int i, j;
							CString strKB;
							WriteRegistryString(hSecKey, _T("1"), m_strKBLangID);
							for (i=0, j=2; i<m_cLanguage.GetCount(); i++)
							{
								m_cLanguage.GetLBText(i, strKB);
								strKB = strKB.Left(8);
								if (strKB != m_strKBLangID)
								{
									str.Format(_T("%d"), j++);
									WriteRegistryString(hSecKey, str, strKB);
								}
							}
						}
						REG_CLOSE_KEY(hSecKey);

						str.Format("%s\\%s", HKU_DEFAULT, HKCU_KEYBORD_TOGGLE);
						lResult = RegCreateKeyEx(HKEY_USERS, str, 0, REG_NONE,
														REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
						if (lResult == ERROR_SUCCESS)
						{
							WriteRegistryString(hSecKey, HOTKEY, _T("1"));
							WriteRegistryString(hSecKey, LANGUAGE_HOTKEY, _T("1"));
							WriteRegistryString(hSecKey, LAYOUT_HOTKEY, _T("2"));
						}
						REG_CLOSE_KEY(hSecKey);
					}
				}
			}

			LCID lcid = MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US);
			_stscanf(m_strLangID, _T("%x"), &lcid);
			_TCHAR szLCdata[128];
			lResult = RegCreateKeyEx(HKEY_CURRENT_USER, HKCU_INTERNATIONAL_SECTION, 0, REG_NONE,
											REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
			if (lResult == ERROR_SUCCESS)
			{
				for (i=0; g_International[i].sRegKey != NULL; i++)
				{
					GetLocaleInfo(lcid, g_International[i].lcType, szLCdata, 128);
					WriteRegistryString(hSecKey, g_International[i].sRegKey, szLCdata);
					ASSERT(lResult == ERROR_SUCCESS);
				}
				WriteRegistryString(hSecKey, g_International[i].sRegKey, szLCdata);
				lResult = RegSetValueEx(hSecKey, INTERNATIONAL_LOCALE, NULL, REG_SZ, (BYTE*)LPCTSTR(m_strLangID), m_strLangID.GetLength()+1);
				ASSERT(lResult == ERROR_SUCCESS);
			}
			REG_CLOSE_KEY(hSecKey);

			lResult = RegCreateKeyEx(HKEY_LOCAL_MACHINE, HKLM_NLS_CODEPAGE, 0, REG_NONE,
											REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hSecKey, &dw);
			if (lResult == ERROR_SUCCESS)
			{
				GetLocaleInfo(lcid, LOCALE_IDEFAULTCODEPAGE, szLCdata, 128);
				WriteRegistryString(hSecKey, OEM_CODEPAGE, szLCdata);
				GetLocaleInfo(lcid, LOCALE_IDEFAULTANSICODEPAGE, szLCdata, 128);
				WriteRegistryString(hSecKey, ANSI_CODEPAGE, szLCdata);
			}
			 
		}
	}
	else
	{
		nResult = IDNO;
	}

	if (nResult != IDCANCEL)
	{
		CDialog::OnOK();
	}
	
	if (nResult == IDYES)
	{	
		HANDLE hToken;              // handle to process token 
		TOKEN_PRIVILEGES tkp;        // ptr. to token structure 
	   
		BOOL fResult;                  // system shutdown flag 
	   
		// Get the current process token handle 
		// so we can get debug privilege. 
		OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) ;

		// Get the LUID for debug privilege. 
		LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid); 
	   
		tkp.PrivilegeCount = 1;  // one privilege to set    
		tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED; 
	   
		// Get shutdown privilege for this process. 
		fResult = AdjustTokenPrivileges(hToken, FALSE, &tkp, 0, (PTOKEN_PRIVILEGES) NULL, 0); 
		ExitWindowsEx(EWX_LOGOFF, 0);
	}
}
/////////////////////////////////////////////////////////////////////////////
LONG CLanguageSetupDlg::ReadRegistryString(HKEY hKey, LPCTSTR strSection, CString &strValue)
{
	DWORD dwType, dwCount = 0;
	LONG lResult = RegQueryValueEx(hKey, strSection, NULL, &dwType, NULL, &dwCount);
	if ((lResult == ERROR_SUCCESS) && (dwType == REG_SZ))
	{
		lResult = RegQueryValueEx(hKey, strSection, NULL, &dwType, (LPBYTE)strValue.GetBufferSetLength(dwCount), &dwCount);
		strValue.ReleaseBuffer();
	}
	return lResult;
}
/////////////////////////////////////////////////////////////////////////////
LONG CLanguageSetupDlg::WriteRegistryString(HKEY hKey, LPCTSTR sName, LPCTSTR sValue)
{
	DWORD dwCount = 0;
	LONG lResult;
	dwCount = _tcslen(sValue) * sizeof(_TCHAR);
	lResult = RegSetValueEx(hKey, sName, NULL, REG_SZ, (BYTE*)sValue, dwCount);
	return lResult;
}
