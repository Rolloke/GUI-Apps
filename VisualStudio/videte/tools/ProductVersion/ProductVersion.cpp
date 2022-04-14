// ProductVersion.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "ProductVersion.h"
#include "ProductVersionDlg.h"

#include "wk_names.h"

#include "WKClasses/wk_trace.h"
#include "WKClasses/wk_file.h"
#include "WKClasses/wk_util.h"

#include "OemGui/OemGuiApi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProductVersionApp

BEGIN_MESSAGE_MAP(CProductVersionApp, CWinApp)
	//{{AFX_MSG_MAP(CProductVersionApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProductVersionApp construction

CProductVersionApp::CProductVersionApp()
{ 
#ifdef _DEBUG
	SetThreadLocale(MAKELCID(MAKELANGID(LANG_GERMAN, SUBLANG_NEUTRAL), 0));
#else
	InitVideteLocaleLanguage();
#endif
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CProductVersionApp object

CProductVersionApp theApp;

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

void CProductVersionApp::ParseArgs(const CStringArray &saParameters)
{
	CStringArray args;	// sigh no copy constr
	for (int i=0;i<saParameters.GetSize();i++) {
		args.Add( saParameters[i]);
	}
	while (args.GetSize()) {
		const CString sArg=args[0];
		if (sArg==_T("-updateDirectory")) {
			if (args.GetSize()>1) {
				m_sDirForCreate = args[1];
				args.RemoveAt(1);
			}
			args.RemoveAt(0);
		} else if (sArg.Find(_T(".pvi"))!=-1 || sArg.Find(_T(".PVI"))!=-1) {
			m_sFileToRead = sArg;

			// remove stupid "file"
			if (m_sFileToRead.GetLength() && m_sFileToRead[0]==_T('\"')) {
				m_sFileToRead = m_sFileToRead.Mid(1);
			}
			if (m_sFileToRead.GetLength()>1
				&& m_sFileToRead[m_sFileToRead.GetLength()-1]==_T('\"')) {
				m_sFileToRead = m_sFileToRead.Left(m_sFileToRead.GetLength()-1);
			}

			args.RemoveAt(0);
		} else {
			args.RemoveAt(0);
		}
	}
}
/////////////////////////////////////////////////////////////////////////////
// CProductVersionApp initialization

BOOL CProductVersionApp::InitInstance()
{
	if ( WK_ALONE(WK_APP_NAME_PRODUCT_VERSION)==FALSE )
	{
		// only one instance (!)
		return FALSE;
	}
	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if _MFC_VER < 0x700
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif
#endif

	/*
	CString sTitle = COemGuiApi::GetApplicationName(WAI_PRODUCT_VERSION);
	m_pszAppName = _tcsdup(sTitle);    // human readable title*/

	InitDebugger(_T("ProductVersion.log"), WAI_PRODUCT_VERSION );

	CStringArray saParameters;
	MySplitCommandLine(m_lpCmdLine, saParameters);

	ParseArgs(saParameters);

	if (m_sDirForCreate.GetLength()) 
	{
		int dwBuild = 1;

		if (DoesFileExist(m_sDirForCreate)==FALSE) {
			AfxMessageBox(_T("Directory not found!?"));
			return FALSE;	// <<< EXIT >>>
		}

		CString sOut;
		sOut = m_sDirForCreate;
		sOut += _T("\\vinfo.pvi");

		if (DoesFileExist(sOut)) {
			CWK_VersionDB oldDB;
			oldDB.ReadFromFile(sOut);
			dwBuild = oldDB.GetBuildNumber()+1;
			//
			// move vinfo.pvi to vinfoBuild%d.pvi
			CString sMovedName;
			CTime ct = CTime::GetCurrentTime();
			sMovedName.Format(_T("%s\\vinfo%d_%d%02d%02d.pvi"),
				m_sDirForCreate,
				oldDB.GetBuildNumber(),
				ct.GetYear(),
				ct.GetMonth(),
				ct.GetDay()
				);
			TRY {
				CFile::Rename( sOut, sMovedName);
			}
			CATCH( CFileException, e ) {
				WK_TRACE_ERROR(_T("Unable to move %s to %s\n"),
					sOut, 
					sMovedName
					);
			}
			END_CATCH
		}
		
		CWK_VersionDB newVersions;
		
		BOOL bIgnoreOS = TRUE;
		newVersions.ScanDirectory(dwBuild,m_sDirForCreate, bIgnoreOS);

		newVersions.WriteToFile(sOut);
		WK_TRACE(_T("Wrote %s, build is %d\n"),sOut,dwBuild);
	} 
	else 
	{
		CProductVersionDlg dlg;	// empty is the default
		m_pMainWnd = &dlg;
		// UNUSED int nResponse = 
		dlg.DoModal();
		// ignore nResponse
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

int CProductVersionApp::ExitInstance() 
{
	CloseDebugger();
	
	return CWinApp::ExitInstance();
}
