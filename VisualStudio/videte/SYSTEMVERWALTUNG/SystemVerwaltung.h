// SystemVerwaltung.h : main header file for the SYSTEMVERWALTUNG application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif
#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
/////////////////////////////////////////////////////////////////////////////
#include "resource.h"       // main symbols
#include "Output.h"

#define REG_KEY_SUPERVISOR			_T("Systemverwaltung")
#define REG_KEY_USE_MONITOR			_T("UseMonitor")

#define PROFESSIONAL_MODE			_T("ProfessionalMode")

#if _MFC_VER >= 0x0710
 #define REG_KEY_CLIENT_SETTINGS	_T("IdipClient\\Settings")
#else
 #define REG_KEY_CLIENT_SETTINGS	_T("Vision\\Settings")
#endif
 #define REG_KEY_VISION_SETTINGS	_T("Vision\\Settings")

/////////////////////////////////////////////////////////////////////////////
int CheckDigits(CString& s);
int IsCameraValid(const CString& sCameras, CString& sWrong, int& iStart, DWORD dwMaxCams=ULONG_MAX);

/////////////////////////////////////////////////////////////////////////////
#define USER_MSG_UPDATE_PARENT_TREE 1
#define USER_MSG_RELOAD_SETTINGS	2
#define USER_MSG_REINIT_TREE		3
/////////////////////////////////////////////////////////////////////////////
#define ID_CONNECT_LOW	0x0E00
#define ID_CONNECT_HIGH	0x0EFF

#ifdef UNICODE
	typedef WINBASEAPI BOOL (WINAPI* GETCOMPUTERNAMEEX_FNC)(COMPUTER_NAME_FORMAT NameType, LPWSTR lpBuffer, LPDWORD nSize);
	#define GETCOMPUTERNAMEEX "GetComputerNameExW"
#else
	typedef WINBASEAPI BOOL (WINAPI* GETCOMPUTERNAMEEX_FNC)(COMPUTER_NAME_FORMAT NameType, LPSTR lpBuffer, LPDWORD nSize);
	#define GETCOMPUTERNAMEEX "GetComputerNameExA"
#endif

/////////////////////////////////////////////////////////////////////////////
class CWK_Dongle;
class CKBInput;

/////////////////////////////////////////////////////////////////////////////
// CSVApp:


class CSVApp : public CWinApp,
	public CWinXPThemes,		// Info about XP Themes
	public COsVersionInfo		// OS Version Info
{
public:
	CSVApp();
	virtual ~CSVApp();

public:
	inline CImageList*		GetSmallImage();
	inline CImageList*		GetStateImage();
	inline CImageList*		Get3StateImage();
	inline CImageList*		GetRadioImage();
	inline CWK_Dongle*		GetDongle();
	inline LPCTSTR			GetModuleSection();
	inline LPCTSTR			GetLauncherSection();
	inline LPCTSTR			GetSytemIni();
	CString					GetHTMLCodePage();
	int GetIntValue(LPCTSTR sValue, int nDefault);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSVApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL


// Implementation
public:
	BOOL Login();
	BOOL LoginAsSuperVisor();
	void InitHTML();
	void CreateKeyboardLayouts(LPCTSTR sLang1, LPCTSTR sLang2);
	int	 ImageByExType(ExOutputType e) const;
	BOOL StartApplication(WK_ApplicationId wai, WORD wStartParam);
	void ReadOpenDirPath();
	BOOL GetComputerNameEx(COMPUTER_NAME_FORMAT NameType, LPTSTR lpBuffer, LPDWORD nSize);

// NetApi Functions
	BOOL	IsNetExeOK();


private:
	static UINT EvaluateCommPortThreadProc(LPVOID pVoid);
	void RegisterWindowClass();
	BOOL SplitCommandLine(const CString sCommandLine, CStringArray& saParameters, LPCTSTR szParam);

	//{{AFX_MSG(CSVApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	
private:
	CImageList		m_SmallImage;
	CImageList		m_StateImage;
	CImageList		m_3StateImage;
	CImageList		m_RadioImage;

public:
	CUser*		m_pUser;
	CPermission*m_pPermission;
	CString		m_sLocalWWW;
	CString		m_sOpenDir;
	CString		m_sApplicationPath;
	BOOL		m_bRunAnyLinkUnit;
	BOOL		m_bUpdateLanguageDependencies;
	BOOL		m_bProfessional;
	
	// Dongle
private:
	CWK_Dongle*		m_pDongle;

	HMODULE			m_hKernel32;
	GETCOMPUTERNAMEEX_FNC m_pGetComputerNameEx;
public:
};
/////////////////////////////////////////////////////////////////////////////
extern CSVApp theApp;
/////////////////////////////////////////////////////////////////////////////
inline CImageList* CSVApp::GetSmallImage()
{
	return &m_SmallImage;
}
/////////////////////////////////////////////////////////////////////////////
inline CImageList* CSVApp::GetStateImage()
{
	return &m_StateImage;
}
/////////////////////////////////////////////////////////////////////////////
inline CImageList* CSVApp::Get3StateImage()
{
	return &m_3StateImage;
}
/////////////////////////////////////////////////////////////////////////////
inline CImageList* CSVApp::GetRadioImage()
{
	return &m_RadioImage;
}
inline CWK_Dongle* CSVApp::GetDongle()
{
	return m_pDongle;
}
/////////////////////////////////////////////////////////////////////////////
inline LPCTSTR CSVApp::GetModuleSection()
{
	return WK_APP_NAME_LAUNCHER _T("\\Modules");
}
/////////////////////////////////////////////////////////////////////////////
inline LPCTSTR CSVApp::GetLauncherSection()
{
	return WK_APP_NAME_LAUNCHER;
}
/////////////////////////////////////////////////////////////////////////////
inline LPCTSTR CSVApp::GetSytemIni()
{
	return _T("system.ini");
}

BOOL AddNoDoubleToWordArray(WORD w, CWordArray &wa);
CString StringCharsNotInSet(const CString s, const CString& sCharSet);
void DDV_Validate_IP_adr(CDataExchange* pDX, CString const& value, UINT nID=0);
void DDV_MinChars(CDataExchange* pDX, CString const& value, int nMin);
void DDX_Text(CDataExchange* pDX, LPCTSTR sFmt, UINT nErrorID, int nIDC, int& value);
void RemoveLastCharacter(CString&s, TCHAR c);
