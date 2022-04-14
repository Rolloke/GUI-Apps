// ProductView.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ProductView.h"
#include "ProductViewDlg.h"
#include "ErrorDialog.h"
#include "PinDialog.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProductViewApp

BEGIN_MESSAGE_MAP(CProductViewApp, CWinApp)
	//{{AFX_MSG_MAP(CProductViewApp)
	ON_COMMAND(ID_HIDDEN_WRITE, OnHiddenWrite)
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProductViewApp construction

CProductViewApp::CProductViewApp()
{
#ifdef _DEBUG
	SetThreadLocale(MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_NEUTRAL), 0));
#else
	InitVideteLocaleLanguage();
#endif

	WK_CheckTempDirectory();
	m_bSaveEnabled  = FALSE;
	m_bCryptEnabled = FALSE;
	m_bShowEnabled  = TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CProductViewApp object

CProductViewApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CProductViewApp initialization
static void MySplitCommandLine(const CString &sCommandLine, CStringArray& saParameters)
{
	CString sCL = sCommandLine;
	CString sOne;
	int p;

	sCL.TrimLeft();
	p = sCL.Find(' ');
	while (p!=-1)
	{
		sOne = sCL.Left(p);
		sCL = sCL.Mid(p+1);
		sCL.TrimLeft();
		saParameters.Add(sOne);
		p = sCL.Find(' ');
	}
	if (!sCL.IsEmpty())
	{
		saParameters.Add(sCL);
	}
}


void CProductViewApp::ParseArgs(const CStringArray &saParameters)
{
	CStringArray args;	// sigh no copy constr
	for (int i=0;i<saParameters.GetSize();i++) 
	{
		args.Add(saParameters[i]);
	}

	while (args.GetSize()) 
	{
		const CString sArg=args[0];
		if (sArg==_T("-enableSave")) 
		{
			args.RemoveAt(0);
			m_bSaveEnabled = TRUE;
		} 
		else if (sArg==_T("-enableCrypt")) 
		{
			args.RemoveAt(0);
			m_bCryptEnabled = TRUE;
			m_bSaveEnabled = TRUE;
		} 
		else if (sArg==_T("-noShow")) 
		{
			args.RemoveAt(0);
			m_bShowEnabled=FALSE;
		} 
		else if (sArg==_T("-load")) 
		{
			if (args.GetSize()>1) 
			{
				m_sFileToUse = args[1];
				args.RemoveAt(1);
			}
			args.RemoveAt(0);
		} 
		else 
		{
			if (args[0].Find(_T(".dcf"))!=-1) 
			{
				m_sFileToUse = args[0];
				if (m_sFileToUse.GetLength() && m_sFileToUse[0]==_T('\"')) 
				{
					m_sFileToUse = m_sFileToUse.Right(m_sFileToUse.GetLength()-1);
				}
				if (m_sFileToUse.GetLength() && m_sFileToUse[m_sFileToUse.GetLength()-1]==_T('\"')) 
				{
					m_sFileToUse = m_sFileToUse.Left(m_sFileToUse.GetLength()-1);
				}
				WK_TRACE(_T("FileToUse '%s'\n"), m_sFileToUse);

				m_bSaveEnabled = TRUE;	// auto enable
			}
			args.RemoveAt(0);
		}
	}
}

BOOL CProductViewApp::InitInstance()
{
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.
	if (!WK_ALONE(GetAppnameFromId(WAI_PRODUCT_VIEW)))
		return FALSE;

#if _MFC_VER < 0x0700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	CString sTitle = COemGuiApi::GetApplicationName(WAI_PRODUCT_VIEW);
	m_pszAppName = _tcsdup(sTitle);    // human readable title

	InitDebugger(_T("ProductView.log"), WAI_PRODUCT_VIEW);

	CStringArray saParameters;
	MySplitCommandLine(m_lpCmdLine, saParameters);

	ParseArgs(saParameters);

	if (IsCryptEnabled())
	{
#ifndef _DEBUG
		CPinDialog pindialog;
		
		if (IDOK == pindialog.DoModal())
		{
			CSystemTime t;
			CString s;
			t.GetLocalTime();
			s = t.GetDBTime().Left(4);
			if (pindialog.m_sPin != s)
			{
				AfxMessageBox(IDP_CRYPT_WRONG);
				return FALSE;
			}
		}
		else
		{
			AfxMessageBox(IDP_CRYPT_WRONG);
			return FALSE;
		}
#endif
	}

	CProductViewDlg dlg;	// CAVEAT create here, the dongle is member of the dialog

	if (m_bShowEnabled) 
	{
		BOOL b = TRUE;


		if (b||dlg.m_dongle.IsValid()) {
			m_pMainWnd = &dlg;	// OOPS here

			int nResponse = dlg.DoModal();
			if (nResponse == IDOK) {
				//  dismissed with OK
			}
			else if (nResponse == IDCANCEL) {
				//  dismissed with Cancel
			}
		} else {
			// NOT YET invalid message
			CErrorDialog errorDialog;

			m_pMainWnd = &errorDialog;	// OOPS here
			errorDialog.m_sErrorMsg = CWK_Dongle::NameOfDongleState( 
				dlg.m_dongle.GetHardlockState() 
				);
			errorDialog.DoModal();
		}
	} else {
		// no show, transfer to registry
		dlg.m_dongle.WriteToRegistry();
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	// application, rather than start the application's message pump.
	return FALSE;
}

int CProductViewApp::ExitInstance() 
{
	CloseDebugger();
	
	return CWinApp::ExitInstance();
}

void CProductViewApp::OnHiddenWrite() 
{
	TRACE(_T("Write...\n"));	
}
