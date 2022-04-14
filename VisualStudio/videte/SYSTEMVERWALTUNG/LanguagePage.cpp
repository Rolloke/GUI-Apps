// LanguagePage.cpp : implementation file
//

#include "stdafx.h"
#include "SystemVerwaltung.h"
#include "LanguagePage.h"
#include "oemgui\oemgui.h"
#include ".\languagepage.h"

#define DLLNAME_OEMGUI	_T("OemGui")

#define HKLM_KEYBORD_LAYOUTS		_T("SYSTEM\\CurrentControlSet\\Control\\Keyboard Layouts\\")


/////////////////////////////////////////////////////////////////////////////
// CLanguagePage dialog
IMPLEMENT_DYNAMIC(CLanguagePage, CSVPage)
CLanguagePage::CLanguagePage(CSVView* pParent /*=NULL*/)
	 : CSVPage(CLanguagePage::IDD)
{
	m_dFixedFontSize    = 12;
	m_dVariableFontSize = 12;
	m_dDialogFontSize   = 8;
	m_bFontSizes = FALSE;
	m_pParent = pParent;
	Create(IDD,pParent);
}
/////////////////////////////////////////////////////////////////////////////
CLanguagePage::~CLanguagePage()
{
	DWORD dwCPBits = COemGuiApi::GetCodePageBits();
	if (dwCPBits == NULL)
	{
		CSkinDialog::DoUseGlobalFonts(FALSE);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLanguagePage::DoDataExchange(CDataExchange* pDX)
{
	CSVPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLanguagePage)
	DDX_Control(pDX, IDC_COMBO_CODEPAGE, m_comboCodePage);
	DDX_Control(pDX, IDC_COMBO_CODEPAGE_BITS, m_comboCodePageBits);
	DDX_Control(pDX, IDC_COMBO_KEYBOARD, m_comboKeyBoard);
	DDX_Control(pDX, IDC_COMBO_INITIAL_KEYBOARD, m_comboInitialKeyBoard);
	DDX_Control(pDX, IDC_TXT_LANGUAGE_NAME, m_txtLanguageName);
	DDX_Check(pDX, IDC_CK_FONT_SIZES, m_bFontSizes);
	//}}AFX_DATA_MAP
	if (m_bFontSizes)
	{
		if (pDX->m_bSaveAndValidate)
		{
			DDX_Text(pDX, IDC_EDT_FIXED_FONT, m_dFixedFontSize);
			DDV_MinMaxFloat(pDX, m_dFixedFontSize, 6, 20);
			DDX_Text(pDX, IDC_EDT_VARIABLE_FONT, m_dVariableFontSize);
			DDV_MinMaxFloat(pDX, m_dVariableFontSize, 6, 20);
			DDX_Text(pDX, IDC_EDT_DIALOG_FONT, m_dDialogFontSize);
			DDV_MinMaxFloat(pDX, m_dDialogFontSize, 6, 18);
		}
		else
		{
			CString str;
			str.Format(_T("%.1f"), m_dFixedFontSize);
			DDX_Text(pDX, IDC_EDT_FIXED_FONT, str);
			str.Format(_T("%.1f"), m_dVariableFontSize);
			DDX_Text(pDX, IDC_EDT_VARIABLE_FONT, str);
			str.Format(_T("%.1f"), m_dDialogFontSize);
			DDX_Text(pDX, IDC_EDT_DIALOG_FONT, str);
		}
	}
	else
	{
		CString str = _T("");
		DDX_Text(pDX, IDC_EDT_FIXED_FONT, str);
		DDX_Text(pDX, IDC_EDT_VARIABLE_FONT, str);
		DDX_Text(pDX, IDC_EDT_DIALOG_FONT, str);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CLanguagePage message handlers
/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CLanguagePage, CSVPage)
	//{{AFX_MSG_MAP(CLanguagePage)
	ON_CBN_SELCHANGE(IDC_COMBO_CODEPAGE, OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_CODEPAGE_BITS, OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_KEYBOARD, OnCbnSelchangeCombo)
	ON_CBN_SELCHANGE(IDC_COMBO_INITIAL_KEYBOARD, OnCbnSelchangeCombo)
	ON_BN_CLICKED(IDC_CK_FONT_SIZES, OnBnClickedCkFontSizes)
	ON_EN_CHANGE(IDC_EDT_FIXED_FONT, OnEnChangeEdtFont)
	ON_EN_CHANGE(IDC_EDT_VARIABLE_FONT, OnEnChangeEdtFont)
	ON_EN_CHANGE(IDC_EDT_DIALOG_FONT, OnEnChangeEdtFont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
BOOL CLanguagePage::OnInitDialog()
{
	CSVPage::OnInitDialog();
	int nKeyBoard = COemGuiApi::GetKeyboardInput(1).IsActive();
	CLoadResourceDll dllLanguages(DLLNAME_OEMGUI);
	COemGuiApi::ToggleKeyboardInput(nKeyBoard);

	CString str;
	DWORD dwBits = 0;
	m_txtLanguageName.SetWindowText(dllLanguages.GetLanguageName());
	int iItem, j, nCount, i, nLanguages = dllLanguages.GetNoOfLanguages();

	str.LoadString(IDS_USE_SYSTEM);	// Betriebssystem
	m_comboKeyBoard.AddString(str);
	str.LoadString(IDS_LANGUAGE_DEPENDENT);	// Sprachabhängig
	m_comboInitialKeyBoard.AddString(str);
	m_comboCodePage.AddString(str);
	iItem = m_comboCodePageBits.AddString(str);
	m_comboCodePageBits.SetItemData(iItem, 0xffffffff);

	for (i=0; i<nLanguages; i++)
	{
		str = dllLanguages.GetLanguageName(i);
		m_comboKeyBoard.AddString(str);
		m_comboInitialKeyBoard.AddString(str);
		m_saLangAbbreviations.Add(dllLanguages.GetLanguageAbbreviation(i));
	}

	LONG  lResult;
	HKEY   hSecKey = NULL;
	DWORD   dwIndex, dwValueNameSize, dwValueSize;
	_TCHAR    szValueName[64];
	_TCHAR    szValue[32];

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
				LCID lcid;
				_stscanf(szValueName, _T("%x"), &lcid);
				GetLocaleInfo(lcid, LOCALE_SABBREVLANGNAME, szValue, 32);
				if (FindString(m_saLangAbbreviations, szValue, FALSE, TRUE) == -1)
				{
					m_saLangAbbreviations.Add(szValue);
					GetLocaleInfo(lcid, LOCALE_SENGLANGUAGE, szValue, 32);
					GetLocaleInfo(lcid, LOCALE_SENGCOUNTRY, szValueName, 64);
					str.Format(_T("%s (%s)"), szValue, szValueName);
					m_comboKeyBoard.AddString(str);
					m_comboInitialKeyBoard.AddString(str);
				}
			}
		}
		RegCloseKey(hSecKey);
	}
	
	for (i=0; i<nLanguages; i++)
	{
		str = dllLanguages.GetLanguageName(i);
		dwBits = dllLanguages.GetCodePageBits(i);
		nCount = m_comboCodePageBits.GetCount();
		for (j=1; j<nCount; j++)
		{
			if (m_comboCodePageBits.GetItemData(j) == dwBits)
			{
				CString sItem;
				m_comboCodePageBits.GetLBText(j, sItem);
				m_comboCodePageBits.DeleteString(j);
				str += _T(", ") + sItem;
			}
		}
		iItem = m_comboCodePageBits.AddString(str);
		m_comboCodePageBits.SetItemData(iItem, dwBits);
	}

	for (i=0; i<nLanguages; i++)
	{
		str = dllLanguages.GetLanguageName(i);
		CString sCodePage = dllLanguages.GetCodePage(i);
		nCount = m_saCodePages.GetCount();
		for (j=0; j<nCount; j++)
		{
			if (m_saCodePages.GetAt(j) == sCodePage)
			{
				CString sItem;
				m_comboCodePage.GetLBText(j+1, sItem);
				m_comboCodePage.DeleteString(j+1);
				m_saCodePages.RemoveAt(j);
				str += _T(", ") + sItem;
			}
		}
		m_comboCodePage.AddString(str);
		m_saCodePages.Add(sCodePage);
	}
	Initialize();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}
