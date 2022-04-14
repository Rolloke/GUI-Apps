// SimUnit.cpp : Defines the class behaviors for the application.
// w+k Sample Application
// Author : Uwe Freiwald
//
// CSimUnitApp encapsulates all code for windows application
// and should simulate a possible hardware device such as ISA cards
// or devices connected to IO ports LPTx or COMx.


#include "stdafx.h"
#include "SimUnit.h"
#include "SimUnitDlg.h"
#include "CIPCInputSimUnit.h"
#include "CIPCOutputSimUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSimUnitApp

BEGIN_MESSAGE_MAP(CSimUnitApp, CWinApp)
	//{{AFX_MSG_MAP(CSimUnitApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSimUnitApp construction

CSimUnitApp::CSimUnitApp()
{
	InitVideteLocaleLanguage();
	// set all 'globals' to default
	m_pCIPCInput   = NULL;
	m_pCIPCOutput  = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CSimUnitApp object

CSimUnitApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CSimUnitApp initialization

BOOL CSimUnitApp::InitInstance()
{
	if (WK_ALONE(WK_APP_NAME_SIMUNIT)==FALSE)
	{
		return FALSE;
	}

	CString sTitle = COemGuiApi::GetApplicationName(WAI_SIMUNIT);
	m_pszAppName = _tcsdup(sTitle);    // human readable title
	XTIB::SetThreadName(m_pszAppName);

	InitDebugger(_T("SimUnit.log"), WAI_SIMUNIT);

	// Dongle vorhanden?
	CWK_Dongle dongle(WAI_SIMUNIT); 
	if (dongle.IsExeOkay()==FALSE)
	{
		return FALSE;
	}
	
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

/*
	CString sIniFile = m_pszHelpFilePath;
	int nSlash = sIniFile.ReverseFind(_T('\\'));
	if (nSlash != -1)
	{
		CString sApplicationDir = sIniFile.Left(nSlash+1);
		sIniFile = sApplicationDir + OEM_FILE;
	}
#ifdef _DEBUG
	CString sDebugInifile = _T("C:\\Security\\")  OEM_FILE;
	if (::GetFileAttributes(sDebugInifile) != INVALID_FILE_ATTRIBUTES)
	{
		sIniFile = sDebugInifile;
	}
#endif

	WK_TRACE(_T("Load OEM %s\n"), sIniFile);
	CWK_Profile wkpOEM(CWK_Profile::WKP_INIFILE_ABSOLUT, NULL, _T(""), sIniFile); 
	COemGuiApi::LoadOEMSkinOptions(wkpOEM);
	CSkinDialog::SetButtonImages(SC_CLOSE    , (UINT)-1, 0, 0);
	CSkinDialog::SetButtonImages(SC_MAXIMIZE , (UINT)-1, 0, 0);
	CSkinDialog::SetButtonImages(SC_MINIMIZE , (UINT)-1, 0, 0);
	CSkinDialog::SetButtonImages(SC_MOUSEMENU, (UINT)-1, 0, 0, LoadIcon(IDR_MAINFRAME));
	CSkinDialog::LoadButtonImages();
*/
	// create main application window (modeless dialog)
	m_pDialog = new CSimUnitDlg(this);

	// create Input object
	m_pCIPCInput = new CIPCInputSimUnit(SM_SIMUNIT_INPUT);

	// create Output object
	m_pCIPCOutput = new CIPCOutputSimUnit(m_pDialog,SM_SIMUNIT_OUTPUT);
	
	if (!m_pCIPCInput || !m_pCIPCOutput)
	{
		AfxMessageBox(IDP_NOCONNECTION,MB_ICONSTOP|MB_OK);
		return FALSE;
	}

	m_pMainWnd = m_pDialog;
	
	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
int CSimUnitApp::ExitInstance() 
{
	// delete both input and output objects
	WK_DELETE(m_pCIPCInput);
	WK_DELETE(m_pCIPCOutput);
	
	return CWinApp::ExitInstance();
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitApp::Terminate() 
{
	m_pDialog->DestroyWindow();
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitApp::DoAlert(int nID)
{
	CString sMessage;
	char szBuf[3];
	
	m_pCIPCInput->IndicateAlarm(nID);

	sMessage.LoadString(IDS_ALARM_ON);
	sMessage += itoa(nID,szBuf,10);
	m_pDialog->InputHistory(sMessage);
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitApp::UndoAlert(int nID)
{
	CString sMessage;
	char szBuf[3];

	m_pCIPCInput->IndicateAlarm(nID,TRUE);

	sMessage.LoadString(IDS_ALARM_OFF);
	sMessage += itoa(nID,szBuf,10);
	m_pDialog->InputHistory(sMessage);
}
/////////////////////////////////////////////////////////////////////////////
void CSimUnitApp::DoRelay(int iNr, BOOL bState)
{

}
/////////////////////////////////////////////////////////////////////////////
BOOL CSimUnitApp::OnIdle(LONG lCount) 
{
	// TODO: Add your specialized code here and/or call the base class
	BOOL bReturn = FALSE;
	// TODO: Add your specialized code here and/or call the base class
	if (WK_IS_WINDOW(m_pDialog))
	{
		bReturn = m_pDialog->HasRandomAlarms();
		if (bReturn)
		{
			m_pDialog->CheckAlarms();
			Sleep(10);
		}
	}
//	TRACE(_T("OnIdle %i\n"), bReturn);
	return bReturn;
}
/////////////////////////////////////////////////////////////////////////////////
#if _MFC_VER >= 0x0710
int CSimUnitApp::SetLanguageParameters(UINT uCodePage, DWORD dwCPbits)
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