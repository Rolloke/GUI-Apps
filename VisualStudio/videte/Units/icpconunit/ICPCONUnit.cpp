/////////////////////////////////////////////////////////////////////////////
// PROJECT:		ICPCONUnit
// FILE:		$Workfile: ICPCONUnit.cpp $
// ARCHIVE:		$Archive: /Project/Units/ICPCONUnit/ICPCONUnit.cpp $
// CHECKIN:		$Date: 28.02.06 11:29 $
// VERSION:		$Revision: 21 $
// LAST CHANGE:	$Modtime: 28.02.06 11:25 $
// BY AUTHOR:	$Author: Rolf.kary-ehlers $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ICPCONUnit.h"
#include "ICPCONUnitDlg.h"
#include "ICPCONDll\ICP7000Module.h"

#include <WKClasses\wk_string.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CICPCONUnitApp

BEGIN_MESSAGE_MAP(CICPCONUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CICPCONUnitApp)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CICPCONUnitApp construction

CICPCONUnitApp::CICPCONUnitApp()
{
	InitVideteLocaleLanguage();
	m_pDlg = NULL;
	CString sBaseDVRT          = REG_LOC_BASE;
	m_sRegistryLocation        = sBaseDVRT + REG_LOC_UNIT;
	m_sRegistryLocationModules = m_sRegistryLocation + REG_LOC_MODULE_UNIT;
	m_sRegistryLocationPorts   = m_sRegistryLocation + REG_LOC_PORT_UNIT;
	m_bIsInMenuLoop = FALSE;
	CWK_Profile wkp(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, m_sRegistryLocation, _T(""));

	m_bStandalone = wkp.GetInt(DEBUG_SETTINGS, STAND_ALONE, FALSE);
}
/////////////////////////////////////////////////////////////////////////////
CICPCONUnitApp::~CICPCONUnitApp()
{
//	WK_DELETE(m_pDlg);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CICPCONUnitApp object

CICPCONUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CICPCONUnitApp initialization	
BOOL CICPCONUnitApp::InitInstance()
{
	if (WK_ALONE(WK_APP_NAME_ICPCON_UNIT) == FALSE)
	{
		return FALSE;
	}

#if _MFC_VER >= 0x0710
	SetLanguageParameters(0, 0);
#else
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	CString str;
	str.Format(_T("%s.log"), m_pszAppName);
	InitDebugger(str, WAI_ICPCON_UNIT);

#ifndef _DEBUG
	CWK_Dongle dongle;
	CWK_Profile wkpUnit(CWK_Profile::WKP_REGPATH, HKEY_LOCAL_MACHINE, theApp.m_sRegistryLocation, _T(""));
	if (!dongle.RunICPCONUnit() && wkpUnit.GetInt(SETTINGS_SECTION, ICP_SWITCH_PANEL, 0) == 0)
	{
		WK_TRACE(_T("No Dongle available\n"));
		return FALSE;
	}
#endif
	
	WK_TRACE(_T("Starting ICP CON Unit..."));
	WK_PRINT_VERSION(7, 4, 16, 31, _T("$Date: 28.02.06 11:29 $"), // @AUTO_UPDATE
		0);	// DO NOT TOUCH THIS LINE

	if (m_pszAppName)
	{
		free((void*)m_pszAppName);
	}

	// Load OEM settings
	CString sIniFile = m_pszHelpFilePath;
	CString sApplicationDir;
	int nSlash = sIniFile.ReverseFind(_T('\\'));
	if (nSlash != -1)
	{
		sApplicationDir = sIniFile.Left(nSlash+1);
		sIniFile = sApplicationDir + OEM_FILE;
	}
#ifdef _DEBUG
	CString sDebugInifile = _T("C:\\Security\\")  OEM_FILE;
	if (::GetFileAttributes(sDebugInifile) != INVALID_FILE_ATTRIBUTES)
	{
		sIniFile = sDebugInifile;
	}
#endif

	WK_TRACE(_T("Load OEM sIniFile\n"));
	CWK_Profile wkpOEM(CWK_Profile::WKP_INIFILE_ABSOLUT, NULL, _T(""), sIniFile); 
	COemGuiApi::LoadOEMSkinOptions(wkpOEM);
	CSkinDialog::SetButtonImages(SC_CLOSE    , (UINT)-1, 0, 0);

	m_pszAppName = _tcsdup(COemGuiApi::GetApplicationName(WAI_ICPCON_UNIT));
	AFX_MODULE_STATE*pState = AfxGetModuleState();
	if (pState)
	{
		if (pState->m_lpszCurrentAppName)
		{
			pState->m_lpszCurrentAppName = m_pszAppName;
		}
	}

	m_pDlg = new CICPCONUnitDlg;
	if (m_pDlg && m_pDlg->Create(IDD_ICPCONUNIT_DIALOG))
	{
		m_pMainWnd = m_pDlg;
	}

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CICPCONUnitApp::ExitInstance() 
{
	CloseDebugger();
	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
LONG CICPCONUnitApp::ReadRegistryString(HKEY hKey, LPCTSTR strSection, CString &strValue)
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
void CICPCONUnitApp::OnAppExit() 
{
	if (WK_IS_WINDOW(m_pDlg))
	{
		m_pDlg->PostMessage(WM_COMMAND,(WPARAM)MAKELONG(IDOK, BN_CLICKED), NULL);
	}
}
/////////////////////////////////////////////////////////////////////////////
void _cdecl CICPCONUnitApp::OutputDebug(LPCTSTR lpszFormat, ...)
{
	if (WK_IS_WINDOW(m_pDlg) && m_pDlg->IsWindowVisible())
	{
		va_list args;
		va_start(args, lpszFormat);
		int nBuf;
		TCHAR szBuffer[1024];
		ZeroMemory(szBuffer, 1023);

		nBuf = _vstprintf(szBuffer, lpszFormat, args);

		TCHAR *psz = szBuffer;
		while (psz)
		{
			psz = _tcsstr(psz, _T("\n"));
			if (psz)
			{
				int i, nLen = _tcslen(psz);
				for (i=nLen; i>0; i--)
					psz[i] = psz[i-1];
				psz[0] = _T('\r');
				psz = &psz[2];
			}
		}
		
		m_pDlg->SetLogText(szBuffer);
		
		va_end(args);
	}
}
/////////////////////////////////////////////////////////////////////////////
LONG __stdcall MyExceptionFilter(_EXCEPTION_POINTERS *ExceptionInfo )
{
	ASSERT(FALSE);

	return EXCEPTION_EXECUTE_HANDLER;
}
/////////////////////////////////////////////////////////////////////////////
int CICPCONUnitApp::Run() 
{
	int nResult=0;
//	SetUnhandledExceptionFilter(MyExceptionFilter);
	nResult = CWinApp::Run();
//	SetUnhandledExceptionFilter(NULL);
	return nResult;
}
/////////////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0710
int CICPCONUnitApp::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
{
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
	if (dwCPbits)
	{
		SetFontFaceNamesFromCodePageBits(dwCPbits);
	}
	else
	{
		CSkinDialog::DoUseGlobalFonts(FALSE);
	}
	return 0;
}
#endif