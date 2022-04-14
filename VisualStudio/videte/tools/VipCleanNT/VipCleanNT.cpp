// VipCleanNT.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "VipCleanNT.h"
#include "VipCleanNTDlg.h"
#include "IdipCleanDlg.h"
#include "VipCleanReg.h"
#include "wkclasses\wk_wincpp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CVipCleanNTApp
BEGIN_MESSAGE_MAP(CVipCleanNTApp, CWinApp)
	//{{AFX_MSG_MAP(CVipCleanNTApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVipCleanNTApp construction
CVipCleanNTApp::CVipCleanNTApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVipCleanNTApp object
CVipCleanNTApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CVipCleanNTApp initialization
BOOL CVipCleanNTApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x0710
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif // _AFXDLL
#endif // _MFC_VER < 0x0710

//		InitDebugger(_T("cleanXP.log"),WAI_AC_DC);

	m_sStartDir = m_pszHelpFilePath;
	int nFind = m_sStartDir.ReverseFind(_T('\\'));
	if (nFind != -1)
	{
		m_sStartDir = m_sStartDir.Left(nFind+1);
	}
#ifdef _DEBUG
 #ifdef _DTS
	m_sStartDir = _T("c:\\dv");
 #else
	m_sStartDir = _T("c:\\security");
 #endif
#endif

////////////////////////////////////////////////////////////////////////////////////
#ifdef _DTS
	// automatic

	CVipCleanNTDlg VipCleanNTDlg;
	BOOL bXP = VipCleanNTDlg.IsOSVersionXP();
	if (AfxMessageBox(_T("Wollen Sie wirklich alle Bilder und Einstellungen löschen?"), MB_YESNO|MB_ICONSTOP|MB_ICONEXCLAMATION)==IDNO)
	{
		  return FALSE;
	}
	
	CVipCleanReg m_Reg;

	//Boot.ini
		 m_Reg.SetNTLoaderTimeout(0);
	//DelDir	c:\dv\mdmask1
	//			c:\dv\mdmask2
	//		  c-z:\\dvs

	//DelFiles	c:\dv\rterrors.dat
	//			c:\\Log
		 m_Reg.DelDir();
	// Del Registry
   	 m_Reg.DelKey(_T("SOFTWARE\\dvrt\\DV\\InputGroups"));
	 m_Reg.DelKey(_T("SOFTWARE\\dvrt\\DV\\OutputGroups"));
	 m_Reg.DelKey(_T("SOFTWARE\\dvrt\\DV\\Archive"));
	 m_Reg.SetKey(_T("SOFTWARE\\dvrt\\DV\\Process"),_T("PIN"),(BYTE*)NULL,0);
	 m_Reg.SetKey(_T("SOFTWARE\\dvrt\\DV\\Process"),_T("OPIN"),(BYTE*)NULL,0);

	if(!bXP)
	{
		//Autorun auf 1
		 m_Reg.SetInt(_T("SYSTEM\\CurrentControlSet\\Services\\Cdrom"), _T("Autorun"), 1);

		 //Dir Temp leeren,bzw anlegen
		 if(m_Reg.CheckTemp())
		 {
			 m_Reg.DelFiles(_T("c:\\temp"));
		 }
		 else
		 {
			  m_Reg.CreateTempDir();
		 }
	}
	 

 	// messagbox
	AfxMessageBox(_T("Werkseinstellungen wiederhergestellt!"),MB_ICONINFORMATION);

//////////////////////////////////////////////////////////////////////////////////

