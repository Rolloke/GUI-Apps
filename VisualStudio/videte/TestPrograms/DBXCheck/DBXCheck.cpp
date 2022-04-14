/* GlobalReplace: pReportBox --> g_pReportBox */
// DBXCheck.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DBXCheck.h"
#include "DBXCheckDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



/////////////////////////////////////////////////////////////////////////////
// CDBXCheckApp

BEGIN_MESSAGE_MAP(CDBXCheckApp, CWinApp)
	//{{AFX_MSG_MAP(CDBXCheckApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDBXCheckApp construction

CDBXCheckApp::CDBXCheckApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDBXCheckApp object

CDBXCheckApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDBXCheckApp initialization

BOOL CDBXCheckApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif


	InitDebugger("DBXCheck.log",WAI_INVALID);	// WK_TODO

	// WK_TODO CString sTitle = COemGuiApi::GetApplicationName(WAI_INVALID);
	// WK_TODO m_pszAppName = _tcsdup(sTitle);    // human readable title


	CDBXCheckDlg dlg;
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

int CDBXCheckApp::ExitInstance()
{
	CloseDebugger();

	return CWinApp::ExitInstance();
}


CListBox *g_pReportBox = NULL;

#define MAX_DEBUG_STRING_LEN 1000
void __cdecl MY_WK_TRACE(LPCSTR lpszFormat, ...)
{
	char szString[MAX_DEBUG_STRING_LEN];
	szString[0]=0;
	LPCSTR lpszLocalFormat = lpszFormat;

	va_list args;
	va_start(args, lpszFormat);

	_vsnprintf(szString, MAX_DEBUG_STRING_LEN-1, lpszLocalFormat, args);
	CString s(szString);
	WK_TRACE("%s",s);	// call the normal WK_TRACE , NOT YET WK_TRACE_INTERN

	if (g_pReportBox && IsWindow(g_pReportBox->m_hWnd)) {
		// always insert at the end, we need the count of existing items
		int iCount = g_pReportBox->GetCount();
		if (iCount>500) {
			g_pReportBox->DeleteItem(0);
		}

		if (szString[strlen(szString)-1]=='\n') {
			szString[strlen(szString)-1]=0;
		}

		int ix = g_pReportBox->AddString(szString);
		if (iCount>20) {
			g_pReportBox->SetTopIndex(ix);
		}
	}
}
