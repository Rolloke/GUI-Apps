// EwfControl.cpp : Definiert das Klassenverhalten für die Anwendung.
//

#include "stdafx.h"
#include "EwfControl.h"
#include "EwfControlDlg.h"
#include "wk_names.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif




// CEwfControlApp

BEGIN_MESSAGE_MAP(CEwfControlApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CEwfControlApp-Erstellung

CEwfControlApp::CEwfControlApp()
{
#ifdef _DEBUG
	SetThreadLocale(MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_NEUTRAL), 0));
#else
	InitVideteLocaleLanguage();
#endif
}


// Das einzige CEwfControlApp-Objekt

CEwfControlApp theApp;


// CEwfControlApp Initialisierung

BOOL CEwfControlApp::InitInstance()
{
	CString sCmdLine(m_lpCmdLine);
	if (sCmdLine.Find(_T("/?")) != -1)
	{
		CString str = 
		_T("CmdLine parameters are:\n\n")
		_T("[c: -a -ls:XXXX] ewf commands\n")
        _T("c: drive letter\n")
		_T("-a asynchronus low space notification, for DISK overlay only\n")
		_T("-ls:XXXX threshold for low (remaining) space for notification\n\n")
        _T("-s run silent even if an error occurs\n")
        _T("/? this help as message box");

		AfxMessageBox(str, MB_OK|MB_ICONINFORMATION);
		return FALSE;
	}
	if (WK_ALONE(WK_APP_NAME_EWF_CONTROL)==FALSE)
	{
		return FALSE;
	}
		
	CWinApp::InitInstance();

	BOOL bEnable = AfxEnableMemoryTracking(FALSE);
	if (m_pszRegistryKey)
	{
		free((void*)m_pszRegistryKey);
	}
	if (m_pszProfileName)
	{
		free((void*)m_pszProfileName);
	}

	CString sHelp(m_pszHelpFilePath);
	int nFind = sHelp.ReverseFind(_T('\\'));
	if (nFind != -1)
	{
		sHelp = sHelp.Left(nFind+1) + PROFILE_VIDETE_CHEKC_DISK;
	}
	m_pszProfileName = _tcsdup(sHelp);
	AfxEnableMemoryTracking(bEnable);

	CString sLogfile = m_pszAppName;
	sLogfile += _T(".Log");
	InitDebugger(sLogfile, WAI_INVALID);


	CEwfControlDlg *pDlg = new CEwfControlDlg();
	m_pMainWnd = pDlg;
	if (sCmdLine.GetLength()>1)
	{
		sCmdLine.MakeUpper();
		if (isalpha(sCmdLine.GetAt(0)) && sCmdLine.GetAt(1) == _T(':'))
		{
			pDlg->m_cProtectedDrive = sCmdLine.GetAt(0);
		}
		if (sCmdLine.Find(_T("-S")) != -1)
		{
			pDlg->m_bSilent = TRUE;
		}
		if (sCmdLine.Find(_T("-A")) != -1)
		{
			pDlg->m_bAsync = TRUE;
		}
		int nSpace = sCmdLine.Find(_T("-LS:"));
		if (nSpace != -1)
		{
			nSpace = _ttoi(sCmdLine.Mid(nSpace+4));
			pDlg->m_llLowSpace = nSpace;
		}
	}

	pDlg->Create(CEwfControlDlg::IDD);

	// Da das Dialogfeld geschlossen wurde, FALSE zurückliefern, so dass wir die
	//  Anwendung verlassen, anstatt das Nachrichtensystem der Anwendung zu starten.
	return TRUE;
}

int CEwfControlApp::ExitInstance()
{
	CloseDebugger();

	return CWinApp::ExitInstance();
}

