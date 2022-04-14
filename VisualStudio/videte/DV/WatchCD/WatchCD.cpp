// WatchCD.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "WatchCD.h"
#include "WatchCDDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CWatchCDApp

BEGIN_MESSAGE_MAP(CWatchCDApp, CWinApp)
	//{{AFX_MSG_MAP(CWatchCDApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWatchCDApp construction

CWatchCDApp::CWatchCDApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CWatchCDApp object

CWatchCDApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CWatchCDApp initialization

BOOL CWatchCDApp::InitInstance()
{
	if (WK_ALONE(WK_APP_NAME_DV_WATCH_CD))
	{
		// first instance
	}
	else
	{
		// second or more instance
		return FALSE;
	}
	// Standard initialization

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	//prüfe, welche DirectCD Version installiert ist. Version 3 oder 5
	
	CheckDirectCDVersion();	//setzt Variable m_iDirectCDVersion

	InitDebugger(_T("WatchCD.log"),WAI_DV_WATCH_CD);

	if(m_iDirectCDVersion == DCD_VERSION_30)
	{
		WK_TRACE(_T("** DirectCD Version: 3.x **\n"));
		CWatchCDDlg* pDlg = new CWatchCDDlg;
		m_pMainWnd = pDlg;
		pDlg->ShowWindow(SW_SHOW);
	} 
	else if(m_iDirectCDVersion == DCD_VERSION_50)
	{
		WK_TRACE(_T("** DirectCD Version: 5.0 **\n"));
		CWatchCDDlg* pDlg = new CWatchCDDlg;
		m_pMainWnd = pDlg;
		pDlg->ShowWindow(SW_SHOW);
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CWatchCDApp::OnIdle(LONG lCount) 
{
	((CWatchCDDlg*)m_pMainWnd)->OnIdle();

	return CWinApp::OnIdle(lCount);
}
/////////////////////////////////////////////////////////////////////////////
int CWatchCDApp::ExitInstance() 
{
	CloseDebugger();

	return 0;
}
/////////////////////////////////////////////////////////////////////////////
//prüfe, welche DirectCD Version installiert ist. Version 3 oder 5
void CWatchCDApp::CheckDirectCDVersion()
{
	m_iDirectCDVersion = DCD_VERSION_UNKNOWN;
	CWK_Profile wkProf(CWK_Profile::WKP_NTLOGON, NULL, _T("Adaptec"),_T(""));
	
	CString sDefault(_T("default"));
	CString sVersion = wkProf.GetString(_T("DirectCD"), _T("versionLabel"), sDefault);
	if(sVersion != sDefault)
	{
		CString sVersionShort = sVersion.Left(1);
		if(sVersionShort == _T("5"))
		{
			m_iDirectCDVersion = DCD_VERSION_50;
		}
		else if(sVersionShort == _T("3"))
		{
			m_iDirectCDVersion = DCD_VERSION_30;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
//liefert die installierte DirectCD Version zurück. 3 oder 5
int CWatchCDApp::GetDirectCDVersion()
{
	return m_iDirectCDVersion;
}
