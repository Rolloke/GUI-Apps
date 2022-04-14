// EmailUnit.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "EmailUnit.h"
#include "EmailUnitDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEmailUnitApp

BEGIN_MESSAGE_MAP(CEmailUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CEmailUnitApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEmailUnitApp construction

CEmailUnitApp::CEmailUnitApp()
{
	InitVideteLocaleLanguage();
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CEmailUnitApp object

CEmailUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CEmailUnitApp initialization

BOOL CEmailUnitApp::InitInstance()
{
	if (!WK_ALONE(WK_APPNAME_EMAILUNIT))
	{
		TRACE(_T("no second instance\n"));
		return FALSE;
	}

	InitDebugger(_T("Email.log"),WAI_EMAIL_UNIT);

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER >= 0x0710
	SetLanguageParameters(0, 0);
#else
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	if (!AfxSocketInit())
	{
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	m_nCmdShow = SW_HIDE;
	
	CEmailUnitDlg dlg;
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

int CEmailUnitApp::ExitInstance() 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CloseDebugger();

	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0710
int CEmailUnitApp::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
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