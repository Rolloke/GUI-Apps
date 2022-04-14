// FileOpen.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "FileOpen.h"
#include "FileOpenDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFileOpenApp

BEGIN_MESSAGE_MAP(CFileOpenApp, CWinApp)
	//{{AFX_MSG_MAP(CFileOpenApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFileOpenApp construction

CFileOpenApp::CFileOpenApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CFileOpenApp object

CFileOpenApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CFileOpenApp initialization

BOOL CFileOpenApp::InitInstance()
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

	CFileOpenDlg dlg;
	m_pMainWnd = &dlg;

	char szFilter[] = "DBF Files (*.dbf)|*.dbf|All Files (*.*)|*.*||";

	CString sName;

	// CFileDialog( BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, 
	//				LPCTSTR lpszFileName = NULL, 
	//				DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL );

	CFileDialog myDialog(TRUE, "*.dbf", NULL, 0, szFilter);
	if (myDialog.DoModal()==IDOK) {
		// sName = myDialog.GetPathName();
		sName = myDialog.GetFileName();
	
		if (sName.GetLength()) {
			dlg.m_sFileName = sName;

			CFile f;
			if (f.Open(sName,CFile::modeRead )) {
				dlg.m_bReadOnly = TRUE;
				f.Close();
			} else {
				dlg.m_bReadOnly = FALSE;
			}

			if (f.Open(sName,CFile::modeReadWrite) ) {
				dlg.m_bReadWrite = TRUE;
				f.Close();
			} else {
				dlg.m_bReadWrite = FALSE;
			}

		}
	}

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
