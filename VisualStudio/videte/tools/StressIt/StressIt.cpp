/////////////////////////////////////////////////////////////////////////////
// PROJECT:		StressIt
// FILE:		$Workfile: StressIt.cpp $
// ARCHIVE:		$Archive: /Project/Tools/StressIt/StressIt.cpp $
// CHECKIN:		$Date: 5.08.98 9:56 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 5.08.98 9:56 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "StressIt.h"
#include "StressItDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStressItApp

BEGIN_MESSAGE_MAP(CStressItApp, CWinApp)
	//{{AFX_MSG_MAP(CStressItApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CStressItApp construction

CStressItApp::CStressItApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CStressItApp object

CStressItApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CStressItApp initialization

BOOL CStressItApp::InitInstance()
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

	CStressItDlg dlg;
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