/////////////////////////////////////////////////////////////////////////////
void CLanguagePage::Initialize()
{
	CWK_Profile&wkp = GetProfile();
	int i, nLanguages;
	CString str = wkp.GetString(SECTION_COMMON, COMMON_CODEPAGE, NULL);
	i = FindString(m_saCodePages, str, FALSE, TRUE);
	if (i==-1)
	{
		m_comboCodePage.SetCurSel(0);
	}
	else
	{
		m_comboCodePage.SetCurSel(i+1);
	}

	str = wkp.GetString(SECTION_COMMON, COMMON_DEFKEBOARD, NULL);
	i = FindString(m_saLangAbbreviations, str, FALSE, TRUE);
	if (i==-1)
	{
		m_comboKeyBoard.SetCurSel(0);
	}
	else
	{
		m_comboKeyBoard.SetCurSel(i+1);
	}

	str = wkp.GetString(SECTION_COMMON, COMMON_INITIAL_KEBOARD, NULL);
	i = FindString(m_saLangAbbreviations, str, FALSE, TRUE);
	if (i==-1)
	{
		m_comboInitialKeyBoard.SetCurSel(0);
	}
	else
	{
		m_comboInitialKeyBoard.SetCurSel(i+1);
	}

	nLanguages = m_comboCodePageBits.GetCount();
	m_comboCodePageBits.SetCurSel(0);
	DWORD dwCPbits = wkp.GetInt(SECTION_COMMON, COMMON_CODEPAGEBITS, -1);
	for (i=1; i<=nLanguages; i++)
	{
		if (m_comboCodePageBits.GetItemData(i) == dwCPbits)
		{
			m_comboCodePageBits.SetCurSel(i);
			break;
		}
	}

	DWORD dwSize, dwType = wkp.GetType(SECTION_COMMON, COMMON_FIXEDFONT, &dwSize);
	if (dwType == REG_DWORD && dwSize == sizeof(DWORD))
	{
		m_dFixedFontSize    = 0.1f * wkp.GetInt(SECTION_COMMON, COMMON_FIXEDFONT, 120);
		m_dVariableFontSize = 0.1f * wkp.GetInt(SECTION_COMMON, COMMON_VARIABLEFONT, 120);
		m_dDialogFontSize   = 0.1f * wkp.GetInt(SECTION_COMMON, COMMON_DIALOGFONT, 80);
		m_bFontSizes = TRUE;
	}
	else
	{
		m_bFontSizes = FALSE;
	}
	UpdateData(FALSE);
	EnableCtrls();
}
/////////////////////////////////////////////////////////////////////////////
void CLanguagePage::SaveChanges()
{
	if (UpdateData())
	{
		CWK_Profile&wkp = GetProfile();
		int nCurSel = m_comboCodePage.GetCurSel();
		if (nCurSel > 0)
		{
			wkp.WriteString(SECTION_COMMON, COMMON_CODEPAGE, m_saCodePages.GetAt(nCurSel-1));
		}
		else
		{
			wkp.DeleteEntry(SECTION_COMMON, COMMON_CODEPAGE);
		}

		nCurSel = m_comboKeyBoard.GetCurSel();
		if (nCurSel > 0)
		{
			wkp.WriteString(SECTION_COMMON, COMMON_DEFKEBOARD, m_saLangAbbreviations.GetAt(nCurSel-1));
		}
		else
		{
			wkp.DeleteEntry(SECTION_COMMON, COMMON_DEFKEBOARD);
		}

		nCurSel = m_comboInitialKeyBoard.GetCurSel();
		if (nCurSel > 0)
		{
			wkp.WriteString(SECTION_COMMON, COMMON_INITIAL_KEBOARD, m_saLangAbbreviations.GetAt(nCurSel-1));
		}
		else
		{
			wkp.WriteString(SECTION_COMMON, COMMON_INITIAL_KEBOARD, _T("xxx"));
		}

		nCurSel = m_comboCodePageBits.GetCurSel();
		if (nCurSel > 0)
		{
			wkp.WriteInt(SECTION_COMMON, COMMON_CODEPAGEBITS, m_comboCodePageBits.GetItemData(nCurSel));
		}
		else
		{
			wkp.DeleteEntry(SECTION_COMMON, COMMON_CODEPAGEBITS);
		}
		DWORD dwCPBits = COemGuiApi::GetCodePageBits();
		if (dwCPBits == 0)
		{
			m_bFontSizes = FALSE;
			CDataExchange dx (this, FALSE);
			DDX_Check(&dx, IDC_CK_FONT_SIZES, m_bFontSizes);
			EnableCtrls();
		}
		if (m_bFontSizes)
		{
			wkp.WriteInt(SECTION_COMMON, COMMON_FIXEDFONT, (int)(m_dFixedFontSize*10));
			wkp.WriteInt(SECTION_COMMON, COMMON_VARIABLEFONT, (int)(m_dVariableFontSize*10));
			wkp.WriteInt(SECTION_COMMON, COMMON_DIALOGFONT, (int)(m_dDialogFontSize*10));
		}
		else
		{
			wkp.DeleteEntry(SECTION_COMMON, COMMON_FIXEDFONT);
			wkp.DeleteEntry(SECTION_COMMON, COMMON_VARIABLEFONT);
			wkp.DeleteEntry(SECTION_COMMON, COMMON_DIALOGFONT);
		}
		
		theApp.StartApplication(WAI_INVALID, LAUNCHER_UPDATE_LANG_SETTINGS);
		CWK_String::SetCodePage(_ttoi(COemGuiApi::GetCodePage()));
		wkp.SetCodePage(CWK_String::GetCodePage());
		if (dwCPBits)
		{
			SetFontFaceNamesFromCodePageBits(0);
			SetFontFaceNamesFromCodePageBits(dwCPBits);
		}
		else
		{
			SetFontFaceNamesFromCodePageBits(2);
			SetFontFaceNames(NULL, NULL);
		}
		if (m_hWnd && CSkinDialog::SetChildWindowFont(m_hWnd))
		{
			if (m_ToolTip.m_hWnd)
			{
				m_ToolTip.SetFont(CSkinDialog::GetDialogItemGlobalFont(), 0);
			}
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
void CLanguagePage::CancelChanges()
{
	Initialize();
}
/////////////////////////////////////////////////////////////////////////////
void CLanguagePage::OnCbnSelchangeCombo()
{
	SetModified(TRUE, FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CLanguagePage::OnBnClickedCkFontSizes()
{
	CDataExchange dx (this, TRUE);
	DDX_Check(&dx, IDC_CK_FONT_SIZES, m_bFontSizes);
	EnableCtrls();
	UpdateData(FALSE);
	SetModified(TRUE, FALSE);
}
/////////////////////////////////////////////////////////////////////////////
void CLanguagePage::EnableCtrls()
{
	GetDlgItem(IDC_EDT_FIXED_FONT)->EnableWindow(m_bFontSizes);
	GetDlgItem(IDC_EDT_VARIABLE_FONT)->EnableWindow(m_bFontSizes);
	GetDlgItem(IDC_EDT_DIALOG_FONT)->EnableWindow(m_bFontSizes);
}
/////////////////////////////////////////////////////////////////////////////
void CLanguagePage::OnEnChangeEdtFont()
{
	SetModified(TRUE, FALSE);
}

