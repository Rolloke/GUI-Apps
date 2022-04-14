// HealthControl.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "HealthControl.h"
#include "HealthControlDlg.h"
#include ".\healthcontrol.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CHealthControlApp

BEGIN_MESSAGE_MAP(CHealthControlApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_UPDATE_COMMAND_UI(ID_APP_EXIT, OnUpdateAppExit)
END_MESSAGE_MAP()
#ifdef _DEBUG
BOOL CALLBACK EnumResNameProc(HMODULE hModule, LPCTSTR lpszType, LPTSTR lpszName, LONG_PTR lParam)
{
	CString *pS = (CString*) lParam;
	pS->Format(_T("#%d\n"), lpszName);
	TRACE(*pS);
	return 0;
}

BOOL CALLBACK EnumResLangProc(HANDLE hModule, LPCTSTR lpszType, LPCTSTR lpszName, WORD wIDLanguage, LONG_PTR lParam)
{
	_TCHAR szAbb[8];
	GetLocaleInfo(wIDLanguage, LOCALE_SABBREVLANGNAME, szAbb, 8);
	TRACE(_T("%d, %x, %s\n"), wIDLanguage, wIDLanguage, szAbb);
	return 1;
}
#endif

/////////////////////////////////////////////////////////////////////////////
// CHealthControlApp-Erstellung
CHealthControlApp::CHealthControlApp()
{
#if WKCLASSES == 1
	LCID Locale = InitVideteLocaleLanguage();
	XTIB::SetThreadName(_T("MainThread"));
//	SetThreadLocale(Locale);

#ifdef _DEBUG
	Locale = GetSystemDefaultLCID();
	CString sName;
	BOOL bReturn = EnumResourceNames(0, RT_STRING, EnumResNameProc, (LONG_PTR)&sName);
	bReturn = EnumResourceLanguages(NULL, RT_STRING, sName, (ENUMRESLANGPROC) EnumResLangProc, 0);
#endif
#endif

	m_pDlg = NULL;
#if WKCLASSES == 1
	CWK_Profile wkp;
	m_bStandalone = wkp.GetInt(REG_PATH_PC_HEALTH, STAND_ALONE, TRUE);
#endif
   m_bStandalone = TRUE;
	m_bIconCreated = FALSE;
	m_bIsDts = FALSE;
}
/////////////////////////////////////////////////////////////////////////////
CHealthControlApp::~CHealthControlApp()
{
	WK_DELETE(m_pDlg);
}
/////////////////////////////////////////////////////////////////////////////
// Das einzige CHealthControlApp-Objekt
CHealthControlApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHealthControlApp Initialisierung
BOOL CHealthControlApp::InitInstance()
{
#if WKCLASSES == 1
	if (WK_ALONE(WK_APP_NAME_HEALT_CONTROL) == FALSE)
	{
		return FALSE;
	}

	WK_ApplicationId	AppID	= WAI_HEALTH_CONTROL;
#if _MFC_VER >= 0x0710
	SetLanguageParameters(0, 0);
#else    // ! _MFC_VER >= 0x0710
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else    // !_AFXDLL
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif   // _AFXDLL
#endif   // _MFC_VER

	// Debugger öffnen
	InitDebugger(_T("HealtControl.log"),AppID, NULL);

	WK_TRACE(_T("Starting HealthControl..."));
	WK_PRINT_VERSION(7, 4, 16, 31, _T("$Date: 6.03.06 14:37 $"), // @AUTO_UPDATE
		0);	// DO NOT TOUCH THIS LINE
#endif   // WKCLASSES

	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	SetRegistryKey(_T("videte"));

	CString str(m_pszHelpFilePath);
	str.MakeLower();
	m_bIsDts = str.Find(_T("c:\\dv")) != -1 ? TRUE : FALSE;
#if WKCLASSES == 1
	CWK_Profile wkp;
	if (m_bIsDts)
	{
		m_bIsShellMode = wkp.GetInt(SECTION_DVSTARTER, AS_SHELL, FALSE);
	}
	else
	{
		m_bIsShellMode = wkp.GetInt(SECTION_LAUNCHER, SHELL_MODE, FALSE);
	}
#endif
	m_pDlg = new CHealthControlDlg;
	if (m_pDlg && m_pDlg->Create(CHealthControlDlg::IDD))
	{
		m_pMainWnd = m_pDlg;
	}
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CHealthControlApp::ExitInstance()
{
#if WKCLASSES == 1
	CloseDebugger();
#endif
	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlApp::OnAppExit()
{
   if (m_pDlg && m_pDlg->m_hWnd)
   {
		m_pDlg->PostMessage(WM_COMMAND,(WPARAM)MAKELONG(IDOK, BN_CLICKED), NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void CHealthControlApp::OnUpdateAppExit(CCmdUI *pCmdUI)
{
	pCmdUI->Enable();
}
/////////////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0710
int CHealthControlApp::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
{
#if OEMGUI == 1
	if (uCodePage == 0)
	{
		uCodePage = _ttoi(COemGuiApi::GetCodePage());
	}
	else
	{
		COemGuiApi::ChangeLanguageResource();
		InitVideteLocaleLanguage();
	}

//  Set the CodePage for MBCS conversion, if necessary
//	CWK_String::SetCodePage(uCodePage);
//	CWK_Profile wkp;
//	wkp.SetCodePage(CWK_String::GetCodePage());

	if (dwCPbits == 0)
	{
		dwCPbits = COemGuiApi::GetCodePageBits();
	}
/*
	if (dwCPbits)
	{
		SetFontFaceNamesFromCodePageBits(dwCPbits);
	}
	else
	{
		CSkinDialog::DoUseGlobalFonts(FALSE);
	}
*/
#endif
	return 0;
}
#endif

int CHealthControlApp::About(void)
{
   AfxMessageBox(_T("Health Control\nVersion 1.0"), MB_ICONINFORMATION|MB_OK);
   return 0;
}
