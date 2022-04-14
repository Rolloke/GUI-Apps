// FindFilter.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FindFilter.h"
#include "FindFilterDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CFindFilterApp

BEGIN_MESSAGE_MAP(CFindFilterApp, CWinApp)
	//{{AFX_MSG_MAP(CFindFilterApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindFilterApp construction

CFindFilterApp::CFindFilterApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFindFilterApp object

CFindFilterApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFindFilterApp initialization

BOOL CFindFilterApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER >= 0x0710
#else
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif // _AFXDLL
#endif // _MFC_VER

   DWORD dwDummy = 0;
   _TCHAR  szFileName[_MAX_PATH];
   GetModuleFileName(NULL, szFileName, _MAX_PATH);
   int   bytes = GetFileVersionInfoSize(szFileName, &dwDummy);
   int nVersion = 1;
   int nSubVers = 0;
   if(bytes)
   {
      void * buffer = new char[bytes];
      GetFileVersionInfo(szFileName, 0, bytes, buffer);
      VS_FIXEDFILEINFO * Version;
      unsigned int     length;
      if(VerQueryValue(buffer,_T("\\"),(void **) &Version, &length))
      {
         nVersion = HIWORD(Version->dwFileVersionMS);
         nSubVers = LOWORD(Version->dwFileVersionMS);
         if ((nSubVers > 0) && (nSubVers < 10)) nSubVers *= 10;
      }
      delete[] buffer;
   }
   SetRegistryKey(_T("Videte"));                                    // Firmenname
   if (m_pszProfileName)
   {
		CString sFmt;
		sFmt.Format(_T("%s\\V%d.%02d"), m_pszProfileName, nVersion, nSubVers);
		free((void*)m_pszProfileName);
		m_pszProfileName = _tcsdup(sFmt);
   }

	CFindFilterDlg dlg;
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

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
