/////////////////////////////////////////////////////////////////////////////
// PROJECT:		GenericUnit
// FILE:		$Workfile: GenericUnitApp.cpp $
// ARCHIVE:		$Archive: /Project/Units/GenericUnit/GenericUnitApp.cpp $
// CHECKIN:		$Date: 19.10.00 8:46 $
// VERSION:		$Revision: 2 $
// LAST CHANGE:	$Modtime: 19.10.00 8:27 $
// BY AUTHOR:	$Author: Martin $
// $Nokeywords:$
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "GenericUnitApp.h"
#include "GenericUnitDlg.h"
#include "OemGui\OemGuiApi.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BOOL CleanUp();

/////////////////////////////////////////////////////////////////////////////
CGenericApp theApp;


/////////////////////////////////////////////////////////////////////////////
BEGIN_MESSAGE_MAP(CGenericApp, CWinApp)
	//{{AFX_MSG_MAP(CGenericApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
CGenericApp::CGenericApp()
{
//	afxMemDF = allocMemDF | checkAlwaysMemDF;

	m_pMainWnd			= NULL;
}

/////////////////////////////////////////////////////////////////////////////
CGenericApp::~CGenericApp()
{
	WK_TRACE("CGenericApp::~CGenericApp()\n");
	m_pMainWnd  = NULL;
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGenericApp::InitInstance()
{
	WK_ApplicationId	AppID	= WAI_INVALID;

	// Instanzen zählen.
	m_pInstance = new CInstanceCounter();
	if (m_pInstance)
		AppID = m_pInstance->GetAppID();
	if (AppID == WAI_INVALID)
		return FALSE;

#if (0)
	CString sAppName = GetAppnameFromId(AppID);
	WK_ALONE(sAppName);	 
#else
	CString sAppName;
	if (GetCardRef() == 1)
		sAppName = "GenericUnit1";
	else if (GetCardRef() == 2)
		sAppName = "GenericUnit2";
	else if (GetCardRef() == 3)
		sAppName = "GenericUnit3";
	else if (GetCardRef() == 4)
		sAppName = "GenericUnit4";
	else
		WK_TRACE_ERROR("Unknown Applicationname\n");
#endif

	CWK_RunTimeError::SetDefaultID(AppID);

	// Logfilename erzeugen
	CString sLogFile = sAppName+".log";

	// Debugger öffnen
	InitDebugger(sLogFile,AppID,NULL);

	WK_TRACE("Starting GenericUnit...");
	WK_PRINT_VERSION(7, 4, 16, 31, "07/04/99,16:31\0", // @AUTO_UPDATE
		0);	// DO NOT TOUCH THIS LINE

	SET_WK_STAT_TICK_COUNT(GetTickCount());

	m_pszAppName = _tcsdup(COemGuiApi::GetApplicationName(AppID));

/*	// Dongle vorhanden?
	CWK_Dongle dongle(AppID);  
	if (dongle.IsExeOkay()==FALSE)
	{
		return FALSE;
	}
*/
	WK_STAT_LOG("Reset", 0, "Active");
	WK_STAT_LOG("Reset", 1, "Active");

	// Hauptfenster anlegen
	m_pGenericUnitDlg = new CGenericUnitDlg();

	// Und erzeugen
	if (m_pGenericUnitDlg && m_pGenericUnitDlg->Create())
	{
		m_pMainWnd = m_pGenericUnitDlg;
		return TRUE;
	}

	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
int CGenericApp::ExitInstance()
{
	WK_TRACE("ExitInstance\n");

	// Den Rest aufräumen.
	CleanUp();

	return m_msgCur.wParam; // returns the value from PostQuitMessage
}

/////////////////////////////////////////////////////////////////////////////
BOOL CGenericApp::CleanUp()
{
	WK_TRACE("CleanUp\n");

	// Instanzen-objekt zerstören
	WK_DELETE(m_pInstance);

	// Debugger schließen
	CloseDebugger();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
WK_ApplicationId CGenericApp::GetApplicationId()
{
	WK_ApplicationId AppId = WAI_INVALID;

	if (m_pInstance)
		AppId = m_pInstance->GetAppID();
		
	return AppId;
}
