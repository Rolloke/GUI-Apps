// NetUnit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "NetUnit.h"
#include "NetUnitDlg.h"

#include <wk_util.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNetUnitApp

BEGIN_MESSAGE_MAP(CNetUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CNetUnitApp)
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNetUnitApp construction

CNetUnitApp::CNetUnitApp()
{
	InitVideteLocaleLanguage();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CNetUnitApp object

CNetUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CNetUnitApp initialization

BOOL CNetUnitApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Rich Edit Control initialisieren
	AfxInitRichEdit();
	
	//_CrtSetBreakAlloc(6512);

#if _MFC_VER <0x070
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif
	
	InitDebugger(_T("NetUnit.log"), WAI_SOCKET_UNIT);
	
	//	WK_TRACE(_T("Version %s\n"),PLAIN_TEXT_VERSION(8, 1, 13, 0, _T("08/01/99,13:00\0"),0));
	
	if (WK_ALONE(WK_APP_NAME_SOCKETS)==FALSE) 
	{
		return FALSE;
	}
	
	//WK_ApplicationId AppID = WAI_SOCKET_UNIT;
	//CString sTitle = GetAppnameFromId(AppID);

	CString sTitle = COemGuiApi::GetApplicationName(WAI_SOCKET_UNIT);
	m_pszAppName = _tcsdup(sTitle);    
	//m_pszAppName = _tcsdup(_T("NetUnit"));    
	CWK_RunTimeError::SetDefaultID(WAI_SOCKET_UNIT);	// sets the static member

	CWK_Dongle dongle(WAI_SOCKET_UNIT);
	if (dongle.IsExeOkay()==FALSE)
	{	// already sends RunTimeError 
		return FALSE;
	}


	CNetUnitDlg* pDlg = new CNetUnitDlg();
	//pDlg->Create(IDD_NETUNIT_DIALOG);
	m_pMainWnd = pDlg;


	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}

int CNetUnitApp::ExitInstance() 
{
	CloseDebugger();
	return 0L;
}

void CNetUnitApp::OnAppExit() 
{
	m_pMainWnd->DestroyWindow();
}

#if _MFC_VER >= 0x0710
int CNetUnitApp::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
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
	CWK_String::SetCodePage(uCodePage);
	CWK_Profile wkp;
	wkp.SetCodePage(CWK_String::GetCodePage());

//	if (dwCPbits == 0)
//	{
//		dwCPbits = COemGuiApi::GetCodePageBits();
//	}
//	if (dwCPbits)
//	{
//		SetFontFaceNamesFromCodePageBits(dwCPbits);
//	}
//	else
//	{
//		CSkinDialog::DoUseGlobalFonts(FALSE);
//	}
	return 0;
}
#endif