#else

	#ifdef _IDIP
		CIdipCleanDlg dlg;
	#else
		CVipCleanNTDlg dlg;
		if(IsFactoryReset())
		{
			//do factory reset
			CVipCleanNTDlg VipCleanNTDlg;
			BOOL bXP = VipCleanNTDlg.IsOSVersionXP();
			if (AfxMessageBox(_T("Wollen Sie wirklich alle Bilder und Einstellungen löschen?"), MB_YESNO|MB_ICONSTOP|MB_ICONEXCLAMATION)==IDNO)
			{
				  return FALSE;
			}
			
			CVipCleanReg m_Reg;

			//Boot.ini
				 m_Reg.SetNTLoaderTimeout(0);
			//DelDir	c:\dv\mdmask1
			//			c:\dv\mdmask2
			//		  c-z:\\dvs

			//DelFiles	c:\dv\rterrors.dat
			//			c:\\Log
				 m_Reg.DelDir();
			// Del Registry
   			 m_Reg.DelKey(_T("SOFTWARE\\dvrt\\DV\\InputGroups"));
			 m_Reg.DelKey(_T("SOFTWARE\\dvrt\\DV\\OutputGroups"));
			 m_Reg.DelKey(_T("SOFTWARE\\dvrt\\DV\\Archive"));

			if(!bXP)
			{
				//Autorun auf 1
				 m_Reg.SetInt(_T("SYSTEM\\CurrentControlSet\\Services\\Cdrom"),_T("Autorun"), 1);

				 //Dir Temp leeren,bzw anlegen
				 if(m_Reg.CheckTemp())
				 {
					 m_Reg.DelFiles(_T("c:\\temp"));
				 }
				 else
				 {
					  m_Reg.CreateTempDir();
				 }
			}
			 

 			// messagbox
			AfxMessageBox(_T("Werkseinstellungen wiederhergestellt!"),MB_ICONINFORMATION);		
			return FALSE;
		}
	#endif

	//do manuell configuration
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();


	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVipCleanNTApp::IsFactoryReset()
{
	//Aufruf ohne Commandozeilen Switch ist Factory Reset
	//falscher Switch ist Factory Reset

	//nur bei Switch "-CleanXP" manuelle Einstellung erlaubt
	BOOL bRet = TRUE;

	CString sComLine = GetCommandLine();
	int iSwitch = sComLine.ReverseFind(_T('-'));

	CString sSwitch = sComLine.Right(sComLine.GetLength()-iSwitch);
	CString sTitle = _T("Clean oder nicht");
	if(0 == sSwitch.CompareNoCase(_T("-cleanxp")))
	{
		bRet = FALSE;
	}
	return bRet;
}
/////////////////////////////////////////////////////////////////////////////
BOOL CVipCleanNTApp::ShellExecute(LPCTSTR sProgram)
{
	HWND hwnd = NULL;
	if (m_pMainWnd)
	{
		hwnd = m_pMainWnd->m_hWnd;
	}
	HINSTANCE hInst = ::ShellExecute(hwnd, _T("open"), sProgram, NULL, NULL, SW_SHOWNORMAL);
	if (hInst <= (HINSTANCE)32)
	{
		CString sPath = m_sStartDir +_T("\\") + sProgram;
		hInst =::ShellExecute(hwnd, _T("open"), sPath, NULL, NULL, SW_SHOWNORMAL);
	}
	return hInst > (HINSTANCE)32 ? TRUE:FALSE;
}
/////////////////////////////////////////////////////////////////////////////
int CVipCleanNTApp::ExecuteProgram(LPCTSTR sCmdline, BOOL bWait)
{
	BOOL bProcess;
	CStartupInfo			startUpInfo;
	
	CProcessInformation prozessInformation;

	startUpInfo.dwFlags			= STARTF_USESHOWWINDOW;
    startUpInfo.wShowWindow     = SW_SHOW; 


	bProcess = CreateProcess(
				(LPCTSTR)NULL,				// pointer to name of executable module 
				(LPTSTR)sCmdline,			// pointer to command line string
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to process security attributes 
				(LPSECURITY_ATTRIBUTES)NULL,// pointer to thread security attributes 
				FALSE,						// handle inheritance flag 
				NORMAL_PRIORITY_CLASS,		//|DEBUG_PROCESS|DEBUG_ONLY_THIS_PROCESS,	// creation flags 
				NULL,						// pointer to new environment block 
				NULL,						// pointer to current directory name 
				&startUpInfo,				// pointer to STARTUPINFO 
				&prozessInformation 		// pointer to PROCESS_INFORMATION  
							);

	if (bProcess && prozessInformation.hProcess != NULL)
	{
		DWORD dwWFII;
		dwWFII = WaitForInputIdle(prozessInformation.hProcess,15*1000);
		if(dwWFII == WAIT_TIMEOUT || dwWFII == -1)
		{
			bProcess = FALSE;
		}

		if (bWait)
		{
			DWORD dwWFSO = WaitForSingleObject(prozessInformation.hProcess, INFINITE);
			if(dwWFSO == WAIT_OBJECT_0)
			{
				DWORD dwExitCode = 0;
				if (GetExitCodeProcess(prozessInformation.hProcess, &dwExitCode))
				{
					bProcess = dwExitCode;
				}
			}
		}
	}

	
	return bProcess;
}
